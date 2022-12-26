#include "LevelEditor.h"

std::vector<std::string> m_fresh_logs_vec;
int last_line_count_index = 0;
void onModifty(std::string file_path_) {
	std::ifstream logFileRead(file_path_);
	std::string my_text;
	int current_line_count = 0;
	while (std::getline(logFileRead, my_text))
	{
		if (current_line_count >= last_line_count_index) {
			m_fresh_logs_vec.push_back(my_text);
			last_line_count_index++;
		}
		current_line_count++;
	}
	logFileRead.close();
}


LevelEditor::LevelEditor()
{
}

int LevelEditor::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int LevelEditor::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void LevelEditor::build()
{
	SBX_INFO("Level Editor Screen loaded into memory");
}

void LevelEditor::onEntry()
{
	m_sprite_batch.init();
	m_debug_renderer.init();
	m_utilities.init();

	//Texture Program
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();

	//Complie our light shader
	m_light_program.compileShaders("Assets/Shaders/lightShading.vert", "Assets/Shaders/lightShading.frag");
	m_light_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
	m_light_program.addAttribute("vertexColor");
	m_light_program.addAttribute("vertexUV");
	m_light_program.linkShaders();

	//the debugProgram
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertexColor");
	m_debug_program.linkShaders();

	//the layer shells
	/* these are the layer types that the editor supports*/
	m_vec_of_layer_types.push_back(new FlatLayer());
	m_vec_of_layer_types.push_back(new TiledLayer());
	m_vec_of_layer_types.push_back(new IsometricLayer());
	layer_labels_ptr = new const char* [m_vec_of_layer_types.size()];
	for each (auto & layer_type in m_vec_of_layer_types)
	{
		layer_type->onInit();
	}


	initGUI();
	//create New Layer
	m_universal_camera_position = glm::vec2(0);
	/* its important for the camera default to be 0 because that is where the tiled layers center their grids at, at 0,0*/
	createNewLayer();
	/*This is currently giving ,me a thread execption , TO be debugged later*/
	//std::thread my_thread_object(FolderChangeTracker(),"test", onModifty);
	//my_thread_object.detach();
}

void LevelEditor::update(const float& deltaTime_)
{
	SquareBox::GWOM::Layer& active_layer = m_layers[m_active_layer_index];
	m_universal_camera_scale = active_layer.camera.getScale();
	m_universal_camera_position = active_layer.camera.getPosition();

	for (unsigned int i = 0; i < m_layers.size(); i++)
	{

		/* maintaning the same properties ensures that we have the same selectable world*/
		m_layers[i].camera.setScale(m_universal_camera_scale);
		m_layers[i].camera.setPosition(m_universal_camera_position);
		//update all camera so that there is no jump when we change layers
		m_layers[i].camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	}


	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
	{
		SDL_Event evnt;
		//stop our game engine from processing input
		m_game_ptr->setProcessingInput(false);
		while (SDL_PollEvent(&evnt))
		{
			//hand over all the events to imGUI
			ImGui_ImplSDL2_ProcessEvent(&evnt);
		}
	}
	else {
		m_game_ptr->setProcessingInput(true);
	}


	if (m_game_ptr->isProcessingInput()) {

		for each (auto & layer_type in m_vec_of_layer_types)
		{
			if (active_layer.layer_type == layer_type->layer_type) {
				layer_type->onUpdateProcessingInput(deltaTime_, m_layers, m_active_layer_index, m_game_ptr, m_editor_mode_enum);
				break;
			}
		}

		m_editor_assitant.cameraControls(m_layers[m_active_layer_index].camera, m_game_ptr);

		//forcing imgui to start responding
		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::ESCAPE))) {
			ImGui::SetNextFrameWantCaptureMouse(true);
			ImGui::SetNextFrameWantCaptureKeyboard(true);
			/* this is the hack that i have come up with to solve the issue of imgui freezing up
			and not allowing any more user input, this block of code  forces it to start
			taking user input but it disoragnises sdls input manager
			so this function only helps you be able to save your work */
		}
	}

	//operations done regardless what is processing input
	for each (auto & layer_type in m_vec_of_layer_types)
	{
		if (active_layer.layer_type == layer_type->layer_type) {
			layer_type->onUpdateWithOrWithoutProcessing(deltaTime_, m_layers, m_active_layer_index, m_game_ptr, m_editor_mode_enum);
			break;
		}
	}
}

