#include "ExampleHomeScreen.h"

/*
Order of Methods Calling
	Constructor
	Build
	OnEntry

	//game loop
	 (while ScreenState::RUNNING){
		 Update(float deltaTime_)
		 Draw
	   }

	OnExit
	Destory
	Deconstructor

*/


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

ExampleHomeScreen::ExampleHomeScreen(SquareBox::RenderEngine::Window* window) :m_window(window)
{
}

int ExampleHomeScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

int ExampleHomeScreen::getPreviousScreenIndex() const
{
	return m_previousScreenIndex;
}

void ExampleHomeScreen::build()
{
	//Called when the Screen is Added to the Screens list
}

void ExampleHomeScreen::onEntry()
{
	// this is where  stuff  the screen will use are  initialized from
	m_spriteBatch.init();
	m_spriteFont.initWithName("Comfortaa-Bold.ttf", 32);
	//Init Cameras
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(1.0f);
	//m_camera.setScale(162.0f);//this is because we want to project meters in game play

	//Init Shaders
	//Initialize our texture program
	m_textureProgram.init();
	m_textureProgram.compileDefaultTextureShaders();
	m_textureProgram.addDefaultTextureAttributes();
	m_textureProgram.linkShaders();



	/* or we can use the engine can mannually specific the shader settings that we would like to use 
			m_textureProgram.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
			m_textureProgram.addAttribute("vertexPosition");
			m_textureProgram.addAttribute("vertexColor");
			m_textureProgram.addAttribute("vertexUV");
			m_textureProgram.linkShaders();
	*/


	//this is where the screens default paramiters are set
	m_camera.setScale(1.0f);//The Zoom of the Camera
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth()* 0.5f, m_window->getScreenHeight() * 0.5f));//Center the camera

	m_vec_examples_pointer.push_back(new SquareBoxExample::ParticleSystemExample());
	m_vec_examples_pointer.push_back(new SquareBoxExample::AudioSystemExample());

	initGUI();
	m_assistant.init();
}

