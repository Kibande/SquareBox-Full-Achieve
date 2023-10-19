#include "Second_Screen.h"
#include <SquareBox-Editor/Editor_Assistant.cpp>
/*
Order of Methods Calling
	Constructor
	Build
	OnEntry

	//game loop
	 (while ScreenState::RUNNING){
		 Update
		 Draw
	   }

	OnExit
	destroy
	De constructor

*/


Second_Screen::Second_Screen()
{
}

int Second_Screen::getNextScreenIndex() const
{
	return m_next_screen_index;
}

int Second_Screen::getPreviousScreenIndex() const
{
	return m_previous_screen_index;
}

void Second_Screen::build()
{
}

void Second_Screen::onEntry()
{
	// this is where  stuff  the screen will use are  initialized from
	m_sprite_font.initWithFilePath("Assets/Fonts/Comfortaa-Bold.ttf", 32);

	//Init Shaders
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();

	m_sprite_batch.init();

	//the debugProgram
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertexColor");
	m_debug_program.linkShaders();

	m_debug_renderer.init();


	//Init Cameras
	glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions.x, screen_dimensions.y);
	m_camera.setScale(m_camera_scale);//The Zoom of the Camera
	m_camera.setPosition(screen_dimensions * glm::vec2(0.5f));

	m_utilities.init();

	//initially black
	m_grid_color_array[0] = 1.0f;
	m_grid_color_array[1] = 1.0f;
	m_grid_color_array[2] = 1.0f;
	m_grid_color_array[3] = 1.0f;

	//initially red
	m_tiling_number_color_array[0] = 1.0f;
	m_tiling_number_color_array[1] = 0.0f;
	m_tiling_number_color_array[2] = 0.0f;
	m_tiling_number_color_array[3] = 1.0f;

	//initially grey
	m_window_back_ground_color_array[0] = 0.168f;
	m_window_back_ground_color_array[1] = 0.168f;
	m_window_back_ground_color_array[2] = 0.188f;
	m_window_back_ground_color_array[3] = 1.0f;


	initGUI();

	//window icon
	auto icon_pixel_data = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/nodpi/bricks_light_top.png");
	m_game_ptr->getWindow()->setWindowIcon(icon_pixel_data.pixels, icon_pixel_data.width, icon_pixel_data.height);
	SquareBox::AssetManager::IOManager::freePixelData(icon_pixel_data);
}

void Second_Screen::update(const float& deltaTime_)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
	{
		//stop our game engine from processing input
		m_game_ptr->setProcessingInput(false);
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
		}
	}
	else {
		m_game_ptr->setProcessingInput(true);
	}

	if (m_game_ptr->isProcessingInput())
	{
		m_editor_assistant.cameraControls(m_camera, m_game_ptr);
		m_camera_scale = m_camera.getScale();
	}

	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	// maintaing the textures aspect ratio
	//retrive texture from the asset manager
	if (m_vec_of_texture_objects.size() > m_active_texture_object_index) {
		auto retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(m_vec_of_texture_objects[m_active_texture_object_index].texture_info.texture_id);
		float x_to_y = SquareBox::MathLib::Float::divideAndGetFloat(retrieved_texture.width, retrieved_texture.height);
		m_vec_of_texture_objects[m_active_texture_object_index].height = screen_dimensions.y * m_screen_texture_height_ratio; // this 
		m_vec_of_texture_objects[m_active_texture_object_index].width = m_vec_of_texture_objects[m_active_texture_object_index].height * x_to_y;
	}

	//Called once every game loop , and updates what will be drawn
	m_camera.update(screen_dimensions.x, screen_dimensions.y);
	m_camera.setScale(m_camera_scale);
	m_game_ptr->getWindow()->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(m_window_back_ground_color_array[0] * 255, m_window_back_ground_color_array[1] * 255, m_window_back_ground_color_array[2] * 255, m_window_back_ground_color_array[2] * 255));
}