void LevelEditor::draw()
{
	//we shall draw layer by layer starting with the first one

	for (unsigned int i = 0; i < m_layers.size(); i++) {
		SquareBox::GWOM::Layer& layer = m_layers[i];
		if (layer.is_visible) {
			//contents Draw
			//TEXTURES
			m_sprite_batch.begin(SquareBox::RenderEngine::NONE);//helps us respect the order we already have predifined in the m_alive_cluster_objects class

			for each (auto & layer_type in m_vec_of_layer_types)
			{
				if (layer.layer_type == layer_type->layer_type) {
					layer_type->onContentDraw(layer, m_sprite_batch);
					break;
				}
			}

			m_sprite_batch.end();
			m_texture_program.use();
			preUpdateShader(&m_texture_program, "mySampler");
			uploadCameraInfo(&m_texture_program, &layer.camera, "P");
			m_sprite_batch.renderBatch();
			m_texture_program.unuse();

			//Debug Draw
			bool should_debug_draw_this_layer = false;
			if (m_debug_mode) {
				if (m_show_all_debug_lines) {
					should_debug_draw_this_layer = true;
				}
				else if (layer.index == m_active_layer_index) {
					should_debug_draw_this_layer = true;
				}
			}

			if (should_debug_draw_this_layer) {
				m_debug_renderer.begin();

				for each (auto & layer_type in m_vec_of_layer_types)
				{
					if (layer.layer_type == layer_type->layer_type) {
						layer_type->onDebugDraw(layer, m_debug_renderer);
						break;
					}
				}

				m_debug_renderer.end();
				//render all our debug shapes
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &layer.camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();
			}

			//LIGHTS
			m_game_ptr->getWindow()->useAdditiveBlending();

			m_sprite_batch.begin();

			for each (auto & layer_type in m_vec_of_layer_types)
			{
				if (layer.layer_type == layer_type->layer_type) {
					layer_type->onLightDraw(layer, m_sprite_batch);
					break;
				}
			}

			m_sprite_batch.end();
			m_light_program.use();
			uploadCameraInfo(&m_light_program, &layer.camera, "P");
			m_sprite_batch.renderBatch();
			m_light_program.unuse();
			m_game_ptr->getWindow()->useAlphaBlending();

			if (layer.index == m_active_layer_index) {
				/* a better option that the checking if this is the active layer is sending down the active layer index in the
					onOutsideDebugModeDebugDraw so that the object trace is only drawn for the active layer but the other things
					that are not limited to the active layer  still get drawn
				*/
				//the out laws
				m_debug_renderer.begin();

				for each (auto & layer_type in m_vec_of_layer_types)
				{
					if (layer.layer_type == layer_type->layer_type) {

						layer_type->onOutsideDebugModeDebugDraw(layer, m_game_ptr, m_debug_renderer, m_editor_mode_enum);
						break;
					}
				}

				m_debug_renderer.end();
				//render all our debug shapes
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &layer.camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();

			}


		}
	}

	drawGUI();
}

void LevelEditor::onExit()
{
	if (!m_allow_exit)
	{
		m_game_ptr->abortExit();//abort normal exit and first see request for permission to exit
		m_request_exit = true;//this will lead to the are u sure dialog being displayed
	}
	else
	{
		//this line must be called by the programmer inorder to permit the game to exit
		// for  a game to exit the programmer must have allowed it to
		m_layers.clear();
		m_sprite_batch.dispose();
		m_texture_program.dispose();
		m_utilities.dispose();
		m_debug_renderer.dispose();
		m_debug_program.dispose();

		//need a better fix for a clean up here
		m_texture_program = SquareBox::RenderEngine::GLSLProgram();
		m_debug_program = SquareBox::RenderEngine::GLSLProgram();

		
		for each (const auto & ref_layer in m_layers) {
			auto& layer = m_layers[ref_layer.index];
			for each (auto & layer_type in m_vec_of_layer_types)
			{
				if (layer.layer_type == layer_type->layer_type) {
					layer_type->onLayerDispose(layer);
				}
			}
		}

		for each (auto & layer_type in m_vec_of_layer_types)
		{
			layer_type->onDispose();
		}
		m_vec_of_layer_types.clear();
		disposeGUI(); //calling this here caused problems because drawGUI was called in the next game loop
	}
}

void LevelEditor::destroy()
{
	SBX_INFO("LevelEditor Screen Cleared out of memory");
}

void LevelEditor::createNewLayer()
{
	//push back a new layer
	m_layers.emplace_back();
	//get a reference to the last placed layer
	SquareBox::GWOM::Layer& layer = m_layers.back();
	layer.camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	layer.camera.setScale(m_universal_camera_scale);
	layer.camera.setPosition(m_universal_camera_position);
	layer.is_visible = true;
	layer.is_locked = false;
	layer.index = m_layers.size() - 1;
	/* its import that the layer be created as a FlatLayer because the other layer types require user input during their setup*/
	layer.layer_type = SquareBox::LayerTypeEnum::FlatLayer;
	for each (auto & layer_type in m_vec_of_layer_types)
	{
		if (layer.layer_type == layer_type->layer_type) {
			layer_type->onLayerInit(layer);
			layer_type->onFocus(layer, m_editor_mode_enum);
			break;
		}
	}
	m_active_layer_index = layer.index; // must be set before upateEditorVariables
	upateEditorVariables();
	//name this layer accoriding to its index
	m_utilities.nameLayerByIndex(layer);
}

void LevelEditor::createNewProject()
{
	cleanOutEditor();
	//Create our first layer
	createNewLayer();
}

