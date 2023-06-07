#include "GamePlayeScreen.h"


GamePlayeScreen::GamePlayeScreen()
{
	

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
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() *0.5f, m_game_ptr->getWindow()->getScreenHeight()*0.5f));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();


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
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");

	m_sprite_batch.begin();



	m_sprite_batch.end();
	m_sprite_batch.renderBatch();

	m_texture_program.unuse();

}

void GamePlayeScreen::onExit()
{

}

void GamePlayeScreen::destroy()
{
	m_texture_program.dispose();
	m_camera.dispose();
	m_sprite_batch.dispose();
}


