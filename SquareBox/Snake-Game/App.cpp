#include "App.h"

void App::onInit()
{

	auto pixel_data = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/nodpi/snake game icon.png");
    getWindow()->setWindowIcon(pixel_data.pixels,pixel_data.width,pixel_data.height);
    SquareBox::AssetManager::IOManager::freePixelData(pixel_data);
}

void App::addScreens()
{
	m_splash_screen_ptr = std::make_unique<SplashScreen>();
	m_screen_list->addScreen(m_splash_screen_ptr.get());

	m_game_screen_ptr = std::make_unique<GameScreen>();
	m_screen_list->addScreen(m_game_screen_ptr.get());
	m_screen_list->setScreen(m_splash_screen_ptr->getScreenIndex());


}

void App::onExit()
{
}
