#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_sdl.h>
#include<ImGUI/imgui_impl_opengl3.h>

class Welcome_Screen :public SquareBox::IGameScreen
{
public:
	Welcome_Screen(SquareBox::RenderEngine::Window* window);
	~Welcome_Screen();

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

	// GUI
	void initGUI();
	void drawGUI();

private:
	SquareBox::RenderEngine::SpriteBatch m_spriteBatch;
	SquareBox::RenderEngine::SpriteFont m_spriteFont;
	SquareBox::RenderEngine::Window* m_window;
	SquareBox::RenderEngine::GLSLProgram m_textureProgram;
	SquareBox::Camera::ParallelCamera m_camera;

	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
	int m_previousScreenIndex = SCREEN_INDEX_NO_SCREEN;
};
