#pragma once
#include "GamePlayeScreen.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include<memory>
class App :
	public SquareBox::IMainGame
{
	// Inherited via IMainGame
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;

	//GamePlayScreen Pointer
	std::unique_ptr<GamePlayeScreen> m_game_play_screen = nullptr;

};
