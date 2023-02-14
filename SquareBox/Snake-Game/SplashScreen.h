#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
#include "ScreenIndicies.h"
class SplashScreen: public SquareBox::IGameScreen
{
public:
	SplashScreen() {};
	~SplashScreen() {};
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

private:

	SquareBox::AssetManager::GLTexture m_game_logo;
	SquareBox::AssetManager::GLTexture m_game_studio_logo;

	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	SquareBox::Camera::ParallelCamera m_camera;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;

	int m_next_screen_index = GAME_SCREEN_INDEX;
	int m_previous_screen_index = SCREEN_INDEX_NO_SCREEN;

	SquareBox::Timer m_timer;

	float m_initial_recorded_time;

	int m_current_texture_id = 0;

};

