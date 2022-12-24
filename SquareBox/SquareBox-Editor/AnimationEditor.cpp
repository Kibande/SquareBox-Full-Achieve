#include "AnimationEditor.h"

AnimationEditor::AnimationEditor()
{
}

AnimationEditor::~AnimationEditor()
{
}

int AnimationEditor::getNextScreenIndex() const
{
    return 0;
}

int AnimationEditor::getPreviousScreenIndex() const
{
    return 0;
}

void AnimationEditor::build()
{
}

void AnimationEditor::onEntry()
{
	//Init Shaders
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();
	//the debugProgram
	m_debug_program.compileDefaultDebugShaders();
	m_debug_program.addDefaultDebugAttributes();
	m_debug_program.linkShaders();

	m_debug_renderer.init();

	//Init Cameras
	glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions.x, screen_dimensions.y);
	m_camera.setScale(1.0f);//The Zoom of the Camera
	m_camera.setPosition(screen_dimensions * glm::vec2(0.5f));

	m_utilities.init();

	initGUI();

	//window icon
	auto icon_pixel_data = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/nodpi/bricks_light_top.png");
	m_game_ptr->getWindow()->setWindowIcon(icon_pixel_data.pixels, icon_pixel_data.width, icon_pixel_data.height);
	SquareBox::AssetManager::IOManager::freePixelData(icon_pixel_data);
}

void AnimationEditor::update(const float& deltaTime_)
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
			/* this is the hack that i have come up with to solve the issue of imgui freezing up
			and not allowing any more user input, this block of code  forces it to start
			taking user input but it disoragnises sdls input manager
			so this function only helps you be able to save your work */
		}
	}

	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	//Called once every game loop , and updates what will be drawn
	m_camera.update(screen_dimensions.x, screen_dimensions.y);


	if (m_game_ptr->isProcessingInput()) {
		if (m_animation_editor_mode == AnimationEditorModeEnum::PLACE) {

		}
		else if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {

		}
	}
}

void AnimationEditor::draw()
{
	m_sprite_batch.begin(SquareBox::RenderEngine::NONE);

	// Render textures

	m_sprite_batch.end();
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	//debug draw

	m_debug_renderer.begin();
	if (m_debug_mode) {
	
	}

	m_debug_renderer.end();
	m_debug_program.use();
	uploadCameraInfo(&m_debug_program, &m_camera, "P");
	m_debug_renderer.render();
	m_debug_program.unuse();

	drawGUI();//this is where is_locked changes from
	//having this here avoids Imgui end frame crashes
}

void AnimationEditor::onExit()
{
	//this is where all screen process are stopped
//this is where objects that the screen used are destroyed from
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();

	m_texture_program.dispose();
	m_utilities.dispose();
	m_debug_program.dispose();

	disposeGUI();

	//need a better fix for a clean up here
	m_texture_program = SquareBox::RenderEngine::GLSLProgram();
	m_debug_program = SquareBox::RenderEngine::GLSLProgram();
}

void AnimationEditor::destroy()
{
}

void AnimationEditor::initGUI()
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

void AnimationEditor::disposeGUI() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void AnimationEditor::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMenuMain();
	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	bool* physics_tab_open = false;
	ImGui::Begin("Control Panel", physics_tab_open, physics_tab_window_flags);

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

		m_animation_editor_mode = static_cast<AnimationEditorModeEnum>(mode);

		if (beforeMode != mode) {
			if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {

			}
			else {

			}
		}
		ImGui::Checkbox("Debug Mode", &m_debug_mode);
	
		if (m_show_open_animation_script_file_dialog) {
			ImGui::OpenPopup("Open Animation Script File");
		}

		if (m_show_about_dialog) {
			ImGui::OpenPopup("About?");
		}

		if (m_show_save_animation_script_file_dialog) {
			ImGui::OpenPopup("Save Animation Script File");
		}


		if (m_show_open_tile_sheet_texture_file_dialog) {
			ImGui::OpenPopup("Open Tile Sheet Texture File");
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

	
		if (m_file_dialog.showFileDialog("Open Tile Sheet Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			auto temp_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(temp_texture.id, m_tiling);// tiling can always be overwitten

			
		}

	

		if (m_file_dialog.showFileDialog("Save Animation Script File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_animation_script_extensions))
		{
			SBX_INFO("Saving Animation Script as {} File", m_animation_script_extensions);

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

			
		}

		if (m_file_dialog.showFileDialog("Open Animation Script File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_animation_script_extensions))
		{
		}
		//reset of all dialogs
		m_show_open_animation_script_file_dialog = false;
		m_show_save_animation_script_file_dialog = false;
		m_show_open_tile_sheet_texture_file_dialog = false;
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

void AnimationEditor::showMenuMain()
{
	ImGui::BeginMainMenuBar();
	m_main_menu_bar_height = ImGui::GetWindowHeight();
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("About", NULL)) {
			m_show_about_dialog = true;
		}
		if (ImGui::MenuItem("Open", "CTRL+O")) {
			m_show_open_animation_script_file_dialog = true;
		}
		if (ImGui::MenuItem("Save", "CTRL+S")) {
			m_show_save_animation_script_file_dialog = true;
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

void AnimationEditor::updateState()
{
}

void AnimationEditor::animationElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_)
{
}

void AnimationEditor::cleanOutAnimationLayer()
{
}

void AnimationEditor::createNewAnimationLayer()
{
}

