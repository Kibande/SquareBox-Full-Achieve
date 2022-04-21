#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "MenuObject.h"
class MobileAppEditor_Screen :public SquareBox::IGameScreen
{
public:
	MobileAppEditor_Screen(SquareBox::RenderEngine::Window* window);
	~MobileAppEditor_Screen();

private:

	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void destroy() override;

	virtual void onEntry() override;

	virtual void onExit() override;

	virtual void update(const float & deltaTime_) override;

	virtual void draw() override;

private:
	SquareBox::RenderEngine::SpriteBatch m_spriteBatch;
	SquareBox::RenderEngine::SpriteFont m_spriteFont;
	SquareBox::RenderEngine::DebugRenderer m_debugRenderer;
	SquareBox::RenderEngine::Window* m_window;
	SquareBox::RenderEngine::GLSLProgram m_textureProgram;
	SquareBox::RenderEngine::GLSLProgram m_debugProgram;
	SquareBox::Camera::ParallelCamera m_camera;
	MenuObject mo;
	std::vector<MenuObject*> menu_manager_vec;
};
