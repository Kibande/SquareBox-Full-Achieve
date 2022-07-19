#include "Welcome_Screen.h"
#include "ScreenIndices.h"
#include <ImGui/ImGuiFileBrowser.h>
Welcome_Screen::Welcome_Screen() {
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
	m_spriteFont.initWithName("Comfortaa-Bold", 32);

	//Init Shaders
	m_textureProgram.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	//Init Cameras
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() / 2, m_game_ptr->getWindow()->getScreenHeight() / 2));//Center the camera
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
	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
		std::cout << "Mouse Click \n";
	}
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_SHIFT) && m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::TAB)) {
		std::cout << "SHift and Tab are being pressed"<<std::endl;
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_1)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_1);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_2)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_2);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_3)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_3);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_4)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_4);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_6)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_6);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_7)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_7);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_8)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_8);
	}
	else if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_9)) {
		std::cout << ToString(SquareBox::KeyBoardEnum::KEYPAD_NUM_9);
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
		SBX_ERROR("Failed to load font Roboto-Medium.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}
void Welcome_Screen::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();

	{
		ImGui::Begin("Route Panel");

		// Buttons

		//if (ImGui::Button("Mobile App Editor")) {
		//	m_nextScreenIndex = MOBILE_APP_EDITOR_SCREEN_INDEX;
		//	m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		//}

		//if (ImGui::Button("Desktop App Editor")) {
		//	m_nextScreenIndex = DESKTOP_APP_EDITOR_SCREEN_INDEX;
		//	m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		//}

		if (ImGui::Button("Level Editor")) {
			m_nextScreenIndex = LEVEL_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("GUI Editor")) {
			m_nextScreenIndex = GUI_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}