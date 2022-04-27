#include "Welcome_Screen.h"
#include "ScreenIndices.h"
#include <ImGui/ImGuiFileBrowser.h>
Welcome_Screen::Welcome_Screen(SquareBox::RenderEngine::Window* window) :m_window(window)
{
}

Welcome_Screen::~Welcome_Screen()
{
}

int Welcome_Screen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

int Welcome_Screen::getPreviousScreenIndex() const
{
	return m_previousScreenIndex;
}

void Welcome_Screen::build()
{
}

void Welcome_Screen::destroy()
{
}

void Welcome_Screen::onEntry()
{
	m_spriteBatch.init();
	m_spriteFont.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);

	//Init Shaders
	m_textureProgram.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	//Init Cameras
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2));//Center the camera
	// SquareBox::AudioSystem::Music music= m_game->getAudioEngine().loadMusic("Assets/Audio/Stranded.mp3");
	// music.play(0);
	initGUI();
}

void Welcome_Screen::onExit()
{	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	m_spriteBatch.dispose();
	m_spriteFont.dispose();
	m_textureProgram.dispose();
}

void Welcome_Screen::update(const float & deltaTime_)
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
	m_window->update();
	m_camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
		std::cout << "Mouse Click \n";
	}
}

void Welcome_Screen::draw()
{
	m_textureProgram.use();

	IGameScreen::preUpdateShader(&m_textureProgram, "mySampler");

	IGameScreen::uploadCameraInfo(&m_textureProgram, &m_camera, "P");

	m_spriteBatch.begin();

	//m_sprite_font or m_sprite_batch draw
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_textureProgram.unuse();
	drawGUI();
}

void Welcome_Screen::initGUI()
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
	ImGui_ImplSDL2_InitForOpenGL(m_window->getWindowPointerForImGui(), m_window->getGLContextPointerForImGui());
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
void Welcome_Screen::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window->getWindowPointerForImGui());
	ImGui::NewFrame();



	{
		ImGui::Begin("");

		// Buttons

		if (ImGui::Button("Mobile App Editor")) {
			m_nextScreenIndex = MOBILE_APP_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("Desktop App Editor")) {
			m_nextScreenIndex = DESKTOP_APP_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("Level Editor")) {
			m_nextScreenIndex = LEVEL_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		//file dialog
		//showMainMenu();
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}