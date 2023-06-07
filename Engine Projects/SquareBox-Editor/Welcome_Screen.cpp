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
	initGUI();
}

void Welcome_Screen::onExit()
{	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
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
}

void Welcome_Screen::draw()
{
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
		ImGuiWindowFlags imgui_window_flags = 0;
		imgui_window_flags |= ImGuiWindowFlags_NoMove;
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Route Panel", NULL, imgui_window_flags);

		// Buttons
		if (ImGui::Button("Level Editor")) {
			m_nextScreenIndex = LEVEL_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("GUI Editor")) {
			m_nextScreenIndex = GUI_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("Script Editor")) {
			m_nextScreenIndex = SCRIPT_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		}

		if (ImGui::Button("Game Play")) {
		/*	
			m_nextScreenIndex = SCRIPT_EDITOR_SCREEN_INDEX;
			m_current_state = SquareBox::ScreenState::CHANGE_NEXT;
		*/
		}
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}