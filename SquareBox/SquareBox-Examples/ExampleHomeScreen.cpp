#include "ExampleHomeScreen.h"
#include "CustomAppConsole.h"
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

static CustomAppConsole console;

static void ShowCustomAppConsole(bool* p_open, int width, int height)
{
	console.Draw("Example: Console", p_open, width, height);
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


	m_vec_examples_pointer.push_back(new SquareBox::Example::ParticleSystemExample());
	m_vec_examples_pointer.push_back(new SquareBox::Example::AudioSystemExample());
	initGUI();
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
	physics_tab_window_flags |= ImGuiWindowFlags_NoResize;
	physics_tab_window_flags |= ImGuiWindowFlags_NoTitleBar;
	bool* physics_tab_open = false;
	ImGui::Begin("Settings Tab", physics_tab_open, physics_tab_window_flags);

	ImGui::SetWindowPos(ImVec2(m_window->getScreenWidth() - ImGui::GetWindowWidth() - 2, 20));
	ImGui::SetWindowSize("Settings Tab", ImVec2(m_window->getScreenWidth()*0.35, m_window->getScreenHeight()));
	{
		ImGui::Text("Examples");
		//Object Types

		const char** example_labels = new const char*[m_vec_examples_pointer.size()];

		for (unsigned int i = 0; i < m_vec_examples_pointer.size(); i++)
		{
			example_labels[i] = m_vec_examples_pointer[i]->exampleTitle.c_str();
		}
		//display our shapes names
		ImGui::Combo("Example ", &m_selectedExampleIndex, example_labels, m_vec_examples_pointer.size());
		delete example_labels;
		setActiveExample(m_selectedExampleIndex);
		m_active_example->imGuiControls(&console);
	}

	ImGui::End();

	if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("This is an Application that shows how different SquareBox Engine Packages can be implemented. The source is provided so you can look at the implmentations of each of the examples ");
		ImGui::Separator();

		//static int unused_i = 0;
		//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();

			m_showAbout = false;
		}

		ImGui::EndPopup();
	}

	static bool *demo;
	ShowCustomAppConsole(demo, m_window->getScreenWidth(), m_window->getScreenHeight());
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
		//Calculated movement Speed
		float cMS = (m_kContstantForScaleAndZoom / m_camera.getScale());

		//Calculated Zooming Speed
		float cZS = (m_kContstantForScaleAndZoom / m_cameraPaningSpeed);
		//Camera Controls
		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::ARROW_LEFT))
		{
			m_camera.setPosition(glm::vec2(m_camera.getPosition().x + cMS, m_camera.getPosition().y));
		}
		else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::ARROW_RIGHT)) {
			m_camera.setPosition(glm::vec2(m_camera.getPosition().x - cMS, m_camera.getPosition().y));
		}
		else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::ARROW_DOWN))
		{
			m_camera.setPosition(glm::vec2(m_camera.getPosition().x, m_camera.getPosition().y + cMS));
		}
		else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::ARROW_UP)) {
			m_camera.setPosition(glm::vec2(m_camera.getPosition().x, m_camera.getPosition().y - cMS));
		}
		else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEY_q)) {
			m_camera.setScale(m_camera.getScale() + cZS);
		}
		else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEY_e)) {
			m_camera.setScale(m_camera.getScale() - cZS);
		}
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
				console.ClearLog();
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
					console.ClearLog();
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