#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "HomeScreen.h"

class App : public SquareBox::IMainGame
{
public:
	App() {};
	~App() {};
	// Inherited via IMainGame
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;

	std::unique_ptr<HomeScreen> m_home_screen_ptr = nullptr;
};

