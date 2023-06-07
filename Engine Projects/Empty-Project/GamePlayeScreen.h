#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>


class GamePlayeScreen :
	public SquareBox::IGameScreen
{
public:
	GamePlayeScreen();
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float & deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;
	bool CheckLua(lua_State* L, int r);

private:
	//we need a camera
	SquareBox::Camera::ParallelCamera m_camera;


	//our texture Program
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	//we need a spriteBatch
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
};
