#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "ScreenIndicies.h"
#include "GameHUDandGUI.h"
#include "Gamelogic.h"
class GamePlayScreen : public SquareBox::IGameScreen
{
public:
	GamePlayScreen(SquareBox::RenderEngine::Window * window_) :m_window_ptr(window_) {};
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float & deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

	//variables
	SquareBox::RenderEngine::Window * m_window_ptr = nullptr;

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

	//Audio Engine
	SquareBox::AudioSystem::AudioEngine * m_audio_engine_ptr = nullptr;

	//layers
	std::vector<SquareBox::GWOM::Layer> m_layers;

	//Graphics
	GameHUDandGUI m_game_huds_and_gui;

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
	bool m_object_show_coordinates = false;
	bool m_show_grid = false;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;


	/* 
		Known game issues , there is a lag when crossing from one cluster object to another
		and there is also a replusion like force that tries to prevent them from crossing over cell
		boundaries
	
	
	*/

};

