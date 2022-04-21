#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "Editor_Assistant.h"
class DesktopAppEditor_Screen :public SquareBox::IGameScreen
{
public:
	DesktopAppEditor_Screen(SquareBox::RenderEngine::Window* window);
	~DesktopAppEditor_Screen();

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
	SquareBox::RenderEngine::DebugRenderer m_debugRender;
	SquareBox::RenderEngine::Window* m_window;
	SquareBox::RenderEngine::GLSLProgram m_textureProgram;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::Camera::ParallelCamera m_camera;

	 //Shape
	 SquareBox::GWOM::ClusterObject box;
	 
	 SquareBox::GWOM::Layer m_layer;

	 SquareBoxEditor::Editor_Assistant m_assistant;


	 SquareBox::Utilities m_utilities;
};
