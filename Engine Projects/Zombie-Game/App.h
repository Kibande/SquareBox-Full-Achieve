#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "GamePlayScreen.h";
class App : public SquareBox::IMainGame
{
public:
	// Inherited via IMainGame
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;
private:
	//screen pointers
	std::unique_ptr<GamePlayScreen> m_game_play_screen_ptr;
};

