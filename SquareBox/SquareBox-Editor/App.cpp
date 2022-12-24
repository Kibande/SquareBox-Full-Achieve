#include "App.h"

App::App()
{
}

App::~App()
{
}

void App::onInit()
{
	//window icon
	auto icon_pixel_data = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/nodpi/Colony Protocol Logo Small.png");
	this->getWindow()->setWindowIcon(icon_pixel_data.pixels, icon_pixel_data.width, icon_pixel_data.height);
	SquareBox::AssetManager::IOManager::freePixelData(icon_pixel_data);
}

void App::addScreens()
{
	m_welcome_screen = std::make_unique<Welcome_Screen>();
	m_screen_list->addScreen(m_welcome_screen.get());

	level_editor_screen_model_2 = std::make_unique<LevelEditor>();
	m_screen_list->addScreen(level_editor_screen_model_2.get());

	desktop_app_editor_screen = std::make_unique<DesktopAppEditor_Screen>();
	m_screen_list->addScreen(desktop_app_editor_screen.get());

	mobile_app_editor_screen = std::make_unique<MobileAppEditor_Screen>();
	m_screen_list->addScreen(mobile_app_editor_screen.get());

	m_gui_editor_screen = std::make_unique<GUIEditor>();
	m_screen_list->addScreen(m_gui_editor_screen.get());
	
	m_animation_editor_screen = std::make_unique<AnimationEditor>();
	m_screen_list->addScreen(m_animation_editor_screen.get());

	// Our starting Screen
	m_screen_list->setScreen(m_welcome_screen->getScreenIndex());
	//m_screen_list->setScreen(m_gui_editor_screen->getScreenIndex());
	//m_screen_list->setScreen(desktop_app_editor_screen->getScreenIndex());


}

void App::onExit()
{
	SBX_INFO("App On Exit Called");
}