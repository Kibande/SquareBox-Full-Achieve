#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "ScreenIndicies.h"
#include "Gamelogic.h"

enum class GameState {
	PLAYING,
	PAUSED,
	LOADING,
	GAMEOVER
};
class GamePlayScreen : public SquareBox::IGameScreen
{
public:
	GamePlayScreen() {};
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float & deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;
	void drawLayer(SquareBox::RenderEngine::SpriteBatch& sprite_batch_, SquareBox::Camera::ParallelCamera& camera_, SquareBox::RenderEngine::GLSLProgram& texture_program_, SquareBox::GWOM::Layer& layer_);
    //The HUDS and GUI camera
	SquareBox::Camera::ParallelCamera m_hud_gui_camera;

	//Game Play Camera
	SquareBox::Camera::ParallelCamera m_game_play_camera;
	//our programs
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	SquareBox::RenderEngine::GLSLProgram m_debug_program;

	//Sprite batch
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;

	//Debug Renderer
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;

	//layers
	std::vector<SquareBox::GWOM::Layer> m_layers;

	//Graphics
	SquareBox::GUIWrapper m_game_mobile_controls_gui;
	SquareBox::GUIWrapper m_game_pause_huds_and_gui;
	SquareBox::GUIWrapper m_game_over_huds_and_gui;
	SquareBox::GUIWrapper m_game_stats_huds_and_gui;

	std::map<int, int> m_game_play_gui_elements_coordinates;
	std::map<int, int> m_game_pause_gui_elements_coordinates;
	std::map<int, int> m_game_over_gui_elements_coordinates;
	std::map<int, int> m_game_stats_gui_elements_coordinates;

	//logic
	GameLogic m_game_logic;

	//The grids
	SquareBox::GWOM::Grid  m_agents_collision_grid;
	SquareBox::Utilities m_utilities;

	SquareBox::IShape * m_current_shape_ptr = nullptr;

	//Player Coordinates
	std::pair<int, std::pair<int, int>> m_player_coordinates; 
	/*
		this is the players location in the layers system

	*/
	bool m_object_show_coordinates = true;
	bool m_show_grid = true;
	

	/* 
		Known game issues , there is a lag when crossing from one cluster object to another
		and there is also a replusion like force that tries to prevent them from crossing over cell
		boundaries
	
	
	*/

	//game stats
	int m_humans_text_gui_id = 60;
	int m_zombie_text_gui_id = 59;
	int m_bullets_text_gui_id = 58;
	int m_fps_text_gui_id = 57;



	//game controls buttons
	int m_direction_center_gui_id = 56;
	int m_direction_body_gui_id = 55;

	int m_fire_accessories_gui_id = 54;
	int m_fire_gradient_gui_id = 53;
	int m_fire_color_gui_id = 52;
	int m_fire_shadow_gui_id = 51;

	int m_move_accessories_gui_id = 50;
	int m_move_gradient_gui_id = 49;
	int m_move_color_gui_id = 48;
	int m_move_shadow_gui_id = 47;

	int m_pause_gui_id = 46;

	//pause screen 
	int m_continue_gui_id = 10;
	int m_main_menu_gui_id = 9;

	//game over screen

	int m_game_over_gui_id = 20;
	int m_your_score_gui_id = 19;
	int m_your_score_int_gui_id = 18;
	int m_high_score_gui_id = 17;
	int m_play_again_gui_id = 16;
	int m_main_menu_go_gui_id = 15;


	bool m_process_game_play_controls = true;
	bool m_display_game_play_controls = true;

	bool m_process_game_pause_controls = false;
	bool m_display_game_pause_controls = false;

	bool m_process_game_over_controls = false;
	bool m_display_game_over_controls = false;

	bool m_process_game_stats = true;
	bool m_display_game_stats = true;

	bool m_update_game_world= true;


	SquareBox::AudioSystem::AudioEngine m_audio_engine;

	SquareBox::AudioSystem::Music m_music;

	SquareBox::AudioSystem::SoundBank m_sound_bank;

};

