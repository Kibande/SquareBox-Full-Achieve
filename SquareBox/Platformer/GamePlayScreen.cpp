#include "GamePlayScreen.h"

int GamePlayScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

int GamePlayScreen::getPreviousScreenIndex() const
{
	return m_previousScreenIndex;
}

void GamePlayScreen::build()
{
}

void GamePlayScreen::onEntry()
{

	m_sprite_batch.init();

	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions);

	m_camera.setPosition(screen_dimensions * 0.5f);
	m_camera.setScale(1.0f);
}

void GamePlayScreen::update(const float & deltaTime_)
{

	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
}

void GamePlayScreen::draw()
{
}

void GamePlayScreen::onExit()
{
	
	m_sprite_batch.begin();

	// make the draw calls here

	m_sprite_batch.end();

	m_texture_program.use();
	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
	IGameScreen::uploadCameraInfo(&m_texture_program, &m_camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();
}

void GamePlayScreen::destroy()
{
	m_sprite_batch.dispose();
	m_texture_program.dispose();
	m_sprite_font.dispose();
}
