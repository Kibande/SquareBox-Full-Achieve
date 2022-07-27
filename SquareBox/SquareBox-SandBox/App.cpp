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
	
	//m_second_screen = std::make_unique<Second_Screen>();
	//m_screen_list->addScreen(m_second_screen.get());

	//m_fourth_screen = std::make_unique<Fourth_Screen>();
	//m_screen_list->addScreen(m_fourth_screen.get());

	//Our starting Screen
	m_screen_list->setScreen(m_first_screen->getScreenIndex());
}

void App::onExit()
{
}