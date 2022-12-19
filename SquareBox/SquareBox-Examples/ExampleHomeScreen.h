#pragma once
#include "ScreenIndices.h"
#include "IExample.h"
#include<SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl.h>
#include <ImGUI/imgui_impl_opengl3.h>
#include <ImGui/ImGuiFileBrowser.h>
#include "ParticleSystemExample.h"
#include "AudioSystemExample.h"

class ExampleHomeScreen :public SquareBox::IGameScreen
{
public:
	ExampleHomeScreen(SquareBox::RenderEngine::Window* window);
	~ExampleHomeScreen();

	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	void setActiveExample(int index);
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;
	void initGUI();
	void drawGUI();
	void showMainMenu();

private:
	//these will be shared by all examples
	//the examples role is just to show the implementation of each package
	SquareBox::RenderEngine::SpriteBatch m_spriteBatch;
	SquareBox::RenderEngine::SpriteFont m_spriteFont;
	SquareBox::RenderEngine::Window* m_window;
	SquareBox::RenderEngine::GLSLProgram m_textureProgram;
	SquareBox::Camera::ParallelCamera m_camera;
	std::vector<SquareBox::Example::IExample*> m_vec_examples_pointer;
	SquareBox::Example::IExample* m_active_example = nullptr;
	float m_kContstantForScaleAndZoom = 1.0f;
	float m_cameraPaningSpeed = 50.0f;
	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
	int m_previousScreenIndex = SCREEN_INDEX_NO_SCREEN;
	bool m_autoClearLogs = false;//should the log be clear each time we switch examples
	bool m_showAbout = false;
	int m_selectedExampleIndex = 0;


};
