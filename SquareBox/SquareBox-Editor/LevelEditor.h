#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>

#include "LayerTypeInterface.h"
#include "TiledLayer.h"
#include "FlatLayer.h"
#include "IsometricLayer.h"
#include "Editor_Assistant.h"
#include "CustomConsole.h"
#include "EditorEnums.h"
#include "ScreenIndices.h"

enum class RightTabDisplayEnum {
	OBJECTPROPERTIES = 0,
	LAYERING = 1
};

enum class EditorThemeEnum {
	ClassicTheme = 0,
	LightTheme = 1,
	DarkTheme = 2 //default theme
};
/*
	0-Physics , properities of the currently active object
	1-Layers , shows the layer arragements, visibility properties e.t.c
	2-Layer Properties for a sepecific layers , mainly the layers tiling
*/


class LevelEditor :public SquareBox::IGameScreen
{
public:
	LevelEditor(SquareBox::RenderEngine::Window* window_ptr_);

	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void onEntry() override;

	virtual void update(const float& deltaTime_) override;

	virtual void draw() override;

	virtual void onExit() override;

	virtual void destroy() override;

	void createNewLayer();
	void createNewProject();
	void cleanOutEditor();
	// GUI
	void initGUI();
	void disposeGUI();
	void showMainMenu();
	void drawGUI();

	~LevelEditor();

private:
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::RenderEngine::GLSLProgram m_light_program;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;

	SquareBox::AssetManager::GLTexture m_texture;

	SquareBox::RenderEngine::Window* m_window_ptr;

	std::vector<LayerTypeInterface*> m_layer_types_vec;

	SquareBox::GWOM::LevelReaderWriter m_level_reader_writer;

	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs

	int m_active_layer_index; //the layer on which we are doing object placements right now

	float m_universal_camera_scale = 162.0f;

	glm::vec2 m_universal_camera_position = { 0.0f,0.0f };

	int m_selected_single_texture_index = 0;//	the single texture index we are currently on

	int m_selected_tile_sheet_texture_index = 0;//	the tile Sheet texture index we are currently on

	RightTabDisplayEnum m_side_view = RightTabDisplayEnum::OBJECTPROPERTIES;

	EditorThemeEnum m_theme_index = EditorThemeEnum::DarkTheme;

	//Modes
	EditorModeEnum m_editor_mode_enum = EditorModeEnum::PLACE;

	//Imgui file handling dialogs
	bool m_show_open_level_file_dialog = false;
	bool m_show_open_texture_file_dialog = false;
	bool m_show_open_tile_sheet_file_dialog = false;
	bool m_show_save_level_file_dialog = false;

	glm::ivec2 m_new_tiled_texture_tiling{ 0,0 };

	bool m_debug_mode = false;

	bool m_respect_ancestor = false;
	bool m_allow_exit = true;//bool to see if we can exit the leveleditor
	bool m_request_exit = false;

	bool m_show_failed_to_save_world_dialog = false;
	bool m_show_create_new_project_dialog = false;
	bool m_show_about_dialog = false;
	bool m_show_key_bindings_dialog = false;

	bool m_show_all_debug_lines = true;//alternative is showing only for the active layer
	bool m_show_console = false;

	std::vector<SquareBox::GWOM::Layer> m_layers;

	std::string m_level_file_extension = ".dat,.sbx";
	std::string m_texture_file_extensions = ".png,.jpg";

	std::string m_file_path_to_track = "";

	//Assistant
	SquareBox::Utilities m_utilities;
	SquareBoxEditor::Editor_Assistant m_editor_assitant;
	SquareBoxEditor::CustomConsole m_console;
	const char** layer_labels_ptr = nullptr;
	std::vector<LayerTypeInterface*> m_vec_of_layer_types;
	int m_selected_layer_type = 0;
	bool m_is_all_work_saved = true;
};