void LevelEditor::cleanOutEditor()
{
	m_selected_single_texture_index = 0;
	m_selected_tile_sheet_texture_index = 0;
	//clear out all layers
	for each (const auto & ref_layer in m_layers) {
		auto& layer = m_layers[ref_layer.index];
		for each (auto & layer_type in m_vec_of_layer_types)
		{
			if (layer.layer_type == layer_type->layer_type) {
				layer_type->onLayerDispose(layer);
			}
		}
	}
	m_layers.clear();
}

void LevelEditor::initGUI()
{
#ifdef __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
#endif

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGuiContext* IMGC = ImGui::CreateContext();
	ImGuiIO io = ImGui::GetIO(); (void)io;
	ImGui::SetCurrentContext(IMGC);

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_game_ptr->getWindow()->getWindowPointerForImGui(), m_game_ptr->getWindow()->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_ERROR("Failed to load font Roboto-Medium.ttf ");
	}
	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/chintzy.ttf", 16.0f);
	if (font == NULL) {
		SBX_ERROR("Failed to load font chintzy.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void LevelEditor::disposeGUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void LevelEditor::showMainMenu()
{
	ImGui::BeginMainMenuBar();
	m_main_menu_bar_height = ImGui::GetWindowHeight();
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Respect Ancestor", NULL, m_respect_ancestor)) {
				m_respect_ancestor = !m_respect_ancestor;
			}
			if (ImGui::MenuItem("Show All Debug Lines", NULL, m_show_all_debug_lines)) {
				m_show_all_debug_lines = !m_show_all_debug_lines;
			}
			if (ImGui::MenuItem("Show Console", NULL, m_show_console)) {
				m_show_console = !m_show_console;
			}

			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				m_show_open_level_file_dialog = true;
			}

			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				m_show_save_level_file_dialog = true;
			}
			if (ImGui::MenuItem("New")) { m_show_create_new_project_dialog = true; }

			if (ImGui::BeginMenu("Open Recent"))
			{
				ImGui::MenuItem("null.sbx", NULL, false, false);
				ImGui::MenuItem("sample.sbx", NULL, false, false);
				ImGui::MenuItem("hello.dat", NULL, false, false);
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("About", NULL)) { m_show_about_dialog = true; }
			if (ImGui::MenuItem("Home", NULL)) { setScreenState(SquareBox::ScreenState::CHANGE_PREVIOUS); }
			if (ImGui::MenuItem("Quit", NULL)) { m_request_exit = true; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", NULL, false, false)) {}
			if (ImGui::MenuItem("Redo", NULL, false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", NULL, false, false)) {}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Extra Settings"))
		{
			if (ImGui::BeginMenu("Theme")) {
				if (ImGui::MenuItem("Classic Theme")) {
					m_theme_index = EditorThemeEnum::ClassicTheme;
				}
				if (ImGui::MenuItem("Dark Theme")) {
					m_theme_index = EditorThemeEnum::DarkTheme;
				}
				if (ImGui::MenuItem("Light Theme")) {
					m_theme_index = EditorThemeEnum::LightTheme;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("KeyBoard Bindings")) { m_show_key_bindings_dialog = true; };
			ImGui::EndMenu();
		}

	ImGui::EndMainMenuBar();

	// Setup Dear ImGui style
	if (m_theme_index == EditorThemeEnum::ClassicTheme) {
		ImGui::StyleColorsClassic();
		m_game_ptr->getWindow()->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(127, 127, 127, 127));
	}
	else if (m_theme_index == EditorThemeEnum::LightTheme) {
		ImGui::StyleColorsLight();
		m_game_ptr->getWindow()->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255));
	}
	else if (m_theme_index == EditorThemeEnum::DarkTheme) {
		ImGui::StyleColorsDark();
		m_game_ptr->getWindow()->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255));
	}
}

