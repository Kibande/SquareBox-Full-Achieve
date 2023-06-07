#include "App.h"

void App::onInit()
{
}

void App::addScreens()
{
	m_game_play_screen_ptr = std::make_unique<GamePlayScreen>();
	m_screen_list->addScreen(m_game_play_screen_ptr.get());

	//set our starting screen
	m_screen_list->setScreen(m_game_play_screen_ptr->getScreenIndex());
}

void App::onExit()
{
}
