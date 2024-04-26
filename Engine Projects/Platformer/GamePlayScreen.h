#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
class GamePlayScreen :public SquareBox::IGameScreen
{
public:
	GamePlayScreen(){};
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

	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
	int m_previousScreenIndex = SCREEN_INDEX_NO_SCREEN;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::Camera::ParallelCamera m_camera;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	SquareBox::GWOM::Layer m_layer;
};