void Second_Screen::draw()
{
	if (m_vec_of_texture_objects.size() > m_active_texture_object_index) {
		//Draw call
		m_texture_program.use();

		IGameScreen::preUpdateShader(&m_texture_program, "mySampler");

		IGameScreen::uploadCameraInfo(&m_texture_program, &m_camera, "P");

		m_sprite_batch.begin();
		//m_sprite_font or m_sprite_batch draw

		//m_sprite_batch.draw();

		const auto& active_texture_object = m_vec_of_texture_objects[m_active_texture_object_index];
		m_utilities.setCurrentShapePointer(active_texture_object.shape, &m_current_shape_ptr);
		m_current_shape_ptr->draw(active_texture_object, m_sprite_batch);
		m_sprite_batch.end();
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();

		// drawing the tiling details
		auto& target_cluster_object = m_vec_of_texture_objects[m_active_texture_object_index];

		if (!target_cluster_object.pod_bool) {


			//hit the asset manager to retrive the target texture using the texture id
			SquareBox::AssetManager::GLTexture retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(active_texture_object.texture_info.texture_id);
			glm::vec4 rect_shape(active_texture_object.position - (glm::vec2(active_texture_object.width, active_texture_object.height) * glm::vec2(0.5f)), active_texture_object.width, active_texture_object.height);

			if (m_show_grid_lines) {
				m_debug_renderer.begin();
				m_current_shape_ptr->debugDraw(active_texture_object, m_debug_renderer, SquareBox::RenderEngine::ColorRGBA8(m_grid_color_array[0] * 255, m_grid_color_array[1] * 255, m_grid_color_array[2] * 255, m_grid_color_array[3] * 255));
				//drawing the grid lines 

				if (retrieved_texture.tiling.x > 1) {
					float x_gap = active_texture_object.width / retrieved_texture.tiling.x;
					for (unsigned int i = 0; i < retrieved_texture.tiling.x; i++)
					{
						glm::vec2 top_point(rect_shape.x + (i * x_gap), rect_shape.y + rect_shape.w);
						glm::vec2 bottom_point(rect_shape.x + (i * x_gap), rect_shape.y);
						m_debug_renderer.drawLine(top_point, bottom_point, SquareBox::RenderEngine::ColorRGBA8(m_grid_color_array[0] * 255, m_grid_color_array[1] * 255, m_grid_color_array[2] * 255, m_grid_color_array[3] * 255));
					}
				}

				if (retrieved_texture.tiling.y > 1) {
					float y_gap = active_texture_object.height / retrieved_texture.tiling.y;
					for (unsigned int i = 0; i < retrieved_texture.tiling.y; i++)
					{
						glm::vec2 left_point(rect_shape.x, rect_shape.y + (i * y_gap));
						glm::vec2 right_point(rect_shape.x + rect_shape.z, rect_shape.y + (i * y_gap));
						m_debug_renderer.drawLine(left_point, right_point, SquareBox::RenderEngine::ColorRGBA8(m_grid_color_array[0] * 255, m_grid_color_array[1] * 255, m_grid_color_array[2] * 255, m_grid_color_array[3] * 255));
					}
				}
				m_debug_renderer.end();
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &m_camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();
			}
			//the numbering
			m_sprite_batch.begin();
			if (retrieved_texture.tiling.x >= 1 && retrieved_texture.tiling.y >= 1) {
				float x_gap = active_texture_object.width / retrieved_texture.tiling.x;
				float y_gap = active_texture_object.height / retrieved_texture.tiling.y;
				//glm::vec2 top_left_corner_grid_center(rect_shape.x + (x_gap*0.5), rect_shape.y + rect_shape.w - (y_gap*0.5));
				glm::vec2 bottom_left_corner_grid_center(rect_shape.x + (x_gap * 0.5), rect_shape.y + (y_gap * 0.5));

				int tile_index = 0;
				for (unsigned int i = 0; i < retrieved_texture.tiling.y; i++)
				{
					for (unsigned int j = 0; j < retrieved_texture.tiling.x; j++)
					{
						std::ostringstream os;
						os << tile_index;
						auto temp_tiling_color = SquareBox::RenderEngine::ColorRGBA8(m_tiling_number_color_array[0] * 255, m_tiling_number_color_array[1] * 255, m_tiling_number_color_array[2] * 255, m_tiling_number_color_array[3] * 255);
						m_sprite_font.draw(m_sprite_batch, os.str().c_str(), glm::vec2(bottom_left_corner_grid_center.x + (x_gap * j), bottom_left_corner_grid_center.y + (y_gap * i)), glm::vec2(m_tiling_numbers_scale), 1.0f, temp_tiling_color, SquareBox::JustificationEnum::MIDDLE);
						tile_index++;
					}
				}


			}
			m_sprite_batch.end();

			m_texture_program.use();
			uploadCameraInfo(&m_texture_program, &m_camera, "P");
			m_sprite_batch.renderBatch();
			m_texture_program.unuse();
		}
		else {
			// we are doing the tile system draw
			float camera_width = m_camera.getCameraDimensions().x;
			float camera_height = m_camera.getCameraDimensions().y;
			glm::vec4 camera_destRect(m_camera.getPosition() - (glm::vec2(camera_width, camera_height) * 0.5f), glm::vec2(camera_width, camera_height));

			std::map<int, SquareBox::GWOM::Tile*> collision_grid_cells_in_view = m_tile_system.getAllTilesInDestRect(camera_destRect);

			if (m_show_grid_lines) {
				m_debug_renderer.begin();

				//draw the collision cells in camera view
				for (auto it = collision_grid_cells_in_view.begin(); it != collision_grid_cells_in_view.end(); it++)
				{
					glm::vec2 grid_cell_position = (*it).second->position;
					float grid_cell_size = m_tile_system.getTileSize();
					glm::vec4 grid_cell_destRect(grid_cell_position - glm::vec2(grid_cell_size) * 0.5f, glm::vec2(grid_cell_size));
					m_debug_renderer.drawBox(grid_cell_destRect, SquareBox::RenderEngine::ColorRGBA8(m_grid_color_array[0] * 255, m_grid_color_array[1] * 255, m_grid_color_array[2] * 255, m_grid_color_array[3] * 255), 0.0f);

				}
				m_debug_renderer.end();
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &m_camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();
			}

			// the indicies

			m_sprite_batch.begin();
			auto temp_tiling_color = SquareBox::RenderEngine::ColorRGBA8(m_tiling_number_color_array[0] * 255, m_tiling_number_color_array[1] * 255, m_tiling_number_color_array[2] * 255, m_tiling_number_color_array[3] * 255);
			//draw the collision cells in camera view
			for (auto it = collision_grid_cells_in_view.begin(); it != collision_grid_cells_in_view.end(); it++)
			{
				glm::vec2 grid_cell_position = (*it).second->position;
				int tile_index = (*it).second->index;
				std::ostringstream os;
				os << tile_index;
				m_sprite_font.draw(m_sprite_batch, os.str().c_str(), (*it).second->position, glm::vec2(m_tiling_numbers_scale), 1.0f, temp_tiling_color, SquareBox::JustificationEnum::MIDDLE);

			}
			m_sprite_batch.end();
			m_texture_program.use();
			uploadCameraInfo(&m_texture_program, &m_camera, "P");
			m_sprite_batch.renderBatch();
			m_texture_program.unuse();
		}
	}
	drawGUI();
}

