#include "App.h"

void App::onInit()
{
}

void App::addScreens()
{
	m_game_screen_ptr = std::make_unique<GameScreen>();
	m_screen_list->addScreen(m_game_screen_ptr.get());

	m_screen_list->setScreen(m_game_screen_ptr->getScreenIndex());
}

void App::onExit()
{
}