void LevelEditor::drawGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(nullptr);
	/*static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);*/

	showMainMenu();
	ImGuiWindowFlags properties_tab_window_flags = 0;
	properties_tab_window_flags |= ImGuiWindowFlags_NoMove;
	if (m_show_console) {
		properties_tab_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}

	bool* properties_tab_open = false;
	if (ImGui::Begin("Properties Tab", properties_tab_open, properties_tab_window_flags)) {
		ImGui::SetWindowPos(ImVec2(0, m_main_menu_bar_height));
		if (m_show_console) {
			auto properties_tab_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), m_game_ptr->getWindow()->getScreenHeight() * 0.75);
			ImGui::SetWindowSize(properties_tab_size);
		}
		else {
			auto properties_tab_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), static_cast<float>(m_game_ptr->getWindow()->getScreenHeight()));
			ImGui::SetWindowSize(properties_tab_size);
		}

		m_universal_camera_scale = m_layers[m_active_layer_index].camera.getScale();
		ImGui::DragFloat("Zoom", &m_universal_camera_scale, 1, 0, 9000, "%.3f");
		m_layers[m_active_layer_index].camera.setScale(m_universal_camera_scale);
		// this camera scale will get updated in the update function call

		ImGui::Text("Mode");

		static int e = 0;
		int beforeMode = e;
		ImGui::RadioButton("Place Mode", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Select Mode", &e, 1); ImGui::SameLine();
		ImGui::Checkbox("Debug Mode", &m_debug_mode);

		if (e == 0) {
			m_editor_mode_enum = EditorModeEnum::PLACE;
		}
		else {
			m_editor_mode_enum = EditorModeEnum::SELECT;
		}

		if (e != beforeMode) {
			for each (auto & layer_type in m_vec_of_layer_types)
			{
				if (layer_type->layer_type == m_layers[m_active_layer_index].layer_type) {
					layer_type->onFocus(m_layers[m_active_layer_index], m_editor_mode_enum);
					/*
						helps us to clear the selected items vector
						and also set the current cluster object to nullptr for he flatlayer
					*/
				}
			}
		}

		for each (auto & layer_type in m_vec_of_layer_types)
		{
			if (m_layers[m_active_layer_index].layer_type == layer_type->layer_type) {
				layer_type->onGUILeftPanelDraw(m_layers, m_active_layer_index, m_editor_mode_enum);
			}
		}
		if (ImGui::Button("+ Single")) {
			m_show_open_texture_file_dialog = true;
		}
		ImGui::NewLine();
		ImGui::NewLine();

		ImGui::Separator();

		ImGui::Text("New  TileSheet Tiling:");
		ImGui::Text("cols : "); ImGui::SameLine();
		ImGui::InputInt("###cols", &m_new_tiled_texture_tiling.x);
		ImGui::Text("rows : "); ImGui::SameLine();
		ImGui::InputInt("###rows", &m_new_tiled_texture_tiling.y);

		if (ImGui::Button("+ TileSheet")) {
			m_show_open_tile_sheet_file_dialog = true;
		}
		ImGui::NewLine();
		ImGui::NewLine();

	}
	ImGui::End();


	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	if (m_show_console) {
		physics_tab_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}
	bool* right_side_tab_open = false;
	if (ImGui::Begin("Side Tab", right_side_tab_open, physics_tab_window_flags)) {
		if (m_show_console) {
			auto right_side_tab_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), m_game_ptr->getWindow()->getScreenHeight() * 0.75);
			ImGui::SetWindowSize(right_side_tab_size);
		}
		else {
			auto right_side_tab_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), static_cast<float>(m_game_ptr->getWindow()->getScreenHeight()));
			ImGui::SetWindowSize(right_side_tab_size);
		}
		ImGui::SetWindowPos(ImVec2(m_game_ptr->getWindow()->getScreenWidth() - ImGui::GetWindowWidth() - 2, m_main_menu_bar_height));
		int right_tab_subject = static_cast<int>(m_side_view);

		ImGui::Text("FPS Counter: %i", static_cast<int>(m_game_ptr->getFps()));
		ImGui::Text("Active Layer: %s", m_layers[m_active_layer_index].name);
		ImGui::NewLine();

		if (ImGui::BeginTabBar("SidePanelTabs"))
		{

			if (ImGui::BeginTabItem("Layering"))
			{
				ImGui::NewLine();
				for (unsigned int i = 0; i < m_layers.size(); i++) {
					SquareBox::GWOM::Layer& layer = m_layers[i];
					//Visibility
					if (layer.is_visible) {
						std::stringstream ss;
						ss << "Hide ";
						ss << i;
						if (ImGui::Button(ss.str().c_str())) {
							layer.is_visible = false;
						}
					}
					else {
						std::stringstream ss;
						ss << "Show ";
						ss << i;
						if (ImGui::Button(ss.str().c_str())) {
							layer.is_visible = true;
						}
					}
					ImGui::SameLine();
					//is locked
					if (layer.is_locked) {
						std::stringstream ss;
						ss << "Unlock ";
						ss << i;
						if (ImGui::Button(ss.str().c_str())) {
							layer.is_locked = false;
						}
					}

					else {
						std::stringstream ss;
						ss << "Lock ";
						ss << i;
						if (ImGui::Button(ss.str().c_str())) {
							layer.is_locked = true;
						}
					}
					ImGui::SameLine();
					std::stringstream ss;
					ss << "####LayerName ";
					ss << i;
					ImGui::InputText(ss.str().c_str(), layer.name, 20);
					ImGui::SameLine();
					if (m_active_layer_index == i) {
						ImGui::Button("active");

					}
					else {

						std::stringstream ss;
						ss << ". ";
						ss << i;

						if (ImGui::Button(ss.str().c_str()) && layer.is_visible && !layer.is_locked) {
							//first do the currently layers clean up
							for each (auto & layer_type in m_vec_of_layer_types)
							{
								if (layer_type->layer_type == m_layers[m_active_layer_index].layer_type) {
									layer_type->onOutOfFocus(m_layers[m_active_layer_index]);
								}
							}
							m_active_layer_index = i;
							SquareBox::GWOM::Layer& new_active_layer = m_layers[i];
							for each (auto & layer_type in m_vec_of_layer_types)
							{
								if (layer_type->layer_type == new_active_layer.layer_type) {
									layer_type->onFocus(new_active_layer, m_editor_mode_enum);
								}
							}
							upateEditorVariables();
						}
					}
				}

				ImGui::NewLine();
				if (ImGui::Button("New Layer")) {
					SBX_INFO("Creating New layers");
					createNewLayer();
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Layer")) {
					//can't leave the world with no layers
					if (m_layers.size() > 1) {
						//remove the active layer from m_layers

						auto& active_layer = m_layers[m_active_layer_index];

						for each (auto & layer_type in m_vec_of_layer_types)
						{
							if (layer_type->layer_type == active_layer.layer_type) {
								//no need to call on out of focus since the layer is getting deleted
								layer_type->onLayerDispose(active_layer);
								break;
							}
						}

						//deleting from  m_layers
						int item_count = 0;
						for (auto it = m_layers.begin(); it != m_layers.end(); it++)
						{
							if (item_count == m_active_layer_index) {
								//delete and break;
								SBX_INFO("{} Layer deleted", std::string(it->name));
								m_layers.erase(it);
								break;
							}
							item_count++;
						}

						item_count = 0;
						for (auto it = m_layers.begin(); it != m_layers.end(); it++)
						{
							std::string old_layer_name = (*it).name;
							//expected layer name
							m_utilities.nameLayerByIndex((*it));
							std::string expected_index_layer_name = (*it).name;

							for each (auto & layer_type in m_vec_of_layer_types)
							{
								if (layer_type->layer_type == (*it).layer_type) {
									layer_type->onFocus((*it), m_editor_mode_enum);
									layer_type->onLayerIndexChange((*it), item_count);
								}
							}

							//new layer name
							m_utilities.nameLayerByIndex((*it));
							std::string new_index_layer_name = (*it).name;

							if (old_layer_name == expected_index_layer_name) {
								//give new index name
								m_utilities.nameLayerByGivenName((*it), new_index_layer_name);
							}
							else {
								//leave with old name
								m_utilities.nameLayerByGivenName((*it), old_layer_name);
							}
							item_count++;
						}

						//reassign the active layer to the first layer 
						m_active_layer_index = 0;
						auto& new_active_layer = m_layers[m_active_layer_index];
						for each (auto & layer_type in m_vec_of_layer_types)
						{
							if (layer_type->layer_type == new_active_layer.layer_type) {
								layer_type->onFocus(new_active_layer, m_editor_mode_enum);
							}
						}
						//force the new active layer to open open it
						new_active_layer.is_locked = false;
						new_active_layer.is_visible = true;
					}
					else {
						SBX_ERROR("Can't leave the world with out any layers to work with !!!");
					}

				}

				ImGui::SameLine();
				if (ImGui::Button("Clear Layer")) {
					auto& active_layer = m_layers[m_active_layer_index];
					for each (auto & layer_type in m_vec_of_layer_types)
					{
						if (layer_type->layer_type == active_layer.layer_type) {
							layer_type->onLayerDispose(active_layer);
							break;
						}
					}

				}

				ImGui::SameLine();
				if (ImGui::ArrowButton("Layer_up", ImGuiDir_Up)) {
					int new_desired_layer_index = m_active_layer_index - 1;
					if (new_desired_layer_index >= 0) {
						/*
							we have to literally shift this layer and the only way to do that is to do a clean cut copy swap replace
						*/
						//the copy
						SquareBox::GWOM::Layer orignal_layer_data = m_layers[m_active_layer_index];
						SquareBox::GWOM::Layer destination_layer_data = m_layers[new_desired_layer_index];

						//the swap
						m_layers[new_desired_layer_index] = orignal_layer_data;
						m_layers[m_active_layer_index] = destination_layer_data;

						m_active_layer_index = new_desired_layer_index;
					}
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("Layer_down", ImGuiDir_Down)) {
					int new_desired_layer_index = m_active_layer_index + 1;
					if (new_desired_layer_index < static_cast<int>(m_layers.size())) {
						/*
						we have to literally shift this layer and the only way to do that is to do a clean cut copy swap replace

						*/
						//the copy
						SquareBox::GWOM::Layer orignal_layer_data = m_layers[m_active_layer_index];
						SquareBox::GWOM::Layer destination_layer_data = m_layers[new_desired_layer_index];
						//the swap
						m_layers[new_desired_layer_index] = orignal_layer_data;
						m_layers[m_active_layer_index] = destination_layer_data;
						m_active_layer_index = new_desired_layer_index;
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("layer"))
			{
				ImGui::NewLine();
				for each (auto & layer_type in m_vec_of_layer_types)
				{
					if (m_layers[m_active_layer_index].layer_type == layer_type->layer_type) {
						layer_type->onLayerPropertiesGUIDraw(m_layers, m_active_layer_index, m_editor_mode_enum);
					}
				}
				auto& active_layer = m_layers[m_active_layer_index];

				ImGui::Text("Layer Opacity :"); ImGui::SameLine();
				ImGui::SliderInt("###layeropacity", &active_layer.opacity, 0, 100);
				SquareBox::LayerTypeEnum current_layer_type = m_layers[m_active_layer_index].layer_type;

				for (unsigned int layer_type_index = 0; layer_type_index < m_vec_of_layer_types.size(); layer_type_index++)
				{
					layer_labels_ptr[layer_type_index] = ToString(m_vec_of_layer_types[layer_type_index]->layer_type);
				}
				ImGui::Text("Layer Type    :"); ImGui::SameLine();
				ImGui::Combo("###LayerType", &m_selected_layer_type, layer_labels_ptr, m_vec_of_layer_types.size());
				if (m_vec_of_layer_types[m_selected_layer_type]->layer_type != current_layer_type) {
					//time for change
					int on_layer_init_gui_reponse = m_vec_of_layer_types[m_selected_layer_type]->onLayerInitGui(active_layer);
					if (on_layer_init_gui_reponse == 1) {
						//use accepated
						for each (auto & layer_type in m_vec_of_layer_types)
						{
							if (layer_type->layer_type == current_layer_type) {
								layer_type->onLayerDispose(active_layer);
								break;
							}
						}
						m_vec_of_layer_types[m_selected_layer_type]->onLayerInit(active_layer);
					}
					else if (on_layer_init_gui_reponse == 0) {
						//cancalled
						for (unsigned int layer_type_index = 0; layer_type_index < m_vec_of_layer_types.size(); layer_type_index++)
						{
							if (m_vec_of_layer_types[layer_type_index]->layer_type == current_layer_type) {
								m_selected_layer_type = layer_type_index;
								break;
							}
						}
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Object"))
			{
				ImGui::NewLine();
				for each (auto & layer_type in m_vec_of_layer_types)
				{
					if (m_layers[m_active_layer_index].layer_type == layer_type->layer_type) {
						layer_type->onObjectPropertiesGUIDraw(m_layers, m_active_layer_index, m_editor_mode_enum);
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}


		ImGui::NewLine();
		ImGui::NewLine();
	}
	ImGui::End();


	//dialogs
	if (m_request_exit) {
		ImGui::OpenPopup("CloseLevelEditor?");
	}


	if (m_show_about_dialog) {
		ImGui::OpenPopup("About?");
	}

	if (m_show_key_bindings_dialog) {
		ImGui::OpenPopup("KeyBindings");
	}

	if (m_show_failed_to_save_world_dialog) {
		ImGui::OpenPopup("FailedToSaveWorld?");
	}

	if (m_show_create_new_project_dialog) {
		ImGui::OpenPopup("CreateNewProject");
	}


	//Close Editor Tab
		// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("CloseLevelEditor?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (!m_is_all_work_saved) {
			ImGui::Text("Warning : Your progress is not fully saved.\nAll unsaved work will be lost.This operation can not be reversed.\n\n");
			ImGui::Separator();
		}
		else {
			ImGui::Text("Do you want to close the editor?");
			ImGui::Separator();
		}
		static bool dont_ask_me_next_time = false;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
		ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			//Close  LevelEditor
			m_allow_exit = true;
			m_game_ptr->initiateExit();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			m_request_exit = false;
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("ABOUT:");
		ImGui::BulletText("This is a 2D game world Editing tool");
		ImGui::BulletText("Built and maintained by stande studios ");
		ImGui::BulletText("Established 2021");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_show_about_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("KeyBindings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("PLACE MODE");
		ImGui::BulletText("Mouse Left-Click              :  Place new cluster object into World");

		ImGui::Separator();
		ImGui::Text("Camera Controls");
		ImGui::Text("LEFT_ALT + LEFT_CLICK         :  Camera Panning");
		ImGui::Text("SHIFT    + mouse wheel        :  Camera Zoom");


		ImGui::Separator();
		ImGui::Text("SELECT MODE");


		if (m_layers[m_active_layer_index].layer_type == SquareBox::LayerTypeEnum::TiledLayer) {
		ImGui::Text("TAB + ARROW KEY               :  Duplicated selected tiles in a particular direction");
		ImGui::Text("F1  + ARROW KEY               :  Tiles out the texture map into the world");
		}
		else if (m_layers[m_active_layer_index].layer_type == SquareBox::LayerTypeEnum::FlatLayer) {
			ImGui::Text("\nCluster Mode");
			ImGui::Text("LEFT_CLICK                    :  makes hovered objects world cluster get selected");
			ImGui::Text("\nObject Mode");
			ImGui::Text("LEFT_CLICK                    :  makes hovered object active object");
			ImGui::Text("TAB + ARROW KEYS              :  dupilcates the selected cluster object in the arrows' direction");

			ImGui::Text("\nFree Select Mode");
			ImGui::Text("LEFT_CLICK                    :  adds hovered object to selected objects");
			ImGui::Text("SPACE_BAR + RIGHT_CLICK       :  creates selection rectangle");

			ImGui::Text("\n\nRIGHT_CLICK                   :  drags selected objects");

			ImGui::Text("\nJoint Mode:");
			ImGui::Text("Still under development");
		}

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_show_key_bindings_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("FailedToSaveWorld?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Failed to save world!\nCheck Logs for more information");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_show_failed_to_save_world_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("CreateNewProject", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		bool do_we_have_unsaved_work = false;
		for each (auto & layer_type in m_vec_of_layer_types)
		{
			if (!layer_type->m_is_all_work_saved) {
				do_we_have_unsaved_work = true;
				break;
			}
		}

		if (do_we_have_unsaved_work) {
			ImGui::Text("Not all  progress has been saved. Continuing will discard unsaved Changes");
			ImGui::Separator();

			if (ImGui::Button("Continue", ImVec2(120, 0))) {
				for each (auto & layer_type in m_vec_of_layer_types)
				{
					layer_type->m_is_all_work_saved = true;
				}
				createNewProject();
				m_show_create_new_project_dialog = false;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				m_show_create_new_project_dialog = false;
				ImGui::CloseCurrentPopup();
			}
		}
		else {
			ImGui::Text("Create New Project ? ");
			ImGui::Separator();
			if (ImGui::Button("Ok", ImVec2(120, 0))) {
				createNewProject();
				m_show_create_new_project_dialog = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	if (m_show_open_level_file_dialog)
		ImGui::OpenPopup("Open Level File");
	if (m_show_save_level_file_dialog)
		ImGui::OpenPopup("Save Level File");
	if (m_show_open_texture_file_dialog)
		ImGui::OpenPopup("Open Texture File");

	if (m_show_open_tile_sheet_file_dialog)
		ImGui::OpenPopup("Open TileSheet File");

	/* Optional third parameter. Support opening only compressed rar/zip files.
	 * Opening any other file will show error, return false and won't close the dialog.
	 */
	if (m_file_dialog.showFileDialog("Open Level File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_level_file_extension))
	{
		SBX_INFO("Starting Level Data Loading.");
		SBX_INFO("Loading {} .", m_file_dialog.selected_fn);
		cleanOutEditor();
		glm::vec2 editing_screen_dimensions;
		m_level_reader_writer.loadLevelDataAsBinary(m_file_dialog.selected_path, m_layers, m_universal_camera_scale, m_universal_camera_position, editing_screen_dimensions, m_active_layer_index);
		// setting up the layers and their textures , both the singles and the tiled
		for (unsigned int i = 0; i < m_layers.size(); i++)
		{
			SquareBox::GWOM::Layer& layer = m_layers[i];
			//recreate all the layer cameras
			layer.camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
			layer.camera.setScale(m_universal_camera_scale);
			layer.camera.setPosition(m_universal_camera_position);
			//load this layers textures
			m_utilities.loadLayerTextures(layer, m_game_ptr->getWindow()->getDDPI());
			//the world clusters
			for each (auto & layer_type in m_vec_of_layer_types)
			{
				if (layer_type->layer_type == layer.layer_type) {
					layer_type->onLoadFromStorage(m_layers, layer.index);
				}
			}
		}

		if (m_layers.size() == 0)
		{
			//this was an empty levelv file
			createNewLayer();
		}
		m_is_all_work_saved = false;
	}
	if (m_file_dialog.showFileDialog("Open Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
	{
		m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
		m_texture.asset_file_path = m_file_dialog.selected_path;
		m_texture.display_name = m_file_dialog.selected_fn;

		/*
			if we want to be saving the textures display names . then
			SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(m_texture.id, m_file_dialog.selected_fn);
		*/

		//first confirm that we don't have this texture already
		bool isAlreadyPresent = false;

		for (size_t i = 0; i < m_layers[m_active_layer_index].single_textures.size(); i++)
		{
			if (m_layers[m_active_layer_index].single_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
				isAlreadyPresent = true;
				break;
			}
		}

		if (!isAlreadyPresent) {
			SquareBox::GWOM::ParentTexture new_parent_texture;
			new_parent_texture.texture_index = m_layers[m_active_layer_index].single_textures.size();
			new_parent_texture.texture = m_texture;
			m_layers[m_active_layer_index].single_textures.push_back(new_parent_texture);

			//add to the sub_textures_table for all layers , since a none tiled can become tiled layer
			SquareBox::GWOM::SubTexture new_sub_texture;
			new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
			new_sub_texture.parent_type = SquareBox::TextureEnum::SINGLE;
			new_sub_texture.sub_texture_key = m_layers[m_active_layer_index].sub_textures_table.size();
			m_layers[m_active_layer_index].sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
		}
	}
	if (m_file_dialog.showFileDialog("Open TileSheet File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
	{
		m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
		m_texture.asset_file_path = m_file_dialog.selected_path;
		m_texture.display_name = m_file_dialog.selected_fn;


		//first confirm that we don't have this texture already
		bool isAlreadyPresent = false;

		for (size_t i = 0; i < m_layers[m_active_layer_index].tiled_textures.size(); i++)
		{
			if (m_layers[m_active_layer_index].tiled_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
				isAlreadyPresent = true;
				break;
			}
		}

		if (!isAlreadyPresent) {
			SquareBox::GWOM::ParentTexture new_parent_texture;
			new_parent_texture.texture_index = m_layers[m_active_layer_index].tiled_textures.size();
			new_parent_texture.texture = m_texture;
			// we do this here so that we do not accidently over write a textures properties
			new_parent_texture.texture.tiling = m_new_tiled_texture_tiling;
			m_new_tiled_texture_tiling.x = 0;
			m_new_tiled_texture_tiling.y = 0;

			SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(new_parent_texture.texture.id, new_parent_texture.texture.display_name);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(new_parent_texture.texture.id, new_parent_texture.texture.tiling);

			if (new_parent_texture.texture.tiling.x > 0 && new_parent_texture.texture.tiling.y > 0) {
				m_layers[m_active_layer_index].tiled_textures.push_back(new_parent_texture);
				//loop through all the tilesheet indiviaual grids adding one by one
				int num_sections = new_parent_texture.texture.tiling.x * new_parent_texture.texture.tiling.y;
				int initial_sub_texture_key = m_layers[m_active_layer_index].sub_textures_table.size();
				for (int i = 0; i < num_sections; i++)
				{
					SquareBox::GWOM::SubTexture new_sub_texture;
					new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
					new_sub_texture.parent_type = SquareBox::TextureEnum::TILESHEET;
					new_sub_texture.tiling_index = i;
					new_sub_texture.sub_texture_key = initial_sub_texture_key + i;
					m_layers[m_active_layer_index].sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
				}
			}
			else {
				SBX_ERROR("Failed to Added TileSheet {} to Editor, Invalid Tiling", new_parent_texture.texture.display_name);
			}
		}
	}

	if (m_file_dialog.showFileDialog("Save Level File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_level_file_extension))
	{

		SBX_INFO("Saving Level as {} File", m_level_file_extension);
		//eliminate all the file extensions just incase the user included an extra file extension
		//e.g level.txt.txt
		std::string ext = m_file_dialog.ext;
		std::string filePath = m_file_dialog.selected_path;

		while (filePath != ext &&
			filePath.size() > ext.size() &&
			filePath.substr(filePath.size() - ext.size()) == ext)
		{
			// if so then strip them off
			filePath = filePath.substr(0, filePath.size() - ext.size());
		}
		m_utilities.worldIndiciesCleanUp(m_layers);
		if (m_level_reader_writer.saveLevelDataAsBinary(filePath + m_file_dialog.ext, m_layers, m_universal_camera_scale, m_universal_camera_position,glm::vec2(m_game_ptr->getWindow()->getMaxMaxScreenWidth(),m_game_ptr->getWindow()->getMaxMaxScreenHeight()), m_active_layer_index))
		{
			SBX_INFO("Level Data Saved at {}.{}", m_file_dialog.selected_path, m_file_dialog.ext);
			m_is_all_work_saved = true;
		}
		else {
			m_show_failed_to_save_world_dialog = true;
			SBX_WARN("Failed To Save Level Data");
		}
		if (m_layers.size() == 0) {
			//this was an empty world before the save so we should return its defaults
			createNewLayer();
		}
	}

	//reset of all dialogs
	m_show_open_level_file_dialog = false;
	m_show_save_level_file_dialog = false;
	m_show_open_texture_file_dialog = false;
	m_show_open_tile_sheet_file_dialog = false;


	//file explore dialog - end
	if (m_show_console) {
		auto console_position = ImVec2(0.0f, m_game_ptr->getWindow()->getScreenHeight() * 0.75f);
		auto console_size = ImVec2(static_cast<float>(m_game_ptr->getWindow()->getScreenWidth()), m_game_ptr->getWindow()->getScreenHeight() * 0.25f);
		ImGuiWindowFlags console_flags = 0;
		console_flags |= ImGuiWindowFlags_NoTitleBar;
		m_console.Draw("Console", &m_show_console, console_flags, console_position, console_size);

		for each (auto log_message in m_fresh_logs_vec)
		{
			m_console.AddLog(log_message.c_str());
		}
		m_fresh_logs_vec.clear();
	}
	else {
		if (m_fresh_logs_vec.size() > 0) {
			m_fresh_logs_vec.clear();
		}
	}


	ImGui::EndFrame();// the example code did not have this.
	// Rendering
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

}

void LevelEditor::upateEditorVariables()
{
	for (unsigned int i = 0; i < m_vec_of_layer_types.size(); i++)
	{
		if (m_vec_of_layer_types[i]->layer_type == m_layers[m_active_layer_index].layer_type) {
			m_selected_layer_type = i;
		}
	}
}

LevelEditor::~LevelEditor()
{
}
