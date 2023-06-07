#include "ScriptEditor.h"


std::vector<std::string> m_fresh_script_editor_logs_vec;
int script_editor_last_line_count_index = 0;

void ScriptEditor::initGUI()
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

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	editor.SetColorizerEnable(false);
}

void ScriptEditor::disposeGUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ScriptEditor::showMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		m_main_menu_bar_height = ImGui::GetWindowHeight();

		if (ImGui::BeginMenu("Project")) {
			if (ImGui::MenuItem("About", NULL)) { m_show_about_dialog = true; }
			if (ImGui::MenuItem("Show Console", NULL, m_show_console)) {
				m_show_console = !m_show_console;
			}

			if (ImGui::MenuItem("Home", NULL)) { setScreenState(SquareBox::ScreenState::CHANGE_PREVIOUS); }

			if (ImGui::MenuItem("Quit", "Alt-F4")) {

				m_game_ptr->initiateExit();
			}
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Open", "Ctrl-O"))
			{
				m_show_open_file_dialog = true;
			}

			if (ImGui::MenuItem("Save", "Ctrl-S"))
			{
				m_show_save_file_dialog = true;
			}
			if (ImGui::MenuItem("New")) { 
				m_show_create_new_file_dialog = true;
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro)) {
				editor.SetReadOnly(ro);
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ScriptEditor::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMainMenu();

	ImGuiWindowFlags editor_window_flags = 0;
	editor_window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
	editor_window_flags |= ImGuiWindowFlags_NoCollapse;
	editor_window_flags |= ImGuiWindowFlags_NoTitleBar;

	//editor_window_flags |= ImGuiWindowFlags_NoMove;
	if (m_show_console) {
		editor_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}
	auto cpos = editor.GetCursorPosition();
	if (ImGui::Begin("Text Editor ", nullptr, editor_window_flags)) {
		
		ImGui::SetWindowPos(ImVec2(0, m_main_menu_bar_height));
		if (m_show_console) {
			auto properties_tab_size = ImVec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight() * 0.75);
			ImGui::SetWindowSize(properties_tab_size);
		}
		else {
			auto properties_tab_size = ImVec2(m_game_ptr->getWindow()->getScreenWidth(),m_game_ptr->getWindow()->getScreenHeight());
			ImGui::SetWindowSize(properties_tab_size);
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
			editor.IsOverwrite() ? "Ovr" : "Ins",
			editor.CanUndo() ? "*" : " ",
			/*editor.GetLanguageDefinition().mName.c_str()*/"", m_current_open_file_name);
		editor.Render("TextEditor");
	}

	ImGui::End();


	//dialogs
	if (m_request_exit) {
		ImGui::OpenPopup("CloseScriptEditor");
	}


	if (m_show_about_dialog) {
		ImGui::OpenPopup("About");
	}

	if (m_show_create_new_file_dialog) {
		ImGui::OpenPopup("CreateNewFile");
	}


	//Close Editor Tab
	// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("CloseScriptEditor", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

	if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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


	if (ImGui::BeginPopupModal("FailedToSaveFile", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Failed to save file!\nCheck Logs for more information");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_show_failed_to_save_file_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("CreateNewFile", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{

		if (!m_is_all_work_saved) {
			ImGui::Text("You have unsaved work.\nDo you want to over write it?");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_current_open_file_name = "unsaved";
				m_current_open_file_path = "unsaved";
				m_show_create_new_file_dialog = false;
				editor.SetText("");
				ImGui::CloseCurrentPopup();
			}
		}
		else {
			ImGui::Text("This will create a new file\nDo you want to continue?");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_current_open_file_name = "unsaved";
				m_current_open_file_path = "unsaved";
				m_show_create_new_file_dialog = false;
				editor.SetText("");
				ImGui::CloseCurrentPopup();
			}
		}
	
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			m_show_create_new_file_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (m_show_open_file_dialog) {
		ImGui::OpenPopup("Open File");
	}
	if (m_show_save_file_dialog) {
		ImGui::OpenPopup("Save File");
	}


	/* Optional third parameter. Support opening only compressed rar/zip files.
 * Opening any other file will show error, return false and won't close the dialog.
 */
	if (m_file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_level_file_extension))
	{
		SBX_INFO("Starting Data Loading.");
		SBX_INFO("Loading {} .", m_file_dialog.selected_fn);
		
		loadFileToEdit(m_file_dialog.selected_path);

		m_is_all_work_saved = false;
	}


	if (m_file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_level_file_extension))
	{
		SBX_INFO("Saving File as {} File", m_level_file_extension);
		auto textToSave = editor.GetText();

		std::string ext = m_file_dialog.ext;
		std::string filePath = m_file_dialog.selected_path;

		while (filePath != ext &&
			filePath.size() > ext.size() &&
			filePath.substr(filePath.size() - ext.size()) == ext)
		{
			// if so then strip them off
			filePath = filePath.substr(0, filePath.size() - ext.size());
		}
		/// save text....
		SquareBox::AssetManager::IOManager::saveRawDataToFile(textToSave.c_str(), textToSave.length(), filePath + m_file_dialog.ext);
	}

	//reset of all dialogs
	m_show_open_file_dialog = false;
	m_show_save_file_dialog = false;


	//file explore dialog - end
	if (m_show_console) {
		auto console_position = ImVec2(0.0f, m_game_ptr->getWindow()->getScreenHeight() * 0.75f);
		auto console_size = ImVec2(static_cast<float>(m_game_ptr->getWindow()->getScreenWidth()), m_game_ptr->getWindow()->getScreenHeight() * 0.25f);
		ImGuiWindowFlags console_flags = 0;
		console_flags |= ImGuiWindowFlags_NoTitleBar;
		m_console.Draw("Console", &m_show_console, console_flags, console_position, console_size);

		for each (auto log_message in m_fresh_script_editor_logs_vec)
		{
			m_console.AddLog(log_message.c_str());
		}
		m_fresh_script_editor_logs_vec.clear();
	}
	else {
		if (m_fresh_script_editor_logs_vec.size() > 0) {
			m_fresh_script_editor_logs_vec.clear();
		}
	}


	ImGui::EndFrame();// the example code did not have this.
	// Rendering
	ImGui::Render();
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

void ScriptEditor::loadFileToEdit(std::string file_path_)
{
	std::ifstream t(file_path_);
	if (t.good())
	{

		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		editor.SetText(str);
		m_current_open_file_path = file_path_;
	}
	else {
		m_show_failed_to_load_file = true;
	}

}

int ScriptEditor::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int ScriptEditor::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void ScriptEditor::build()
{
}

void ScriptEditor::onEntry()
{
	initGUI();
}

void ScriptEditor::update(const float& deltaTime_)
{
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
}

void ScriptEditor::draw()
{
	drawGUI();

}

void ScriptEditor::onExit()
{
	if (!m_allow_exit)
	{
		m_game_ptr->abortExit();//abort normal exit and first see request for permission to exit
		m_request_exit = true;//this will lead to the are u sure dialog being displayed
	}
	else
	{
		disposeGUI();
	}
}

void ScriptEditor::destroy()
{
}
