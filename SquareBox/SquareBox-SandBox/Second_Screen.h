#pragma once
#include "ScreenIndices.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_sdl.h>
#include<ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>
class Second_Screen : public SquareBox::IGameScreen
{
public:
	Second_Screen(SquareBox::RenderEngine::Window* window);
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

	void cameraControls(SquareBox::Camera::ParallelCamera & camera_, SquareBox::IMainGame * gamePtr_);

	void showMenuMain();

	SquareBox::AssetManager::GLTexture loadTexture(std::string file_path_, glm::vec2 tiling_);
	SquareBox::IShape * m_current_shape_ptr;
	SquareBox::Utilities m_utilities;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::RenderEngine::Window* m_window;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	std::string m_texture_file_path= "Assets/Textures/nodpi/blue_grey.png";
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	SquareBox::Camera::ParallelCamera m_camera;
	SquareBox::GWOM::ClusterObject m_texture_holder_object;

	imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
	glm::ivec2 m_original_texture_dimensions;
	float m_tiling_numbers_scale = 1.0f;
	float m_camera_scale = 1.0f;
	float m_grid_color_array[4];
	float m_window_back_ground_color_array[4];
	float m_tiling_number_color_array[4];
	bool m_show_open_tile_sheet_file_dialog = false;
	bool m_show_about_dialog = false;
	std::vector<std::string> m_recent_open_texture_paths;
	glm::vec2 m_camera_position;
	float m_screen_texture_height_ratio =0.8f; //got through trial and error on my pc
	int m_target_tile_index = 0;
	glm::ivec2 m_new_texture_tiling;
	int m_next_screen_index = SECOND_SCREEN_INDEX;
	int m_previous_screen_index = SCREEN_INDEX_NO_SCREEN;
	bool m_show_grid_lines = true;//because drawing the debug lines is draining us alot , i suspect it it the debug renderer
	//Know errors
	/*
	The debug renderer is only drawing the debug lines that are close to white

	this feels like a very important but very deep dive requiring bug. 
	!!! God should help me :)' 
	
	*/
};
