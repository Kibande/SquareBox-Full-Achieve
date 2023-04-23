#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "ScreenIndicies.h"
#include "GameLogic.h"

class GameScreen: public SquareBox::IGameScreen
{

public:
	GameScreen() {};
	~GameScreen() {};


	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
	int m_previousScreenIndex = SPLASH_SCREEN_INDEX;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::Camera::ParallelCamera m_camera;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	SnakeGame::GameLogic m_game_logic;

	int heightScore = 40;

	SquareBox::AssetManager::GLTexture m_box_texture;
	SquareBox::AssetManager::GLTexture m_circle_texture;
	double m_time = 0.0f;
	SquareBox::Timer timer;

	SquareBox::GWOM::Text warning;

private:

	//Graphics
	SquareBox::GUIWrapper m_snake_game_home_gui;
	SquareBox::GUIWrapper m_snake_game_loading_screen_gui;
	SquareBox::GUIWrapper m_snake_game_pop_up_navigation_menu_gui;
	SquareBox::GUIWrapper m_snake_game_splash_screen_gui;

	std::map<int, int> m_snake_game_home_gui_elements_coordinates;
	std::map<int, int> m_snake_game_loading_screen_gui_elements_coordinates;
	std::map<int, int> m_snake_game_pop_up_navigation_menu_gui_elements_coordinates;
	std::map<int, int> m_snake_game_splash_screen_gui_elements_coordinates;

	int m_central_button = 5;
	int m_right_arrow = 2;
	int m_left_arrow = 4;
	int m_up_arrow = 1;
	int m_down_arrow = 3;

	int m_menu_button = 8;
	int m_pause_play = 6;
	int m_shoot_button = 7;
	int m_score_board = 9;

	int score = 10;
	int life_bar_border = 11;
	int life_bar_body = 12;

	bool show_mobile_controls = true;

	// set up the size of the world
	int world_width;
	int world_height;
};

