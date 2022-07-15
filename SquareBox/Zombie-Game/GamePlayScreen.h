#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "ScreenIndicies.h"
#include "GameHUDandGUI.h"
#include "Gamelogic.h"
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
	SquareBox::AudioSystem::Music m_music_1;
	SquareBox::AudioSystem::SoundBank m_sound_bank;

	/* 
		Known game issues , there is a lag when crossing from one cluster object to another
		and there is also a replusion like force that tries to prevent them from crossing over cell
		boundaries
	
	
	*/

};

