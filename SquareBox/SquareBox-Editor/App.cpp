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
	m_welcome_screen = std::make_unique<Welcome_Screen>(&m_window);
	m_screen_list->addScreen(m_welcome_screen.get());

	level_editor_screen_model_2 = std::make_unique<LevelEditor>(&m_window);
	m_screen_list->addScreen(level_editor_screen_model_2.get());

	desktop_app_editor_screen = std::make_unique<DesktopAppEditor_Screen>(&m_window);
	m_screen_list->addScreen(desktop_app_editor_screen.get());

	mobile_app_editor_screen = std::make_unique<MobileAppEditor_Screen>(&m_window);
	m_screen_list->addScreen(mobile_app_editor_screen.get());

	// Our starting Screen
	m_screen_list->setScreen(m_welcome_screen->getScreenIndex());
	//m_screen_list->setScreen(desktop_app_editor_screen->getScreenIndex());
}

void App::onExit()
{
	SBX_INFO("App On Exit Called");
}