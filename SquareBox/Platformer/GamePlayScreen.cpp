#include "GamePlayScreen.h"

int GamePlayScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayScreen::build()
{
}

void GamePlayScreen::onEntry()
{
	m_texture_program.init();
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertextPosition");
	m_texture_program.addAttribute("vertextColor");
	m_texture_program.addAttribute("vertextUV");
	m_texture_program.linkShaders();

	m_sprite_batch.init();
	glm::vec2 screen_dimensions;
	screen_dimensions.x = std::max(m_window_ptr->getScreenWidth(),m_window_ptr->getScreenHeight());
	screen_dimensions.y = std::min(m_window_ptr->getScreenWidth(),m_window_ptr->getScreenHeight());
	m_layer.camera.init(screen_dimensions.x,screen_dimensions.y);

}

void GamePlayScreen::update(const float & deltaTime_)
{
	glm::vec2 screen_dimensions;
	screen_dimensions.x = std::max(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	screen_dimensions.y = std::min(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	m_layer.camera.update(screen_dimensions.x, screen_dimensions.y);
	m_window_ptr->update();
}

void GamePlayScreen::draw()
{
}

void GamePlayScreen::onExit()
{
	m_sprite_batch.begin();
	m_sprite_batch.end();

	//sprite draws should be done here

	m_texture_program.use();
	preUpdateShader(&m_texture_program,"mySampler");
	uploadCameraInfo(&m_texture_program, &m_layer.camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();
}

void GamePlayScreen::destroy()
{
	m_texture_program.dispose();
	m_sprite_batch.dispose();
}