void ExampleHomeScreen::initGUI()
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

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_window->getWindowPointerForImGui(), m_window->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_INFO("Failed to load font Roboto-Medium.ttf ");
	}
	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/chintzy.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_INFO("Failed to load font chintzy.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void ExampleHomeScreen::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMainMenu();
	if (m_showAbout) {
		ImGui::OpenPopup("About?");
	}
	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	
	physics_tab_window_flags |= ImGuiWindowFlags_NoTitleBar;
	bool* physics_tab_open = false;

	float settings_tab_width;
	if(ImGui::Begin("Settings Tab", physics_tab_open, physics_tab_window_flags)){

	if (m_show_console) {
		physics_tab_window_flags |= ImGuiWindowFlags_NoResize;
		settings_tab_width = m_window->getScreenWidth() * 0.35f;
	}
	else {
		physics_tab_window_flags |= ImGuiWindowFlags_NoResize;

		settings_tab_width = static_cast<float>(ImGui::GetWindowWidth());
	}


	ImGui::SetWindowPos(ImVec2(m_window->getScreenWidth() - ImGui::GetWindowWidth() - 2, 20));


	ImGui::SetWindowSize("Settings Tab", ImVec2(settings_tab_width, m_window->getScreenHeight()));
	{
		ImGui::Text("Examples");
		//Object Types

		const char** example_labels = new const char* [m_vec_examples_pointer.size()];

		for (unsigned int i = 0; i < m_vec_examples_pointer.size(); i++)
		{
			example_labels[i] = m_vec_examples_pointer[i]->exampleTitle.c_str();
		}
		//display our shapes names
		ImGui::Combo("Example ", &m_selectedExampleIndex, example_labels, m_vec_examples_pointer.size());
		delete example_labels;
		setActiveExample(m_selectedExampleIndex);
		m_active_example->imGuiControls(&m_console);
	}

	};
	ImGui::End();

	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("This is an Application that shows how different SquareBox Engine Packages can be implemented.\nThe source is provided so you can look at the implmentations of each of the examples ");
		ImGui::Separator();

		//static int unused_i = 0;
		//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();

			m_showAbout = false;
		}

		ImGui::EndPopup();
	}

	//file explore dialog - end
	if (m_show_console) {
		auto console_position = ImVec2(0.0f, m_game_ptr->getWindow()->getScreenHeight() * 0.75f);
		auto console_size = ImVec2(static_cast<float>(m_game_ptr->getWindow()->getScreenWidth()*0.65f), m_game_ptr->getWindow()->getScreenHeight() * 0.25f);
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
	ImGui::EndFrame();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ExampleHomeScreen::showMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Show Console", NULL, m_show_console)) {
				m_show_console = !m_show_console;
			}

			if (ImGui::MenuItem("Clear logos", NULL, m_autoClearLogs)) {
				m_autoClearLogs = !m_autoClearLogs;
			}

			if (ImGui::MenuItem("About", NULL)) { m_showAbout = true; }
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ExampleHomeScreen::update(const float& deltaTime_)
{
	ImGuiIO& io = ImGui::GetIO();
	SDL_Event evnt;
	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
	{
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

	/*
	 these operations are only carried out when we are processing input ,(The mouse is in our world space)
	 please take note of that
	 If an operation does not depend on us processing input then is should join its similar friends below this if
	 //statement
	*/
	if (m_game_ptr->isProcessingInput()) {
		//zooming and moving the camera in our world
		m_assistant.cameraControls(m_camera, m_game_ptr);
	}
	//Called once every game loop , and updates what will be drawn
	m_window->update();
	m_camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	if (m_active_example != nullptr && m_active_example->isExampleInitialised) {
		
			// for a PC screen we always have mouse coordinates
			m_active_example->onUpdate(deltaTime_,m_game_ptr,m_camera);
		
	}
}

void ExampleHomeScreen::setActiveExample(int index)
{
	//first example being loaded
	if (m_active_example == nullptr) {
		if (index < m_vec_examples_pointer.size()) {
			m_active_example = m_vec_examples_pointer[index];
			m_active_example->onEntry(m_window);
			m_active_example->isExampleInitialised = true;
			if (m_autoClearLogs) {
				m_console.ClearLog();
			}
		}
	}
	else if (index < m_vec_examples_pointer.size()) {
			if (m_active_example != m_vec_examples_pointer[index]) {
				m_active_example->onExit();
				m_active_example->isExampleInitialised = false;

				m_active_example = m_vec_examples_pointer[index];
				m_active_example->onEntry(m_window);
				m_active_example->isExampleInitialised = true;

				if (m_autoClearLogs) {
					m_console.ClearLog();
				}
			}
		
	}
	
}

void ExampleHomeScreen::draw()
{
	// fill up the sprite batch
	m_spriteBatch.begin();
	//m_sprite_font or m_sprite_batch draw
	if (m_active_example != nullptr && m_active_example->isExampleInitialised) {
		m_active_example->onDraw(&m_spriteBatch, &m_spriteFont, nullptr);
	}
	m_spriteBatch.end();


	//Draw call
	m_textureProgram.use();
	IGameScreen::preUpdateShader(&m_textureProgram, "mySampler");
	//maintaining our backGround Color
	IGameScreen::uploadCameraInfo(&m_textureProgram, &m_camera, "P");
	m_spriteBatch.renderBatch();
	m_textureProgram.unuse();
	drawGUI();
}

void ExampleHomeScreen::onExit()
{
	//this is where objects that the screen used are destoryed from
	m_spriteBatch.dispose();
	m_spriteFont.dispose();
	m_textureProgram.dispose();
	m_assistant.dispose();
	if (m_active_example != nullptr) {
		m_active_example->onExit();
	}
}

void ExampleHomeScreen::destroy()
{
	//Called when the Screen is removed from the screenList
}

ExampleHomeScreen::~ExampleHomeScreen()
{
}