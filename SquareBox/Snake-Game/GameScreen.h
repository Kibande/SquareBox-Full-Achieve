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

	SnakeGame::Move determineMovementDirection();

};

