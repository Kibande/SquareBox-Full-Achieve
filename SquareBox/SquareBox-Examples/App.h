#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
#include "ExampleHomeScreen.h"
class App :public SquareBox::IMainGame
{
public:
	App();
	~App();
	// Inherited via IMainGame
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;
private:
	std::unique_ptr<ExampleHomeScreen> m_example_home_screen;
};
