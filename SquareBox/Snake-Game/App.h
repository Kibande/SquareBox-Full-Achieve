#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
#include "GameScreen.h"
#include "SplashScreen.h"
class App: public SquareBox::IMainGame
{
public:
	App() {};
	~App() {};

	// Inherited via IMainGame
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;

	std::unique_ptr<SplashScreen> m_splash_screen_ptr = nullptr;
	std::unique_ptr<GameScreen> m_game_screen_ptr = nullptr;
};

