#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
class GamePlayeScreen :
	public SquareBox::IGameScreen
{
public:
	GamePlayeScreen(SquareBox::RenderEngine::Window * window_);
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float & deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

private:
	SquareBox::RenderEngine::Window * m_window = nullptr;
	//we need a camera
	SquareBox::Camera::ParallelCamera m_camera;
	//we need a spriteBatch
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;

	//our texture Program
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	//our audio system
	SquareBox::AudioSystem::AudioEngine m_audio_engine;
};
