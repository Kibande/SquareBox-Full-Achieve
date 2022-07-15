#pragma once
#include "ScreenIndices.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <SquareBox-Editor/Editor_Assistant.h>
#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_sdl.h>
#include<ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>
class Second_Screen : public SquareBox::IGameScreen
{
public:
	Second_Screen();
	~Second_Screen();
private:
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void onEntry() override;

	virtual void update(const float & deltaTime_) override;

	virtual void draw() override;

	virtual void onExit() override;

	virtual void destroy() override;

	void initGUI();

	void drawGUI();

	void showMenuMain();

	void loadTextureObject(std::string file_path_,std::string file_name_, glm::vec2 tiling_);
	SquareBox::IShape * m_current_shape_ptr;
	SquareBox::Utilities m_utilities;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	SquareBox::Camera::ParallelCamera m_camera;
	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
	float m_tiling_numbers_scale = 1.0f;
	float m_camera_scale = 1.0f;
	float m_grid_color_array[4];
	float m_window_back_ground_color_array[4];
	float m_tiling_number_color_array[4];
	bool m_show_open_tile_sheet_file_dialog = false;
	bool m_show_about_dialog = false;
	glm::vec2 m_camera_position;
	int m_next_screen_index = SCREEN_INDEX_NO_SCREEN;
	int m_previous_screen_index = SCREEN_INDEX_NO_SCREEN;
	bool m_show_grid_lines = true;
	float m_screen_texture_height_ratio =0.8f; //got through trial and error on my pc

	std::vector<SquareBox::GWOM::ClusterObject> m_vec_of_texture_objects;
	//storing the loaded textures as objects since cluster objects have alot of inbuilt
	// parameters that make it easy for us to store information for retriving
	int m_active_texture_object_index = 0;
	int m_target_tile_index = 0;

	std::vector<std::string> m_recent_open_texture_paths;
	glm::ivec2 m_current_texture_object_tiling;
	glm::ivec2 m_new_texture_tiling;

	//new_texture_object.pod_bool
	/*
		if a texture qualifies as a square grid, we are using camera curling
		and utilizing the tile system to do the tiles
	*/
	SquareBox::GWOM::TileSystem m_tile_system;

	SquareBoxEditor::Editor_Assistant m_editor_assistant;
};
