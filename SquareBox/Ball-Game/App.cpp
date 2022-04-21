#include "App.h"

void App::onInit()
{
}

void App::addScreens()
{
	//make the unique pointer
	m_game_play_screen = std::make_unique<GamePlayeScreen>(&m_window);
	m_screenList->addScreen(m_game_play_screen.get());

	m_screenList->setScreen(m_game_play_screen->getScreenIndex());
}

void App::onExit()
{
}