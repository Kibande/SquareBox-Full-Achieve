#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>

#include "ScreenIndices.h"
#include "Editor_Assistant.h"

enum class GUIEditorThemeEnum {
	ClassicTheme = 0,
	LightTheme = 1,
	DarkTheme = 2 //default theme
};

enum class GUIEditorModeEnum {
	PLACE = 0,
	SELECT = 1
};

enum class GUIEditorOrientationEnum {
	LANDSCAPE = 0,
	PORTRIAT = 1
};

class GUIEditor : public SquareBox::IGameScreen
{
public:
	GUIEditor();
	~GUIEditor();
private:
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void onEntry() override;

	virtual void update(const float& deltaTime_) override;

	virtual void draw() override;

	virtual void onExit() override;

	virtual void destroy() override;

	void initGUI();

	void drawGUI();

	void showMenuMain();

	void updateState();//updates the state that the gui is in

	void guiElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_);

	void setCurrentGuiElementPtrLocationRatio();

	SquareBox::Utilities m_utilities;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	SquareBox::Camera::ParallelCamera m_camera;
	bool m_show_about_dialog = false;
	int m_next_screen_index = SCREEN_INDEX_NO_SCREEN;
	int m_previous_screen_index = SCREEN_INDEX_NO_SCREEN;
	SquareBoxEditor::Editor_Assistant m_editor_assistant;
	SquareBox::GWOM::GuiLayer m_gui_layer;

	std::vector<SquareBox::GUI::IGUIElement*> m_gui_element_shapes_ptr_vec;
	bool m_debug_mode = false;
	GUIEditorModeEnum m_gui_editor_mode = GUIEditorModeEnum::PLACE;
	GUIEditorOrientationEnum m_orientation = GUIEditorOrientationEnum::LANDSCAPE;
	glm::vec4 m_visiable_dest_rect;
	float m_main_menu_bar_height = 0.0f;
	float m_controls_panel_width = 0.0f;
	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
	SquareBox::GWOM::GuiReaderWriter m_gui_layer_reader_writer;
	bool m_show_open_gui_layer_file_dialog = false;
	bool m_show_save_gui_layer_file_dialog = false;
	bool m_show_open_single_texture_file_dialog = false;
	bool m_show_open_tile_sheet_texture_file_dialog = false;
	bool m_show_open_font_file_dialog = false;

	std::string m_gui_layer_extensions = ".dat,.sbx";
	std::string m_texture_file_extensions = ".png,.jpg";
	std::string m_font_file_extensions = ".ttf";
	const char** m_shapes_labels_ptr = nullptr;
	const char** m_gui_element_states_labels_ptr = nullptr;


	//variables that need updating in select mode
	SquareBox::GWOM::GUIElement* m_current_gui_element_ptr = nullptr;
	SquareBox::GUI::IGUIElement* m_current_gui_element_shape_shell_ptr = nullptr;
	glm::ivec2 m_tiling = { 0.0f,0.0f };
	int m_selected_gui_element_shape = 0;
	int m_selected_gui_element_state = 0;
	int m_selected_single_texture_index = 0;
	int m_selected_tile_sheet_texture_index = 0;
	int m_selected_font_index = 0;
	int m_font_size = 32;
	int m_selected_texture_type = 0;
};
