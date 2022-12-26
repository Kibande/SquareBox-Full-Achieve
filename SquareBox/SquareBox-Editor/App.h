#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "Welcome_Screen.h"
#include "LevelEditor.h"
#include "GUIEditor.h"
#include "AnimationEditor.h"

class App : public SquareBox::IMainGame
{
public:
	App();
	~App();

private:

	// Inherited via IMainGame
	virtual void onInit() override;

	virtual void addScreens() override;

	virtual void onExit() override;

	//The memory for the screens will be managed from here
	std::unique_ptr<Welcome_Screen> m_welcome_screen = nullptr;
	std::unique_ptr<LevelEditor> level_editor_screen_model_2 = nullptr;
	std::unique_ptr<GUIEditor> m_gui_editor_screen = nullptr;
	std::unique_ptr<AnimationEditor> m_animation_editor_screen = nullptr;
};
