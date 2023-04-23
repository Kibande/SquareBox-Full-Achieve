#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "First_Screen.h"
#include "Second_Screen.h"
#include "Fourth_Screen.h"

class App : public SquareBox::IMainGame
{
public:
	App();
	~App();

	// Inherited via IMainGame
	virtual void onInit() override;

	virtual void addScreens() override;

	virtual void onExit() override;
	//The memory for the screens will be managed from here
private:
	std::unique_ptr<First_Screen> m_first_screen;
	std::unique_ptr<Second_Screen> m_second_screen;
	std::unique_ptr<Fourth_Screen> m_fourth_screen;
};
