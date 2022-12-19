#include "GamePlayeScreen.h"

GamePlayeScreen::GamePlayeScreen(SquareBox::RenderEngine::Window * window_) :m_window(window_)
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
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth(), m_window->getScreenHeight()));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	
}

void GamePlayeScreen::onEntry()
{
	m_window->setWindowOpacity(1.0f);



	SquareBox::AudioSystem::SoundBank sound_bank(SquareBox::FallOffEnum::LINEAR);

	sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("Sound Effect 1", "Assets/Audio/cg1.wav"));
	sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("Sound Effect 2", "Assets/Audio/pistol.wav"));
	sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("Sound Effect 3", "Assets/Audio/rifle.wav"));
	sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("Sound Effect 4", "Assets/Audio/shotgun.wav"));
	
	m_audio_engine.loadSoundBank(sound_bank);

	sound_bank.play();
}

void GamePlayeScreen::update(const float & deltaTime_)
{
}

void GamePlayeScreen::draw()
{
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");

	m_texture_program.unuse();
}

void GamePlayeScreen::onExit()
{

}

void GamePlayeScreen::destroy()
{
	m_texture_program.dispose();
	m_camera.dispose();
	
}