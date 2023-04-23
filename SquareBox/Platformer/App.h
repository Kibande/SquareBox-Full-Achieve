#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "GamePlayScreen.h"

class App : public SquareBox::IMainGame
{
public:
	App() {};
	~App() {};

	// Inherited via IMainGame
	virtual void onInit() override;

	virtual void addScreens() override;

	virtual void onExit() override;
	//The memory for the screens will be managed from here
private:
	std::unique_ptr<GamePlayScreen> m_game_play_screen_ptr;
};
