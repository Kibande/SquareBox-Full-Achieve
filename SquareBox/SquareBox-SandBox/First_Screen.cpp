#include "First_Screen.h"

/*
Order of Methods Calling
	Constructor
	Build
	OnEntry

	//game loop
	 (while ScreenState::RUNNING){
		 Update
		 Draw
	   }

	OnExit
	Destory
	Deconstructor

*/


First_Screen::First_Screen()
{

}

int First_Screen::getNextScreenIndex() const
{
	return 0;
}

int First_Screen::getPreviousScreenIndex() const
{
	return 0;
}

void First_Screen::build()
{
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.5f));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();

	
	m_sprite_font.initWithFilePath("Assets/Fonts/DroidSans.ttf", 8);



}

void First_Screen::onEntry()
{

}



void First_Screen::update(const float& deltaTime_)
{
	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
}

void First_Screen::draw()
{
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");

	m_sprite_batch.begin();

	
	m_sprite_font.draw(m_sprite_batch, "COLONY PROTOCOL", m_camera.convertScreenToWorld(glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight()) * 0.5f), glm::vec2(2), 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), SquareBox::JustificationEnum::MIDDLE);

	m_sprite_batch.end();
	m_sprite_batch.renderBatch();

	m_texture_program.unuse();

}

void First_Screen::onExit()
{

}

void First_Screen::destroy()
{
	m_texture_program.dispose();
	m_camera.dispose();
	m_sprite_batch.dispose();
	m_sprite_font.dispose();
}


