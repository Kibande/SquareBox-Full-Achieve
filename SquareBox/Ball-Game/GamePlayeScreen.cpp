#include "GamePlayeScreen.h"

GamePlayeScreen::GamePlayeScreen(SquareBox::RenderEngine::Window * window_) :m_window(window_)
{
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth(), m_window->getScreenHeight()));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();
}

int GamePlayeScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayeScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayeScreen::build()
{
}

void GamePlayeScreen::onEntry()
{
}

void GamePlayeScreen::update(const float & deltaTime_)
{
}

void GamePlayeScreen::draw()
{
	m_texture_program.use();

	preUpdateShader(&m_texture_program, "mySampler", m_game_ptr->getWindowBGColor());
	preUpdateCamera(&m_texture_program, &m_camera, "P");

	m_texture_program.unuse();
}

void GamePlayeScreen::onExit()
{
	m_texture_program.dispose();
	m_camera.dispose();
}

void GamePlayeScreen::destroy()
{
}