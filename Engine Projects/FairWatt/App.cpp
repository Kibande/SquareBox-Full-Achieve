#include "App.h"

void App::onInit()
{
    auto pixel_data = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/logo.png");
    getWindow()->setWindowIcon(pixel_data.pixels, pixel_data.width, pixel_data.height);
    SquareBox::AssetManager::IOManager::freePixelData(pixel_data);
}

void App::addScreens()
{
    m_home_screen_ptr = std::make_unique<HomeScreen>();
    m_screen_list->addScreen(m_home_screen_ptr.get());
    m_screen_list->setScreen(m_home_screen_ptr->getScreenIndex());
}

void App::onExit()
{
}
