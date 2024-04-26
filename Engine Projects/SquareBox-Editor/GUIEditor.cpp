#include "GUIEditor.h"
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


GUIEditor::GUIEditor()
{
}

int GUIEditor::getNextScreenIndex() const
{
	return m_next_screen_index;
}

int GUIEditor::getPreviousScreenIndex() const
{
	return m_previous_screen_index;
}

void GUIEditor::build()
{
}

void GUIEditor::onEntry()
{
	//Init Shaders
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	//the debugProgram
	m_debug_program.compileDefaultDebugShaders();
	m_debug_program.addDefaultDebugAttributes();
	m_debug_program.linkShaders();

	m_sprite_batch.init();
	m_debug_renderer.init();

	//Init Cameras
	glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions.x, screen_dimensions.y);
	m_camera.setScale(1.0f);//The Zoom of the Camera
	m_camera.setPosition(screen_dimensions * glm::vec2(0.5f));

	m_utilities.init();

	m_gui_layer.gui_elements.emplace_back();
	m_current_gui_element_ptr = &m_gui_layer.gui_elements.back();

	//avaliable shapes
	m_gui_element_shapes_ptr_vec.push_back(new SquareBox::GUI::BoxGUIElement());
	m_gui_element_shapes_ptr_vec.push_back(new SquareBox::GUI::CircleGUIElement());
	m_shapes_labels_ptr = new const char* [m_gui_element_shapes_ptr_vec.size()];

	m_gui_element_states_labels_ptr = new const char* [6];
	m_gui_element_states_labels_ptr[0] = ToString(SquareBox::GUIElementStateEnum::ACTIVE);
	m_gui_element_states_labels_ptr[1] = ToString(SquareBox::GUIElementStateEnum::DEFAULT);
	m_gui_element_states_labels_ptr[2] = ToString(SquareBox::GUIElementStateEnum::DISABLED);
	m_gui_element_states_labels_ptr[3] = ToString(SquareBox::GUIElementStateEnum::FOCUS);
	m_gui_element_states_labels_ptr[4] = ToString(SquareBox::GUIElementStateEnum::HOVER);
	m_gui_element_states_labels_ptr[5] = ToString(SquareBox::GUIElementStateEnum::SELECTED);

	m_text_justification_lables_ptr = new const char* [3];
	m_text_justification_lables_ptr[0] = ToString(SquareBox::JustificationEnum::LEFT);
	m_text_justification_lables_ptr[1] = ToString(SquareBox::JustificationEnum::RIGHT);
	m_text_justification_lables_ptr[2] = ToString(SquareBox::JustificationEnum::MIDDLE);

	initGUI();

	updateState();//Since m_current_gui_element_ptr has just been assigned
}

void GUIEditor::update(const float& deltaTime_)
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

		//forcing imgui to start responding
		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::ESCAPE))) {
			ImGui::SetNextFrameWantCaptureMouse(true);
			ImGui::SetNextFrameWantCaptureKeyboard(true);
			/* 
			
			this is the hack that i have come up with to solve the issue of imgui freezing up 
			and not allowing any more user input, this block of code  forces it to start
			taking user input but it disoragnises sdls input manager 
			so this function only helps you be able to save your work 
			but a restart of the program is needed after you have used it
			
			*/
		}
	}

	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	//Called once every game loop , and updates what will be drawn
	m_camera.update(screen_dimensions.x, screen_dimensions.y);

	float visable_area_ratio = 0.9f;
	float calculated_area_width = screen_dimensions.x - m_controls_panel_width;
	float working_area_width = 0.0f;
	float working_area_height = 0.0f;

	if (m_gui_layer.orientation == 0) {
		working_area_width = calculated_area_width;
		working_area_height = calculated_area_width * 0.7f;// a propotional height to width ratio

		if (working_area_height > screen_dimensions.y * visable_area_ratio) {
			working_area_height = screen_dimensions.y * visable_area_ratio;

			working_area_width = working_area_height * (1.0f / 0.7f);// this maintains the ratio
		}
	}
	else if(m_gui_layer.orientation == 1){
		//Portriat
		working_area_width = calculated_area_width * 0.4f;
		working_area_height = calculated_area_width * 0.7f;// a propotional height to width ratio

		if (working_area_height > screen_dimensions.y * visable_area_ratio) {
			working_area_height = screen_dimensions.y * visable_area_ratio;

			working_area_width = working_area_height * (0.4f / 0.7f);// this maintains the ratio
		}
	}
	else {
		SBX_CRITICAL("Invalid layer orientation type");
	}


	if (working_area_height > screen_dimensions.y * visable_area_ratio) {
		working_area_height = screen_dimensions.y * visable_area_ratio;
	}

	glm::vec2 working_area_center(calculated_area_width * 0.5f, screen_dimensions.y * 0.5f);
	glm::vec2 visable_area_dimenions(working_area_width * visable_area_ratio, working_area_height * visable_area_ratio);
	m_visiable_dest_rect = glm::vec4((working_area_center - visable_area_dimenions * 0.5f), visable_area_dimenions);

	// The default button state should be default
	// Until changed by user input

	for (unsigned int i = 0; i < m_gui_layer.gui_elements.size(); i++)
	{
		m_gui_layer.gui_elements[i].state = SquareBox::GUIElementStateEnum::DEFAULT;
	}

	if (m_game_ptr->isProcessingInput()) {
		if (m_gui_editor_mode == GUIEditorModeEnum::PLACE){
			if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {

				glm::vec2& mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

				if (m_utilities.isInBox(mouse_in_world, m_visiable_dest_rect)) {
					bool well_built = false;
					if (m_current_gui_element_shape_shell_ptr->needs_height_width) {
						if (m_current_gui_element_ptr->height_ratio > 0.0f && m_current_gui_element_ptr->width_ratio > 0.0f) {
							well_built = true;
						}
						else {
							SBX_ERROR("Invalid Box Shape dimensions");
						}
					}
					else if (m_current_gui_element_shape_shell_ptr->needs_radius) {
						if (m_current_gui_element_ptr->radius_ratio > 0.0f) {
							well_built = true;
						}
						else {
							SBX_ERROR("Invalid Circle Shape dimensions");
						}
					}

					if (well_built) {
						setCurrentGuiElementPtrLocationRatio();
						m_current_gui_element_ptr->is_alive = true;
						//move on to the next
						m_gui_layer.gui_elements.emplace_back();
						m_current_gui_element_ptr = &m_gui_layer.gui_elements.back();
						updateState();
					}
					else {
						SBX_ERROR("Invalid {} Gui Element Dimensions", ToString(static_cast<SquareBox::GUIElementShapeEnum>(m_selected_gui_element_shape_index)));
					}
				}
				else {
					SBX_ERROR("Can not place elements out side the visable area");
				}
			}
		}
		else if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
			for (auto& it = m_gui_layer.gui_elements.rbegin(); it != m_gui_layer.gui_elements.rend(); ++it) {
				 auto& gui_element = (*it);
				if (gui_element.is_alive) {
					glm::vec2& mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
					guiElementShapeShellSetter(gui_element);
					if (!gui_element.is_locked && m_current_gui_element_shape_shell_ptr->containtsPoint(gui_element, mouse_in_world, m_visiable_dest_rect)) {
						gui_element.state = SquareBox::GUIElementStateEnum::HOVER;
						if (
							m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))
							) {
							gui_element.state = SquareBox::GUIElementStateEnum::ACTIVE;
							if (m_active_selecting) {
								//selecting for editing
								m_current_gui_element_ptr = &gui_element;
								updateState();
							}
							else {
								/*
								we do not intend to really select the object for editing but just to see 
								its' gui reaction.
								*/
							}
							
						}
						break;//we can't be hovering more than one element at a time
					}
				}
			}

			//changing position or dragging an element
			if (
				m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))
				) {
				if (m_current_gui_element_ptr != nullptr) {
					glm::vec2& mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
					if (m_utilities.isInBox(mouse_in_world, m_visiable_dest_rect)) {
						setCurrentGuiElementPtrLocationRatio();
					}
				}
			}

			//deleting
			if (m_current_gui_element_ptr != nullptr) {
				if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEYBOARD_DELETE))) {
					m_current_gui_element_ptr->is_to_be_deleted = true;
				}
			}

			for (auto & it = m_gui_layer.gui_elements.begin(); it != m_gui_layer.gui_elements.end();)
			{
				if ((*it).is_to_be_deleted) {
					it = m_gui_layer.gui_elements.erase(it);
					m_current_gui_element_ptr = nullptr;
					/* since the selected the object will always be the m_current_gui_element_ptr and thats */
					break; // since 
				}
				++it;
			}
		}
	}
}

