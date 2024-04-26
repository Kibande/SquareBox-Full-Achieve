#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "First_Screen.h"


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
	std::unique_ptr<First_Screen> m_first_screen = nullptr;

};
