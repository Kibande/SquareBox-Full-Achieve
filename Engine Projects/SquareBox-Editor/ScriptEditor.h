#pragma once

#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>
#include "CustomConsole.h"
#include "ScreenIndices.h"
#include <ImGui/TextEditor.h>

class ScriptEditor :public SquareBox::IGameScreen
{
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

	// GUI
	void initGUI();
	void disposeGUI();
	void showMainMenu();
	void drawGUI();
	void loadFileToEdit(std::string file_path_);


	bool m_allow_exit = true;//bool to see if we can exit the leveleditor
	bool m_request_exit = false;
	bool m_show_about_dialog = false;

	bool m_show_console = false;

	SquareBoxEditor::CustomConsole m_console;

	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs

	TextEditor editor;

	bool m_show_open_file_dialog = false;
	bool m_show_save_file_dialog = false;
	bool m_show_create_new_file_dialog = false;

	float m_main_menu_bar_height = 0.0f;

	bool m_is_all_work_saved = false;

	bool m_show_failed_to_save_file_dialog = false;

	bool m_show_failed_to_load_file= false;


	std::string m_level_file_extension = ".*,.lua,.";

	std::string m_current_open_file_path = "unsaved";
	std::string m_current_open_file_name = "unsaved";

	SquareBox::GWOM::LevelReaderWriter m_level_reader_writer;
};