void Second_Screen::onExit()
{
	//this is where all screen process are stopped
	//this is where objects that the screen used are destroyed from
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();
	m_sprite_font.dispose();
	m_texture_program.dispose();
	m_sprite_batch.dispose();
	m_utilities.dispose();
	m_debug_program.dispose();
	m_debug_renderer.dispose();
}

void Second_Screen::destroy()
{
}

void Second_Screen::initGUI()
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
	ImGui::CreateContext();
	ImGuiIO io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_game_ptr->getWindow()->getWindowPointerForImGui(), m_game_ptr->getWindow()->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_ERROR("Failed to load font Roboto-Medium.ttf ");
	}
	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/chintzy.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_ERROR("Failed to load font Roboto-Medium.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void Second_Screen::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMenuMain();
	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::Begin("Control Panel", nullptr, physics_tab_window_flags);

	ImGui::SetWindowPos(ImVec2(m_game_ptr->getWindow()->getScreenWidth() - ImGui::GetWindowWidth() - 2, 20));
	{
		ImGui::InputFloat("Camera Scale ", &m_camera_scale, 0.01f, 0.1f, "%.3f");
		ImGui::InputFloat("Indicies Scale", &m_tiling_numbers_scale, 0.01f, 0.1f, "%.3f");
		ImGui::Checkbox("Show Grid", &m_show_grid_lines);

		ImGui::ColorEdit4("Background color", m_window_back_ground_color_array);

		ImGui::ColorEdit4("Grid color", m_grid_color_array);
		ImGui::ColorEdit4("Indicies color", m_tiling_number_color_array);

		if (m_vec_of_texture_objects.size() > m_active_texture_object_index) {
			//Retreiving a particular indicies uv coordinates
			ImGui::InputInt("UV React", &m_target_tile_index, 1, 1);
			SquareBox::AssetManager::GLTexture retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(m_vec_of_texture_objects[m_active_texture_object_index].texture_info.texture_id);
			int retrieved_texture_max_index = retrieved_texture.tiling.x * retrieved_texture.tiling.y;
			if (m_target_tile_index < 0) {
				m_target_tile_index = 0;
			}
			else if (m_target_tile_index >= retrieved_texture_max_index) {
				m_target_tile_index = retrieved_texture_max_index - 1;
			}
			glm::vec4 uvRect = retrieved_texture.getUVReactAtIndex(m_target_tile_index,glm::ivec2(1));

			ImGui::Text("X : %.3f", uvRect.x);
			ImGui::Text("Y : %.3f", uvRect.y);
			ImGui::Text("Z : %.3f", uvRect.z);
			ImGui::Text("W : %.3f", uvRect.w);


			ImGui::NewLine();

			ImGui::Text("Name : ");
			ImGui::SameLine();
			std::vector<char*>  vc;
			vc.reserve(m_vec_of_texture_objects.size());
			std::transform(m_vec_of_texture_objects.begin(), m_vec_of_texture_objects.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ClusterObject& cluster_object_) {
				//this new here is never deleted, and is called alot, that is a problem to us 
				auto retrived_texture = SquareBox::AssetManager::TextureManager::getTextureById(cluster_object_.texture_info.texture_id);
				char* pc = new char[retrived_texture.display_name.size() + 1];
				std::strcpy(pc, retrived_texture.display_name.c_str());
				return pc;
			});
			int m_active_texture_object_index_before = m_active_texture_object_index;
			ImGui::Combo("###loadedTextures", &m_active_texture_object_index, &vc[0], m_vec_of_texture_objects.size());
			if (m_active_texture_object_index_before != m_active_texture_object_index) {
				auto& target_cluster_object = m_vec_of_texture_objects[m_active_texture_object_index];

				if (retrieved_texture.tiling.x == retrieved_texture.tiling.y) {
					if (SquareBox::MathLib::Float::areFloatsEqual(target_cluster_object.width, target_cluster_object.height, 0.1)) {
						target_cluster_object.pod_bool = true;
					}
					else {
						target_cluster_object.pod_bool = false;
					}
				}
				else {
					target_cluster_object.pod_bool = false;
				}
				if (target_cluster_object.pod_bool) {
					// updated the tile system
					m_tile_system.resetTileSystem();
					glm::vec2 target_position = target_cluster_object.position;
					glm::vec2 target_dimensions = glm::vec2(target_cluster_object.width, target_cluster_object.height);
					glm::vec4 cluster_objects_dest_rect = glm::vec4(target_position - (target_dimensions * 0.5f), target_dimensions);
					SquareBox::AssetManager::GLTexture retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(target_cluster_object.texture_info.texture_id);
					float tile_size = target_cluster_object.width / retrieved_texture.tiling.x;// since we are only doing this for sqaure grids
					m_tile_system.init(cluster_objects_dest_rect.x, cluster_objects_dest_rect.y, cluster_objects_dest_rect.z, cluster_objects_dest_rect.w, tile_size);
				}
				else {
					// dispose of the tile system 
					/*
						if each of these cluster objects where instead each an independent layer , we would not be having need to
						dispose the tile system when we selected a different layer
						this would have an advantage of being faster
					*/
					m_tile_system.dispose();
				}
			}
			//Drop down menu
			ImGui::Text("cols : "); ImGui::SameLine();
			ImGui::InputInt("###c_cols", &m_current_texture_object_tiling.x);
			if (m_current_texture_object_tiling.x < 1) {
				m_current_texture_object_tiling.x = 1;
			}
			ImGui::Text("rows : "); ImGui::SameLine();
			ImGui::InputInt("###c_rows", &m_current_texture_object_tiling.y);
			if (m_current_texture_object_tiling.y < 1) {
				m_current_texture_object_tiling.y = 1;
			}

			if (m_vec_of_texture_objects.size() > m_active_texture_object_index) {
				if (ImGui::Button("Update Tiling ")) {
					SquareBox::AssetManager::GLTexture retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(m_vec_of_texture_objects[m_active_texture_object_index].texture_info.texture_id);
					SquareBox::AssetManager::TextureManager::setTextureTilingById(retrieved_texture.id, m_current_texture_object_tiling);
					auto& target_cluster_object = m_vec_of_texture_objects[m_active_texture_object_index];
					if (m_current_texture_object_tiling.x == m_current_texture_object_tiling.y) {
						if (SquareBox::MathLib::Float::areFloatsEqual(target_cluster_object.width, target_cluster_object.height, 0.1)) {
							target_cluster_object.pod_bool = true;
						}
						else {
							target_cluster_object.pod_bool = false;
						}
					}
					else {
						target_cluster_object.pod_bool = false;
					}

					if (target_cluster_object.pod_bool) {
						// updated the tile system
						m_tile_system.resetTileSystem();
						glm::vec2 target_position = target_cluster_object.position;
						glm::vec2 target_dimensions = glm::vec2(target_cluster_object.width, target_cluster_object.height);
						glm::vec4 cluster_objects_dest_rect = glm::vec4(target_position - (target_dimensions * 0.5f), target_dimensions);
						SquareBox::AssetManager::GLTexture retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(target_cluster_object.texture_info.texture_id);
						float tile_size = target_cluster_object.width / retrieved_texture.tiling.x;// since we are only doing this for sqaure grids
						m_tile_system.init(cluster_objects_dest_rect.x, cluster_objects_dest_rect.y, cluster_objects_dest_rect.z, cluster_objects_dest_rect.w, tile_size);
					}
					else {
						// dispose of the tile system 
						/*
							if each of these cluster objects where instead each an independent layer , we would not be having need to
							dispose the tile system when we selected a different layer
							this would have an advantage of being faster
						*/
						m_tile_system.dispose();
					}
				}
			}
			ImGui::NewLine();
		}
		else {
			ImGui::Text("No Texture Object loaded");
		}

		ImGui::NewLine();
		ImGui::Text("New  TileSheet Tiling:");
		ImGui::Text("cols : "); ImGui::SameLine();
		ImGui::InputInt("###n_cols", &m_new_texture_tiling.x);
		if (m_new_texture_tiling.x < 1) {
			m_new_texture_tiling.x = 1;
		}
		ImGui::Text("rows : "); ImGui::SameLine();
		ImGui::InputInt("###n_rows", &m_new_texture_tiling.y);
		if (m_new_texture_tiling.y < 1) {
			m_new_texture_tiling.y = 1;
		}

		if (ImGui::Button("Get Texture ")) {
			m_show_open_tile_sheet_file_dialog = true;
		}



		ImGui::Text("%.2f", static_cast<float>(m_game_ptr->getFps()));
		if (m_show_open_tile_sheet_file_dialog) {
			ImGui::OpenPopup("Open TileSheet File");
		}

		if (m_show_about_dialog) {
			ImGui::OpenPopup("About");
		}

		if (m_file_dialog.showFileDialog("Open TileSheet File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png"))
		{
			loadTextureObject(m_file_dialog.selected_path, m_file_dialog.selected_fn, m_new_texture_tiling);

		}

		if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BulletText("This is a tiling visualizing tool");
			ImGui::BulletText("Built and maintained by Stande Studios ");
			ImGui::BulletText("Established 2021");

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_about_dialog = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	ImGui::End();
	//resets 
	m_show_open_tile_sheet_file_dialog = false;

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Second_Screen::showMenuMain()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{

			if (ImGui::MenuItem("Open", NULL))
			{
				m_show_open_tile_sheet_file_dialog = true;
			}

			if (ImGui::BeginMenu("Open Recent"))
			{
				for (unsigned int i = 0; i < m_vec_of_texture_objects.size(); i++)
				{
					const auto& retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(m_vec_of_texture_objects[i].texture_info.texture_id);
					if (ImGui::MenuItem(retrieved_texture.asset_file_path.c_str(), NULL, false, true)) {
						m_active_texture_object_index = i;
						m_current_texture_object_tiling = retrieved_texture.tiling;
					};
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete Current Texture", NULL)) {
				int counter = 0;
				for (auto it = m_vec_of_texture_objects.begin(); it != m_vec_of_texture_objects.end(); it++)
				{
					if (counter == m_active_texture_object_index) {
						m_vec_of_texture_objects.erase(it);

						// adjust the m_active_object_index
						m_active_texture_object_index--;
						if (m_active_texture_object_index < 0) {
							m_active_texture_object_index = 0;
						}
						break; // using return here caused an out of access memory leaked else where in 
						// the program
					}
					counter++;
				}
			}
			if (ImGui::MenuItem("About", NULL)) {
				m_show_about_dialog = true;
			}
			if (ImGui::MenuItem("Quit", NULL)) {
				m_game_ptr->initiateExit();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
void Second_Screen::loadTextureObject(std::string file_path_, std::string file_name_, glm::vec2 tiling_)
{
	SquareBox::AssetManager::GLTexture loaded_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(file_path_);
	SquareBox::AssetManager::TextureManager::setTextureTilingById(loaded_texture.id, tiling_);
	SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(loaded_texture.id, file_name_);
	auto retrieved_texture = SquareBox::AssetManager::TextureManager::getTextureById(loaded_texture.id);

	m_vec_of_texture_objects.emplace_back();
	auto& new_texture_object = m_vec_of_texture_objects.back();
	//Setting up our object
	new_texture_object.texture_info.color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white).getIVec4();
	glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	new_texture_object.position = screen_dimensions * glm::vec2(0.5f); //position the texture at the center 
	//of the screen
	new_texture_object.shape = SquareBox::BodyShapeEnum::Box;
	new_texture_object.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	new_texture_object.texture_info.texture_id = retrieved_texture.id;

	m_active_texture_object_index = m_vec_of_texture_objects.size() - 1;
	m_current_texture_object_tiling = tiling_;

	if (retrieved_texture.tiling.x == retrieved_texture.tiling.y) {
		if (SquareBox::MathLib::Float::areFloatsEqual(new_texture_object.width, new_texture_object.height, 0.1)) {
			SBX_INFO("Texture Qualifies as a square texture");
			new_texture_object.pod_bool = true;
		}
		else {
			new_texture_object.pod_bool = false;
		}
	}
	else {
		new_texture_object.pod_bool = false;
	}

	if (new_texture_object.pod_bool) {
		float x_to_y = SquareBox::MathLib::Float::divideAndGetFloat(retrieved_texture.width, retrieved_texture.height);
		new_texture_object.height = screen_dimensions.y * m_screen_texture_height_ratio; // this 
		new_texture_object.width = new_texture_object.height * x_to_y;

		// updated the tile system
		m_tile_system.resetTileSystem();;
		glm::vec2 target_position = new_texture_object.position;
		glm::vec2 target_dimensions = glm::vec2(new_texture_object.width, new_texture_object.height);
		glm::vec4 cluster_objects_dest_rect = glm::vec4(target_position - (target_dimensions * 0.5f), target_dimensions);
		float tile_size = new_texture_object.width / retrieved_texture.tiling.x;// since we are only doing this for sqaure grids
		m_tile_system.init(cluster_objects_dest_rect.x, cluster_objects_dest_rect.y, cluster_objects_dest_rect.z, cluster_objects_dest_rect.w, tile_size);
	}
}

Second_Screen::~Second_Screen()
{
}