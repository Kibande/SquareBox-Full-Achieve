#pragma once
//#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <SquareBox-Core/IMainGame.h>
#include "Welcome_Screen.h"
#include "MobileAppEditor_Screen.h"
#include "DesktopAppEditor_Screen.h"
#include "LevelEditor.h"

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
	std::unique_ptr<DesktopAppEditor_Screen> desktop_app_editor_screen = nullptr;
	std::unique_ptr<MobileAppEditor_Screen> mobile_app_editor_screen = nullptr;
};
