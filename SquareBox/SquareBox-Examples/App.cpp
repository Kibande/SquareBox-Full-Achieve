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
	m_screenList->addScreen(m_example_home_screen.get());

	//Our starting Screen
	m_screenList->setScreen(m_example_home_screen->getScreenIndex());
}

void App::onExit()
{
}