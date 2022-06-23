#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
class GamePlayScreen :public SquareBox::IGameScreen
{
public:
	GamePlayScreen(SquareBox::RenderEngine::Window * window_ptr_) :m_window_ptr(window_ptr_) {};
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
	SquareBox::RenderEngine::Window * m_window_ptr;

	//texture stuff
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	//our layer
	SquareBox::GWOM::Layer m_layer;

	// Level storage
	std::string sLevel;
	int nLevelWidth;
	int nLevelHeight;
};