void GUIEditor::draw()
{
	m_sprite_batch.begin(SquareBox::RenderEngine::NONE);

	for each (const auto & gui_element in m_gui_layer.gui_elements)
	{
		guiElementShapeShellSetter(gui_element);
		m_current_gui_element_shape_shell_ptr->drawTexture(gui_element, m_sprite_batch, m_visiable_dest_rect);
		const auto& font_info = gui_element.fonts[static_cast<int>(gui_element.state)];
		if (font_info.show_text && m_gui_layer.sprite_fonts.size() > 0) {
			m_current_gui_element_shape_shell_ptr->drawText(gui_element, m_gui_layer.sprite_fonts[font_info.font_index], m_sprite_batch, m_camera.getScale(), m_visiable_dest_rect);
		}
	}


	m_sprite_batch.end();
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();


	//debug draw
	m_debug_renderer.begin();
	m_debug_renderer.end();
	/*
	 having this end here is confusing, my logic must be very wrong
	
	*/
	m_debug_renderer.drawBox(m_visiable_dest_rect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);

	//trace drawing
	if (m_gui_editor_mode == GUIEditorModeEnum::PLACE) {
		guiElementShapeShellSetter(*m_current_gui_element_ptr);
		setCurrentGuiElementPtrLocationRatio();
		m_current_gui_element_shape_shell_ptr->traceDraw(*m_current_gui_element_ptr, m_debug_renderer, m_camera.getScale(), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), m_visiable_dest_rect);
	}

	if (m_debug_mode) {
		//debug linees
		float line_width = m_visiable_dest_rect.z * 0.5f;
		float line_height = m_visiable_dest_rect.w * 0.5f;

		glm::vec2 visiable_area_center = (glm::vec2(m_visiable_dest_rect.x, m_visiable_dest_rect.y) + (glm::vec2(m_visiable_dest_rect.z, m_visiable_dest_rect.w) * 0.5f));
		// +X
		m_debug_renderer.drawLine(visiable_area_center, glm::vec2(visiable_area_center.x + line_width, visiable_area_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 200));
		// -X axis
		m_debug_renderer.drawLine(visiable_area_center, glm::vec2(visiable_area_center.x - line_width, visiable_area_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 100));

		// +Y axis
		m_debug_renderer.drawLine(visiable_area_center, glm::vec2(visiable_area_center.x, visiable_area_center.y + line_height), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 200));
		// -Y axis
		m_debug_renderer.drawLine(visiable_area_center, glm::vec2(visiable_area_center.x, visiable_area_center.y - line_height), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 100));

		// elements
		for each (auto & gui_element in m_gui_layer.gui_elements)
		{
			guiElementShapeShellSetter(gui_element);
			m_current_gui_element_shape_shell_ptr->debugDraw(gui_element, m_debug_renderer, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), m_visiable_dest_rect);
		}
	}

	if (m_gui_editor_mode == GUIEditorModeEnum::SELECT && m_current_gui_element_ptr != nullptr) {
		guiElementShapeShellSetter(*m_current_gui_element_ptr);
		m_current_gui_element_shape_shell_ptr->debugDraw(*m_current_gui_element_ptr, m_debug_renderer, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow), m_visiable_dest_rect);
	}

	m_debug_program.use();
	uploadCameraInfo(&m_debug_program, &m_camera, "P");
	m_debug_renderer.render();
	m_debug_program.unuse();


	bool before_is_locked = true;
	if (m_current_gui_element_ptr != nullptr) {
		before_is_locked = m_current_gui_element_ptr->is_locked;
	}
	drawGUI();//this is where is_locked changes from when the user uses the gui to unlock a gui element
	//having this here avoids Imgui end frame crashes
	if (m_current_gui_element_ptr != nullptr) {
		if (m_current_gui_element_ptr->is_locked && !before_is_locked && m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
			m_current_gui_element_ptr = nullptr;
		}
	}
}

