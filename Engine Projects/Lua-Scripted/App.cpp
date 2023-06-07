#include "App.h"

void App::onInit()
{
}

void App::addScreens()
{
	//make the unique pointer
	m_game_play_screen = std::make_unique<GamePlayeScreen>();
	m_screen_list->addScreen(m_game_play_screen.get());

	m_screen_list->setScreen(m_game_play_screen->getScreenIndex());
}

void App::onExit()
{
}