#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>
#include "ScreenIndices.h"
#include "Editor_Assistant.h"

/*
	the comments in this file were lifted from FlatLayer.h
*/

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

	void animationElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_);

	void cleanOutAnimationLayer();

	void updateStates();  //make sure every objects state is respected and not accidentally changed

	SquareBox::Utilities m_utilities;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	bool m_show_about_dialog = false;
	int m_next_screen_index = SCREEN_INDEX_NO_SCREEN;
	int m_previous_screen_index = WELCOME_SCREEN_INDEX;
	SquareBoxEditor::Editor_Assistant m_editor_assistant;
	SquareBox::AnimationSystem::AnimationCreator m_animation_creator;
	std::vector<SquareBox::GWOM::Layer> m_layers;
	std::vector<std::pair<int, int>> m_selected_cluster_objects;
	std::vector<std::pair<int, std::pair<int, int>>> m_cluster_objects_to_delete; //[layer_index[world_cluster_index,cluster_object_index]]

	bool m_debug_mode = false;
	bool m_is_all_work_saved = false;
	AnimationEditorModeEnum m_animation_editor_mode = AnimationEditorModeEnum::PLACE;
	//cluster object drag select mode 
	int cluster_object_drag_selection_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);


	float m_main_menu_bar_height = 0.0f;
	float m_controls_panel_width = 0.0f;

	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs

	bool m_show_open_animation_script_file_dialog = false;
	bool m_show_save_animation_script_file_dialog = false;

	bool m_show_open_tile_sheet_texture_file_dialog = false;
	bool m_show_open_single_texture_file_dialog = false;

	std::string m_animation_script_extensions = ".dat,.sbx,.lua";
	std::string m_texture_file_extensions = ".png,.jpg";
	const char** m_shapes_labels_ptr = nullptr;

	//variables that need updating in select mode
	SquareBox::GWOM::ClusterObject* m_current_cluster_object_ptr = nullptr;

	glm::ivec2 m_tiling = { 0.0f,0.0f };


	int m_selected_shape = 0; //in reference to m_vec_shapes_pointer (vector of pointer of all shapes avaliable to us);

	int m_selected_body_type = 0;


	int m_selected_single_texture_index = 0;//	the single texture index we are currently on

	int m_selected_tile_sheet_texture_index = 0;//	the tile Sheet texture index we are currently on

	//in reference to the active layer
	//	current index of the world cluster we are hovering over
	int m_current_hovered_world_cluster_index = 0;
	//	current index of the cluster object we are hovering over
	int m_current_hovered_cluster_object_index = 0;

	bool m_enable_physics = true;

	std::vector<SquareBox::IShape*> m_vec_shapes_pointer;

	SquareBox::RenderEngine::ColorRGBA8 m_debug_static_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_debug_dynamic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_debug_kinetic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 0, 255, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_place_mode_sketch_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::antique_white);
	SquareBox::RenderEngine::ColorRGBA8 m_not_physics_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_unknown_body_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::violet);
	SquareBox::RenderEngine::ColorRGBA8 m_border_color;
	SquareBox::RenderEngine::ColorRGBA8 m_selected_body_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);

	/*poiners*/
	SquareBox::IShape* m_current_shape_ptr = nullptr;// a pointer to the current world shape

	const char** shapes_labels_ptr = nullptr;
	SquareBox::PhysicsCollisionEngine::PhysicsWorld m_physics_world;

	int cluster_object_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);

	SquareBoxEditor::Editor_Assistant m_editor_assitant;

	bool m_play_simulation = false;//controls physics and animations update loops

	bool m_show_world_locked_dialog = false;

	bool m_respect_ancestor = false;

	//deletion key
	int delete_selection_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEYBOARD_DELETE);

	glm::ivec2 m_new_tiled_texture_tiling;

	SquareBox::AssetManager::GLTexture m_texture;

};