void GUIEditor::onExit()
{
	//this is where all screen process are stopped
	//this is where objects that the screen used are destroyed from
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();

	m_texture_program.dispose();
	m_utilities.dispose();
	m_debug_program.dispose();

	for (unsigned int i = 0; i < m_gui_layer.sprite_fonts.size(); i++)
	{
		m_gui_layer.sprite_fonts[i].dispose();
	}

	m_gui_layer= SquareBox::GWOM::GuiLayer();
	disposeGUI();

	for each (auto & m_gui_element_shapes_ptr in m_gui_element_shapes_ptr_vec)
	{
		delete m_gui_element_shapes_ptr;
	}
	m_gui_element_shapes_ptr_vec.clear();

	//need a better fix for a clean up here
	m_texture_program = SquareBox::RenderEngine::GLSLProgram();
	m_debug_program = SquareBox::RenderEngine::GLSLProgram();
}

void GUIEditor::destroy()
{
}

void GUIEditor::initGUI()
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
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_game_ptr->getWindow()->getWindowPointerForImGui(), m_game_ptr->getWindow()->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);
	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Comfortaa-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_ERROR("Failed to load font Comfortaa-Medium.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void GUIEditor::disposeGUI() {
	// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
}

void GUIEditor::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMenuMain();
	ImGuiWindowFlags properties_tab_window_flags = 0;
	properties_tab_window_flags |= ImGuiWindowFlags_NoMove;
	bool* properties_tab_open = false;
	ImGui::Begin("Properties Panel", properties_tab_open, properties_tab_window_flags);

	ImGui::SetWindowPos(ImVec2(m_game_ptr->getWindow()->getScreenWidth() - ImGui::GetWindowWidth() - 2, m_main_menu_bar_height));
	{
		m_controls_panel_width = ImGui::GetWindowWidth();

		ImGui::Text("FPS Counter: %.2f", static_cast<float>(m_game_ptr->getFps()));
		ImGui::Text("Elapsed Sec: %.2f", static_cast<float>(m_game_ptr->getGameLoopElapsedSeconds()));
		ImGui::Separator();
		static int mode = 0;
		int beforeMode = mode;
		ImGui::RadioButton("Place Mode  ", &mode, 0); ImGui::SameLine();
		ImGui::RadioButton("Select Mode ", &mode, 1); ImGui::SameLine();

		m_gui_editor_mode = static_cast<GUIEditorModeEnum>(mode);

		if (beforeMode != mode) {
			if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
				m_current_gui_element_ptr = nullptr;
			}
			else {
				m_current_gui_element_ptr = &m_gui_layer.gui_elements.back();
			}
		}
		ImGui::Checkbox("Debug Mode", &m_debug_mode);
		ImGui::RadioButton("Landscape   ", &m_gui_layer.orientation, 0); ImGui::SameLine();
		ImGui::RadioButton("Portriat     ", &m_gui_layer.orientation, 1);
		if (ImGui::BeginTabBar("Panel A"))
		{
			if (ImGui::BeginTabItem("Element")) {


				if (m_current_gui_element_ptr != nullptr) {

					ImGui::Separator();
					ImGui::Text("ID        : "); ImGui::SameLine();
					ImGui::InputInt("###button_id", &m_current_gui_element_ptr->id, 1, 10);
					ImGui::Text("Name      : "); ImGui::SameLine();
					ImGui::InputText("###button_name", m_current_gui_element_ptr->name, 20);
					ImGui::Text("Text      : "); ImGui::SameLine();
					ImGui::InputText("###state_text", m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].text, 20);
					ImGui::Text("Show Text : "); ImGui::SameLine();
					ImGui::Checkbox("###show_text", &m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].show_text);
					ImGui::Text("Show Just : "); ImGui::SameLine();
					int target_text_justification = static_cast<int>(m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].justification);
					ImGui::Combo("###TextJustification", &target_text_justification, m_text_justification_lables_ptr, 3);
					m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].justification = static_cast<SquareBox::JustificationEnum>(target_text_justification);
					//text color
					ImGui::Text("Text Color: "); ImGui::SameLine();
					static float col2[4];

					/*
					 i do not know what is wrong with the color, it's not maintaining it's state value
					 i think imgui does something behind the scenes because my logic looks very okay to me
					*/


					auto& state_font_color = m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].color;

					col2[0] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_font_color.x), static_cast<float>(255));
					col2[1] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_font_color.y), static_cast<float>(255));
					col2[2] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_font_color.z), static_cast<float>(255));
					col2[3] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_font_color.w), static_cast<float>(255));

					ImGui::ColorEdit4("###text_color", col2);
					state_font_color.x = static_cast<int>(col2[0] * 255);
					state_font_color.y = static_cast<int>(col2[1] * 255);
					state_font_color.z = static_cast<int>(col2[2] * 255);
					state_font_color.w = static_cast<int>(col2[3] * 255);

					ImGui::Text("Angle     : "); ImGui::SameLine();
					ImGui::InputFloat("###button_angle", &m_current_gui_element_ptr->angle, 0.01, 1.0f);
					ImGui::Text("Color     : "); ImGui::SameLine();
					auto& state_texture_color = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].color;

					col2[0] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_texture_color.x), static_cast<float>(255));
					col2[1] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_texture_color.y), static_cast<float>(255));
					col2[2] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_texture_color.z), static_cast<float>(255));
					col2[3] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(state_texture_color.w), static_cast<float>(255));

					ImGui::ColorEdit4("###button_color", col2);
					state_texture_color.x = static_cast<int>(col2[0] * 255);
					state_texture_color.y = static_cast<int>(col2[1] * 255);
					state_texture_color.z = static_cast<int>(col2[2] * 255);
					state_texture_color.w = static_cast<int>(col2[3] * 255);

					SquareBox::GWOM::TextureInfo& texture_info = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index];
					ImGui::Text("Opacity   : "); ImGui::SameLine();
					ImGui::InputInt("###button_opactity", &texture_info.opacity, 1, 10);
					ImGui::Text("Tx Opacity: "); ImGui::SameLine();
					ImGui::InputInt("###text_opactity", &m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].opacity, 1, 10);

					ImGui::Text("Text scale: "); ImGui::SameLine();
					ImGui::InputFloat("###text_to_box_height_scale", &m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].text_to_box_height_scale, 0.05f, 0, "%.3f");

					ImGui::Text("Hidden    : "); ImGui::SameLine();
					ImGui::Checkbox("###is_hidden", &m_current_gui_element_ptr->is_hidden);

					ImGui::Text("Locked    : "); ImGui::SameLine();
					ImGui::Checkbox("###is_locked", &m_current_gui_element_ptr->is_locked);

					ImGui::Separator();

					ImGui::RadioButton("Individual", &m_edit_states_individually, 1); ImGui::SameLine();
					ImGui::RadioButton("All", &m_edit_states_individually, 0);

					ImGui::Spacing();
					//display our shapes names
					int beforeState = m_selected_gui_element_state_index;
					ImGui::Text("State     : "); ImGui::SameLine();
					if (!m_edit_states_individually) {
						ImGui::BeginDisabled();
						adaptAllStatesToAUniformState();
						ImGui::Combo("###GUIElementState", &m_selected_gui_element_state_index, m_gui_element_states_labels_ptr, 6);
						ImGui::EndDisabled();
					}
					else {
						ImGui::Combo("###GUIElementState", &m_selected_gui_element_state_index, m_gui_element_states_labels_ptr, 6);
					}

					if (beforeState != m_selected_gui_element_state_index) {
						m_current_gui_element_ptr->state = static_cast<SquareBox::GUIElementStateEnum>(m_selected_gui_element_state_index);
						updateState();
					}

					ImGui::Text("Shape     : "); ImGui::SameLine();
					for (unsigned int i = 0; i < m_gui_element_shapes_ptr_vec.size(); i++)
					{
						m_shapes_labels_ptr[i] = ToString(m_gui_element_shapes_ptr_vec[i]->shape);
					}

					if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
						ImGui::BeginDisabled();
					}
					ImGui::Combo("###Shape", &m_selected_gui_element_shape_index, m_shapes_labels_ptr, m_gui_element_shapes_ptr_vec.size());
					if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
						ImGui::EndDisabled();
					}

					m_current_gui_element_ptr->shape = m_gui_element_shapes_ptr_vec[m_selected_gui_element_shape_index]->shape;

					for (unsigned int i = 0; i < m_gui_element_shapes_ptr_vec.size(); i++)
					{
						auto& focus_shape_ptr = m_gui_element_shapes_ptr_vec[i];
						if (m_selected_gui_element_shape_index == i) {
							ImGui::Spacing();
							ImGui::Text("%s Dimensions", ToString(focus_shape_ptr->shape));
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							if (focus_shape_ptr->needs_height_width) {
								ImGui::Checkbox("###draw_perfect_square", &m_current_gui_element_ptr->draw_perfect_square);
								
								if (m_current_gui_element_ptr->draw_perfect_square) {
									ImGui::Text("Square    : "); ImGui::SameLine();
									ImGui::InputInt("###perfect_square_dimensions", &m_current_gui_element_ptr->perfect_square_dimensions, m_visiable_dest_rect.z * 0.01, m_visiable_dest_rect.z*0.1);
									m_current_gui_element_ptr->height_ratio = SquareBox::MathLib::Float::divideAndGetFloat(m_current_gui_element_ptr->perfect_square_dimensions, m_visiable_dest_rect.w);
									m_current_gui_element_ptr->width_ratio = SquareBox::MathLib::Float::divideAndGetFloat(m_current_gui_element_ptr->perfect_square_dimensions, m_visiable_dest_rect.z);
								}
								else {
									ImGui::Text("Height    : "); ImGui::SameLine();
									ImGui::InputFloat("###b_h", &m_current_gui_element_ptr->height_ratio, 0.05f, 0, "%.3f");
									ImGui::Text("Width     : "); ImGui::SameLine();
									ImGui::InputFloat("###b_w", &m_current_gui_element_ptr->width_ratio, 0.05f, 0, "%.3f");
								}
								

							}
							if (focus_shape_ptr->needs_radius) {
								ImGui::Text("Radius    : "); ImGui::SameLine();
								ImGui::InputFloat("###c_r", &m_current_gui_element_ptr->radius_ratio, 0.025f, 0, "%.3f");
							}

							if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
								ImGui::Text("Location Ratios");
								ImGui::Text("X axis     : "); ImGui::SameLine();
								ImGui::InputFloat("###x_axis", &m_current_gui_element_ptr->location_ratio.x, 0.05f, 0, "%.3f");
								ImGui::Text("Y axis     : "); ImGui::SameLine();
								ImGui::InputFloat("###y_axis", &m_current_gui_element_ptr->location_ratio.y, 0.05f, 0, "%.3f");
							}

						}
					}

					ImGui::Separator();
					if (ImGui::BeginTabBar("State Propteries"))
					{
						if (ImGui::BeginTabItem("Texture")) {



							int m_selected_texture_type = static_cast<int>(m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_type);
							ImGui::RadioButton("Single", &m_selected_texture_type, static_cast<int>(SquareBox::TextureEnum::SINGLE)); ImGui::SameLine();
							ImGui::RadioButton("TileSheet", &m_selected_texture_type, static_cast<int>(SquareBox::TextureEnum::TILESHEET));
							m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_type = static_cast<SquareBox::TextureEnum>(m_selected_texture_type);
						
							if (m_selected_texture_type == static_cast<int>(SquareBox::TextureEnum::SINGLE)) {
								ImGui::Text("uvRect ");
								ImGui::Text("uv_x      : "); ImGui::SameLine();
								ImGui::InputFloat("###uv_x", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.x, 0.01f, 0, "%.2f");
								ImGui::Text("uv_y      : "); ImGui::SameLine();
								ImGui::InputFloat("###uv_y", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.y, 0.01f, 0, "%.2f");
								ImGui::Text("uv_z      : "); ImGui::SameLine();
								ImGui::InputFloat("###uv_z", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.z, 0.01f, 0, "%.2f");
								ImGui::Text("uv_w      : "); ImGui::SameLine();
								ImGui::InputFloat("###uv_w", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.w, 0.01f, 0, "%.2f");

								//compute how many single textures we want to display
								//we are only displaying those that are not empty strings
								if (m_gui_layer.single_textures.size() > 0) {
									std::vector<char*>  vc;
									vc.reserve(m_gui_layer.single_textures.size());
									std::transform(m_gui_layer.single_textures.begin(), m_gui_layer.single_textures.end(), std::back_inserter(vc), [](const SquareBox::AssetManager::GLTexture& texture_)
									{
										//this new here is never deleted, and is called alot, that is a problem to us 
										char* pc = new char[texture_.display_name.size() + 1];
										std::strcpy(pc, texture_.display_name.c_str());
										return pc;
									});
									ImGui::Text("Textures  : "); ImGui::SameLine();
									ImGui::Combo("###Single", &m_selected_single_texture_index, &vc[0], m_gui_layer.single_textures.size());
									//update the current objects texture info
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_index = m_selected_single_texture_index;
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_id = m_gui_layer.single_textures[m_selected_single_texture_index].id;
									//current active texture
									ImTextureID my_tex_id;
									my_tex_id = (ImTextureID)m_gui_layer.single_textures[m_selected_single_texture_index].id;

									float my_tex_w = 50;
									float my_tex_h = 50;
									float zoom = 4.0f;
									float texture_width = m_gui_layer.single_textures[m_selected_single_texture_index].width;
									float texture_height = m_gui_layer.single_textures[m_selected_single_texture_index].height;

									float region_width = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.z * texture_width;
									float region_height = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.w * texture_height;

									float region_x_orign = (m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.x) * texture_width;
									float region_y_orign = (1 - m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.w - m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.y) * texture_height;
									/*dont under stand why we have to subtract the height. this is a classic it works so dont touch it scenario*/
									ImVec2 uv0 = ImVec2((region_x_orign) / texture_width, (region_y_orign) / texture_height);
									ImVec2 uv1 = ImVec2((region_x_orign + region_width) / texture_width, (region_y_orign + region_height) / texture_height);

									ImGui::Text("Preview");
									//ImVec2 uv_min = ImVec2(texture_info.uv_rect.x, texture_info.uv_rect.y);                 // Top-left
									//ImVec2 uv_max = ImVec2(uv_min.x + texture_info.uv_rect.z, uv_min.y+ texture_info.uv_rect.w );                 // Lower-right
									ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
									ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
									ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv0, uv1, tint_col, border_col);
								}
								else {
									ImGui::Text("Textures  : "); ImGui::SameLine();
									ImGui::Combo("###Single", &m_selected_single_texture_index, "Empty", m_gui_layer.single_textures.size());
								}

								if (ImGui::Button("Load Single Texture")) {
									m_show_open_single_texture_file_dialog = true;
								}

							
								
							}
							else {

								//compfute how many tiled textures we want to display
								//we are only displaying those that are not empty strings
								if (m_gui_layer.tiled_textures.size() > 0) {

									//Tiled texture
									std::vector<char*>  vc;
									vc.reserve(m_gui_layer.tiled_textures.size());
									std::transform(m_gui_layer.tiled_textures.begin(), m_gui_layer.tiled_textures.end(), std::back_inserter(vc), [](const SquareBox::AssetManager::GLTexture& texture_)
									{
										//this new here is never deleted, and is called alot, that is a problem to us 
										char* pc = new char[texture_.display_name.size() + 1];
										std::strcpy(pc, texture_.display_name.c_str());
										return pc;
									});
									ImGui::Text("Textures  : "); ImGui::SameLine();
									ImGui::Combo("###TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], m_gui_layer.tiled_textures.size());
									ImGui::Text("Tiling    :");

									ImGui::Text("cols      : "); ImGui::SameLine();
									ImGui::Text("%d", m_gui_layer.tiled_textures[m_selected_tile_sheet_texture_index].tiling.x);
									ImGui::Text("rows      : "); ImGui::SameLine();
									ImGui::Text("%d", m_gui_layer.tiled_textures[m_selected_tile_sheet_texture_index].tiling.y);

									//update the current objects texture info
									auto& selected_tile_sheet_texture = m_gui_layer.tiled_textures[m_selected_tile_sheet_texture_index];
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_index = m_selected_tile_sheet_texture_index;
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_id = selected_tile_sheet_texture.id;
									//current active texture
									ImTextureID my_tex_id = (ImTextureID)selected_tile_sheet_texture.id;
									float my_tex_w = 50;
									float my_tex_h = 50;
									float zoom = 4.0f;
									ImGui::Text("Preview");

									/*
									we update the cluster objects uv coords here .if it is a freelance cluster Object
									*/
									float texture_width = selected_tile_sheet_texture.width;
									float texture_height = selected_tile_sheet_texture.height;

									float region_width = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.z * texture_width;
									float region_height = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.w * texture_height;

									float region_x_orign = (m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.x) * texture_width;
									float region_y_orign = (1 - m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.w - m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect.y) * texture_height;
									/*dont under stand why we have to subtract the height. this is a classic it works so dont touch it scenario*/
									ImVec2 uv0 = ImVec2((region_x_orign) / texture_width, (region_y_orign) / texture_height);
									ImVec2 uv1 = ImVec2((region_x_orign + region_width) / texture_width, (region_y_orign + region_height) / texture_height);

									ImVec2 texture_dimensions(selected_tile_sheet_texture.width, selected_tile_sheet_texture.height);
									//ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);   // Top-left
									//ImVec2 uv_max = ImVec2(uv_min.x +uvRect.z , uv_min.y + uvRect.w);  // Lower-right
									ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
									ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

									ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv0, uv1, tint_col, border_col);
									ImGui::Text("Index: "); ImGui::SameLine();
									//ImGui::InputInt("", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].tile_sheet_index);
									ImGui::DragInt("###m_current_tile_sheet_index", &m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].tile_sheet_index, 1, 0, (selected_tile_sheet_texture.tiling.x * selected_tile_sheet_texture.tiling.y) - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].uv_rect = selected_tile_sheet_texture.getUVReactAtIndex(m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].tile_sheet_index, m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].inversions);
									if (m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].tile_sheet_index < 0) {
										m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].tile_sheet_index = 0;
									}

									ImGui::Spacing();
									ImGui::Text("X Inverted: "); ImGui::SameLine();
									bool inversion_state = static_cast<bool>(m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].inversions.x);
									ImGui::Checkbox("###is_x_inverted", &inversion_state);
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].inversions.x = static_cast<int>(inversion_state);

									inversion_state = static_cast<bool>(m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].inversions.y);

									ImGui::Text("Y Inverted: "); ImGui::SameLine();
									ImGui::Checkbox("###is_y_inverted", &inversion_state);
									m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].inversions.y = static_cast<int>(inversion_state);

									ImGui::Spacing();
								}
								else {

									ImGui::Text("Textures  : "); ImGui::SameLine();
									ImGui::Combo("###TiledSheet", &m_selected_tile_sheet_texture_index, "Empty", m_gui_layer.tiled_textures.size());
								}

								ImGui::Text("New Texture Tiling:");
								ImGui::Text("cols      : "); ImGui::SameLine();
								ImGui::InputInt("###new cols", &m_tiling.x);
								ImGui::Text("rows      : "); ImGui::SameLine();
								ImGui::InputInt("###new rows", &m_tiling.y);

								if (ImGui::Button("Load Tile Sheet Texture")) {
									m_show_open_tile_sheet_texture_file_dialog = true;
								}
							}

							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Font")) {

							if (m_gui_layer.sprite_fonts.size() > 0) {
								std::vector<char*>  vc;
								vc.reserve(m_gui_layer.sprite_fonts.size());
								std::transform(m_gui_layer.sprite_fonts.begin(), m_gui_layer.sprite_fonts.end(), std::back_inserter(vc), [](const SquareBox::RenderEngine::SpriteFont& font)
								{
									//this new here is never deleted, and is called alot, that is a problem to us 
									char* pc = new char[font.getDisplayName().size() + 1];
									std::strcpy(pc, font.getDisplayName().c_str());
									return pc;
								});

								ImGui::Text("Fonts     : "); ImGui::SameLine();
								ImGui::Combo("###Font", &m_selected_font_index, &vc[0], m_gui_layer.sprite_fonts.size());
								if (m_current_gui_element_ptr) {
									ImGui::Text("Size      : %d", m_current_gui_element_ptr->fonts[m_selected_font_index].size);
								}
								m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].font_index = m_selected_font_index;

							}
							else {
								ImGui::Text("Fonts     : "); ImGui::SameLine();
								ImGui::Combo("###Font", &m_selected_font_index, "Empty", m_gui_layer.sprite_fonts.size());
							}


							ImGui::Text("New Font Size:");
							ImGui::Text("Size      : "); ImGui::SameLine();
							ImGui::InputInt("###new font size", &m_font_size);

							if (ImGui::Button("Load Font File")) {

								m_show_open_font_file_dialog = true;
							}
							ImGui::EndTabItem();
						}

						ImGui::NewLine();
						ImGui::NewLine();
						ImGui::EndTabBar();
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Layering")) {

				if (m_gui_layer.gui_elements.size() > 1) {//because the trace object can exit but if its alone
					//then the gui_elements array is techincally empty

					if (ImGui::BeginTable("layer layout", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
					{
						//ImGuiTabBarFlags_FittingPolicyResizeDown
						std::pair<int, int> target_shift(0, 0);//(inde of element, shift direction)
						for (int i = 0; i < m_gui_layer.gui_elements.size(); i++)
						{
							auto& focus_gui_element = m_gui_layer.gui_elements[i];
							if (focus_gui_element.is_alive) {//last element excluded
								ImGui::TableNextColumn();
								if (ImGui::IsItemClicked(ImGui::ArrowButton("up", ImGuiDir_Up))) {
									target_shift = std::pair<int, int>(i, -1);
								}

								ImGui::TableNextColumn();
								if (ImGui::IsItemClicked(ImGui::ArrowButton("down", ImGuiDir_Down))) {
									target_shift = std::pair<int, int>(i, 1);
								}

								ImGui::TableNextColumn();
								if (focus_gui_element.is_hidden) {
									if (ImGui::IsItemClicked(ImGui::SmallButton("S"))) {
										focus_gui_element.is_hidden = false;
									}
								}
								else {
									if (ImGui::IsItemClicked(ImGui::SmallButton("H"))) {
										focus_gui_element.is_hidden = true;
									}
								}


								ImGui::TableNextColumn();
								if (m_gui_editor_mode == GUIEditorModeEnum::PLACE) {
									ImGui::BeginDisabled();
								}
								if (&focus_gui_element == m_current_gui_element_ptr) {
									ImGui::Text("A");
								}
								else {
									if (ImGui::IsItemClicked(ImGui::SmallButton("_"))) {
										if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
											m_current_gui_element_ptr = &focus_gui_element;
											updateState();
										}
									}
								}
								if (m_gui_editor_mode == GUIEditorModeEnum::PLACE) {
									ImGui::EndDisabled();
								}

								ImGui::TableNextColumn();
								
								sprintf(m_buffer, "###gui_element_id%d", i);
								ImGui::InputInt(m_buffer, &focus_gui_element.id, 1, 10);

								ImGui::TableNextColumn();
								sprintf(m_buffer, "###gui_element_name%d", i);
								ImGui::InputText(m_buffer, focus_gui_element.name, 100);
							}
						}
						ImGui::EndTable();

						if (target_shift != std::pair<int, int>(0, 0)) {
							//a gui element it being shifted
							int current_index = target_shift.first;
							int desired_index = current_index + target_shift.second;
							if (desired_index >= 0 && desired_index < m_gui_layer.gui_elements.size() - 1) {//last element excluded since its the current trace element 

								auto element_a = m_gui_layer.gui_elements[current_index];
								auto element_b = m_gui_layer.gui_elements[desired_index];

								m_gui_layer.gui_elements[desired_index] = element_a;
								m_gui_layer.gui_elements[current_index] = element_b;

								if (m_gui_editor_mode == GUIEditorModeEnum::SELECT) {
									m_current_gui_element_ptr = nullptr;
									/*
										this is meant to protect us from us having have shift the selected cluster object
										but still pointing at its previous memory address
										a better apporach would be to have indexs stored , but those would to be
										updated when a deletion occurs and that sounds like stress.

										having a index system would be great, so thats something i will work on next
										because with it , i can even have a select button here with me


										but then again, i can just have a for loop lopping through the ids like i currently have 

									*/
								}
							}
						}
					}
				}
				else {
					ImGui::Text("Layer is Empty");
				}

				ImGui::EndTabItem();
			}
			ImGui::Spacing();
			ImGui::EndTabBar();
		}
		if (m_show_open_gui_layer_file_dialog) {
			ImGui::OpenPopup("Open Gui Layer File");
		}

		if (m_show_about_dialog) {
			ImGui::OpenPopup("About?");
		}

		if (m_show_save_gui_layer_file_dialog) {
			ImGui::OpenPopup("Save Gui Layer File");
		}

		if (m_show_open_single_texture_file_dialog) {
			ImGui::OpenPopup("Open Single Texture File");
		}

		if (m_show_open_tile_sheet_texture_file_dialog) {
			ImGui::OpenPopup("Open Tile Sheet Texture File");
		}

		if (m_show_open_font_file_dialog) {
			ImGui::OpenPopup("Open Font File");
		}
		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */

		 /*
		   this simple texture opening system gives us the ability to edit texture properties straight out of the box
		 */
		if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("ABOUT:");
			ImGui::BulletText("This is a 2D Graphics User Interface Editing tool");
			ImGui::BulletText("Built and maintained by stande studios ");
			ImGui::BulletText("Established 2021");

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (m_file_dialog.showFileDialog("Open Gui Layer File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_gui_layer_extensions))
		{
			SBX_INFO("Starting Data Loading.");
			SBX_INFO("Loading {}", m_file_dialog.selected_fn);
			cleanOutGUILayer();
			if (m_gui_layer_reader_writer.loadGuiLayerDataAsBinary(m_file_dialog.selected_path, m_gui_layer)) {

				m_utilities.loadGuiLayerTexturesAndFonts(m_gui_layer, m_game_ptr->getWindow()->getDDPI());

				if (m_gui_editor_mode == GUIEditorModeEnum::PLACE) {
					m_current_gui_element_ptr = &m_gui_layer.gui_elements.back();
					updateState();
				}
				else {
					//we are in select mode
					m_current_gui_element_ptr = nullptr;
				}
			}
			else {
				SBX_ERROR("Failed to open {} ", m_file_dialog.selected_fn);
				createNewGUILayer();
			}
		}
		if (m_file_dialog.showFileDialog("Open Single Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			auto temp_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(temp_texture.id, m_tiling);// tiling can always be overwitten

			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_gui_layer.single_textures.size(); i++)
			{
				if (m_gui_layer.single_textures[i].asset_file_path == temp_texture.asset_file_path) {
					m_selected_single_texture_index = i;
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {

				SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(temp_texture.id, m_file_dialog.selected_fn);
				m_gui_layer.single_textures.push_back(SquareBox::AssetManager::TextureManager::getTextureById(temp_texture.id));
				m_selected_single_texture_index = m_gui_layer.single_textures.size() - 1;
			}
		}

		if (m_file_dialog.showFileDialog("Open Tile Sheet Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			auto temp_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(temp_texture.id, m_tiling);// tiling can always be overwitten

			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_gui_layer.tiled_textures.size(); i++)
			{
				if (m_gui_layer.tiled_textures[i].asset_file_path == temp_texture.asset_file_path) {
					m_selected_tile_sheet_texture_index = i;
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {

				SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(temp_texture.id, m_file_dialog.selected_fn);
				m_gui_layer.tiled_textures.push_back(SquareBox::AssetManager::TextureManager::getTextureById(temp_texture.id));
				m_selected_tile_sheet_texture_index = m_gui_layer.tiled_textures.size() - 1;
			}
		}

		if (m_file_dialog.showFileDialog("Open Font File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_font_file_extensions))
		{
			SquareBox::RenderEngine::SpriteFont temp_sprite_font;
			temp_sprite_font.initWithFilePath(m_file_dialog.selected_path, m_font_size);
			temp_sprite_font.setDisplayName(m_file_dialog.selected_fn);

			//first confirm that we don't have this font already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_gui_layer.sprite_fonts.size(); i++)
			{
				if (m_gui_layer.sprite_fonts[i].getFontFilePath() == temp_sprite_font.getFontFilePath()) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {

				m_gui_layer.sprite_fonts.push_back(temp_sprite_font);
			}
			else {
				temp_sprite_font.dispose();
			}
		}


		if (m_file_dialog.showFileDialog("Save Gui Layer File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_gui_layer_extensions))
		{
			SBX_INFO("Saving Gui Layer as {} File", m_gui_layer_extensions);
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

			//i don't know if we need to clean up or not.
			/*
				 we do so that textures get transfered to the right folders when we save the gui layer
				 right now this approach that we are using requires the textures to be in the right
				 folders by default , so accesing a texture from the downloads folder while
				 working wil work but that image wont be loaded the next time the gui is loaded
			*/
			m_gui_layer.editing_screen_dimensions = glm::vec2(m_visiable_dest_rect.z, m_visiable_dest_rect.w);
			if (m_gui_layer_reader_writer.saveGuiLayerDataAsBinary(filePath + ext, m_gui_layer)) {
				SBX_INFO("GUI layer successfully saved at {}", m_file_dialog.selected_path);
			}
		}

		//reset of all dialogs
		m_show_open_gui_layer_file_dialog = false;
		m_show_save_gui_layer_file_dialog = false;
		m_show_open_single_texture_file_dialog = false;
		m_show_open_tile_sheet_texture_file_dialog = false;
		m_show_open_font_file_dialog = false;
		m_show_about_dialog = false;
	}

	ImGui::End();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void GUIEditor::showMenuMain()
{
	ImGui::BeginMainMenuBar();
	m_main_menu_bar_height = ImGui::GetWindowHeight();
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("About", NULL)) {
			m_show_about_dialog = true;
		}
		if (ImGui::MenuItem("Open", "CTRL+O")) {
			m_show_open_gui_layer_file_dialog = true;
		}
		if (ImGui::MenuItem("Save", "CTRL+S")) {
			m_show_save_gui_layer_file_dialog = true;
		}
		if (ImGui::MenuItem("New", "CTRL+N")) {
			createNewGUILayer();
		}
		if (ImGui::MenuItem("Active Selecting",NULL,m_active_selecting)) {
			m_active_selecting = !m_active_selecting;
			if (!m_active_selecting && m_gui_editor_mode==GUIEditorModeEnum::SELECT) {
				m_current_gui_element_ptr = nullptr;
			}
		}
		if (ImGui::MenuItem("unLock all", NULL)) {
			for (unsigned int i = 0; i < m_gui_layer.gui_elements.size(); i++)
			{
				m_gui_layer.gui_elements[i].is_locked = false;
			}
		}
		if (ImGui::MenuItem("Home", NULL)) {
			setScreenState(SquareBox::ScreenState::CHANGE_PREVIOUS);
		}
		if (ImGui::MenuItem("Quit", NULL)) {
			m_game_ptr->initiateExit();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void GUIEditor::updateState()
{
	m_selected_gui_element_shape_index = static_cast<int>(m_current_gui_element_ptr->shape);
	//m_selected_gui_element_state_index = m_selected_gui_element_state_index;
	
	if (m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_type == SquareBox::TextureEnum::SINGLE) {
		//Singles
		m_selected_single_texture_index = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_index;
	}
	else {
		//sheets
		m_selected_tile_sheet_texture_index = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index].texture_index;
	}

	m_selected_font_index = m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index].font_index;
}

void GUIEditor::adaptAllStatesToAUniformState()
{

	if (m_current_gui_element_ptr) {

		for (auto i = 0; i < m_current_gui_element_ptr->textures.size(); i++)
		{

			if (i!= m_selected_gui_element_state_index) {
				// update the texture
				m_current_gui_element_ptr->textures[i] = m_current_gui_element_ptr->textures[m_selected_gui_element_state_index];

				//update the font details
				m_current_gui_element_ptr->fonts[i] = m_current_gui_element_ptr->fonts[m_selected_gui_element_state_index];
			}
		}

	}
}

void GUIEditor::guiElementShapeShellSetter(const SquareBox::GWOM::GUIElement& gui_element_)
{
	m_current_gui_element_shape_shell_ptr = nullptr;
	for each (const auto & gui_shape in m_gui_element_shapes_ptr_vec)
	{
		if (gui_shape->shape == gui_element_.shape) {
			m_current_gui_element_shape_shell_ptr = gui_shape;
		}
	}
}

void GUIEditor::setCurrentGuiElementPtrLocationRatio()
{
	glm::vec2& mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
	m_current_gui_element_ptr->location_ratio = ((mouse_in_world - glm::vec2(m_visiable_dest_rect.x, m_visiable_dest_rect.y)) / glm::vec2(m_visiable_dest_rect.z, m_visiable_dest_rect.w));
}

void GUIEditor::cleanOutGUILayer()
{
	for (unsigned int i = 0; i < m_gui_layer.sprite_fonts.size(); i++)
	{
		m_gui_layer.sprite_fonts[i].dispose();
	}
	m_selected_single_texture_index = 0;
	m_selected_tile_sheet_texture_index	 = 0;
	m_gui_layer.gui_elements.clear();
	m_gui_layer.single_textures.clear();
	m_gui_layer.tiled_textures.clear();
	m_gui_layer.sprite_fonts.clear();

	m_current_gui_element_ptr = nullptr;
	m_current_gui_element_shape_shell_ptr = nullptr;
}

void GUIEditor::createNewGUILayer()
{
	cleanOutGUILayer();
	m_gui_layer.gui_elements.emplace_back();
	if (m_gui_editor_mode == GUIEditorModeEnum::PLACE) {
		m_current_gui_element_ptr = &m_gui_layer.gui_elements.back();
		updateState();
	}
}

GUIEditor::~GUIEditor()
{
}

