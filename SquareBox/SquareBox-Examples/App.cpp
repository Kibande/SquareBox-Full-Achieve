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
	m_example_home_screen = std::make_unique<ExampleHomeScreen>(&m_window);
	m_screen_list->addScreen(m_example_home_screen.get());

	//Our starting Screen
	m_screen_list->setScreen(m_example_home_screen->getScreenIndex());
}

void App::onExit()
{
}