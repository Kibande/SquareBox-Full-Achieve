#include "App.h"

App::App()
{
}

App::~App()
{
}

void App::onInit()
{
}

void App::addScreens()
{
	m_first_screen = std::make_unique<First_Screen>();
	m_screen_list->addScreen(m_first_screen.get());
	m_screen_list->setScreen(m_first_screen->getScreenIndex());
}

void App::onExit()
{
}