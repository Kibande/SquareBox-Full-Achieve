#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>

#include "ScreenIndices.h"
#include "Editor_Assistant.h"

enum class AnimationEditorThemeEnum {
	ClassicTheme = 0,
	LightTheme = 1,
	DarkTheme = 2 //default theme
};

enum class AnimationEditorModeEnum {
	PLACE = 0,
	SELECT = 1
};



class AnimationEditor: public SquareBox::IGameScreen
{
public:
	AnimationEditor();
	~AnimationEditor();

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

	void disposeGUI();

	void drawGUI();

	void showMenuMain();

	void updateState();//updates the state that the gui is in

	void animationElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_);

	void cleanOutAnimationLayer();

	void createNewAnimationLayer();

	SquareBox::Utilities m_utilities;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	SquareBox::Camera::ParallelCamera m_camera;
	bool m_show_about_dialog = false;
	int m_next_screen_index = SCREEN_INDEX_NO_SCREEN;
	int m_previous_screen_index = WELCOME_SCREEN_INDEX;
	SquareBoxEditor::Editor_Assistant m_editor_assistant;
	SquareBox::AnimationSystem::AnimationCreator m_animation_creator;
	std::vector<SquareBox::GWOM::WorldCluster> m_world_cluster_vec;
	std::vector<std::pair<int, int >> m_alive_cluster_objects_;
	bool m_debug_mode = false;

	AnimationEditorModeEnum m_animation_editor_mode = AnimationEditorModeEnum::PLACE;

	float m_main_menu_bar_height = 0.0f;
	float m_controls_panel_width = 0.0f;

	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs

	bool m_show_open_animation_script_file_dialog = false;
	bool m_show_save_animation_script_file_dialog = false;

	bool m_show_open_tile_sheet_texture_file_dialog = false;

	std::string m_animation_script_extensions = ".dat,.sbx,.lua";
	std::string m_texture_file_extensions = ".png,.jpg";
	const char** m_shapes_labels_ptr = nullptr;


	//variables that need updating in select mode
	SquareBox::GWOM::ClusterObject* m_current_cluster_object_ptr = nullptr;

	glm::ivec2 m_tiling = { 0.0f,0.0f };

};

