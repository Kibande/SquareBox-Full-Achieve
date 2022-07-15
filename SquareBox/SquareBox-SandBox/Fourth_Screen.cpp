#include "Fourth_Screen.h"

Fourth_Screen::Fourth_Screen()
{

}

Fourth_Screen::~Fourth_Screen()
{
}

int Fourth_Screen::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int Fourth_Screen::getPreviousScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

//Called when the Screen is added to list of screens
void Fourth_Screen::build()
{
	m_sprite_batch.init();
	m_texture_program.init();
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertextUV");
	m_texture_program.linkShaders();

	m_debug_renderer.init();
	m_debug_program.init();
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert","Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertextColor");
	m_debug_program.linkShaders();

#ifdef SQB_PLATFORM_ANDROID
	m_sprite_font.init("fonts/Comfortaa-Bold.ttf", 32);
#else
	m_sprite_font.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);
#endif

	//layer
	m_layers.emplace_back();
	SquareBox::GWOM::Layer & active_layer=m_layers[m_active_layer_index];
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	active_layer.camera.init(screen_dimensions.x,screen_dimensions.y);
	active_layer.camera.setPosition(screen_dimensions*glm::vec2(0.5f));

	
}

//Called when the screen is  actived
void Fourth_Screen::onEntry()
{
	m_audio_engine_ptr_1 = new SquareBox::AudioSystem::AudioEngine();
	int audio_flags = static_cast<int>(SquareBox::AudioInputFormatEnum::MP3_FORMAT);
	audio_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OGG_FORMAT);
	audio_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MOD_FORMAT);
	audio_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MID_FORMAT);
	audio_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::FLAC_FORMAT);
	audio_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OPUS_FORMAT);
	m_audio_engine_ptr_1->init(audio_flags, 22050, SquareBox::AudioOutputFormatEnum::U8_AUDIO_OUTPUT, SquareBox::AudioChannlesEnum::MONO, 4096);
	SDL_GetNumAudioDevices(SDL_FALSE);
	auto music_raw_data = SquareBox::AssetManager::IOManager::getRawDataFromFile("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
	m_music_1 = SquareBox::AudioSystem::Music("", SquareBox::FallOffEnum::INVERSE, 10.0f, 50.0f,glm::vec2(0.0f), music_raw_data.first, music_raw_data.second);
	//lets load our music from here
	m_audio_engine_ptr_1->loadMusic(m_music_1);
	m_music_1.play();
	auto raw_data = SquareBox::AssetManager::IOManager::getRawDataFromFile("Assets/Audio/rifle.wav");
	m_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("", raw_data.first, raw_data.second));
	
	m_audio_engine_ptr_1->loadSoundBank(m_sound_bank);
	//m_sound_bank.play();

	auto sound_effect1=SquareBox::AudioSystem::SoundEffect("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
	auto sound_effect2=SquareBox::AudioSystem::SoundEffect("Assets/Audio/Patoranking-Love-You-Die-ft.-Diamond-Platnumz.mp3");
	auto sound_effect3=SquareBox::AudioSystem::SoundEffect("Assets/Audio/prettyboydo, Wani - Mentally (320).mp3");
	auto sound_effect4=SquareBox::AudioSystem::SoundEffect("Assets/Audio/Reminisce-ft-Fireboy-DML-Ogaranya.mp3");
	auto sound_effect5=SquareBox::AudioSystem::SoundEffect("Assets/Audio/Rudeboy-Reason-With-Me.mp3");
	auto sound_effect6=SquareBox::AudioSystem::SoundEffect("Assets/Audio/Tion Wayne feat. Afro B - Cant Go Broke.mp3");
	m_sound_bank = SquareBox::AudioSystem::SoundBank(SquareBox::FallOffEnum::LINEAR, 5.0f, 30.0f,glm::vec2(0.0f));
	//m_sound_bank.sound_effects.push_back(sound_effect1);
	//m_sound_bank.sound_effects.push_back(sound_effect2);
	//m_sound_bank.sound_effects.push_back(sound_effect3);
	//m_sound_bank.sound_effects.push_back(sound_effect4);
	//m_sound_bank.sound_effects.push_back(sound_effect5);

	//m_audio_engine_ptr_1->loadSoundBank(m_sound_bank);
	//m_sound_bank.play();
}

//called once every frame to update the game logic
void Fourth_Screen::update(const float & deltaTime_)
{
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	m_layers[m_active_layer_index].camera.update(screen_dimensions.x, screen_dimensions.y);


	m_audio_engine_ptr_1->update(glm::vec2(0.0f));

}

//called once every frame to draw the game
void Fourth_Screen::draw()
{
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_layers[m_active_layer_index].camera,"P");
	m_sprite_batch.begin();
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	m_sprite_font.draw(m_sprite_batch, "Fourth Screen", screen_dimensions*glm::vec2(0.5f), glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), SquareBox::JustificationEnum::MIDDLE);
	m_sprite_batch.end();
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	m_debug_program.use();
	uploadCameraInfo(&m_debug_program, &m_layers[m_active_layer_index].camera, "P");
	m_debug_renderer.begin();
	m_debug_renderer.end();
	m_debug_renderer.render();
	m_debug_program.unuse();
}

//called  when an exit game request is recieved from the system
void Fourth_Screen::onExit()
{
	/* Because the audio engine is being intialised in onEntry 
	When placed on in build , it is initialsed even when this screen is not going to be used 
	and we are getting a pop sound each time  that it is getting initialsed */
	m_audio_engine_ptr_1->dispose();
	delete m_audio_engine_ptr_1;
}

//called when the screen is being deactivated
void Fourth_Screen::destroy()
{
	m_sprite_batch.dispose();
	m_texture_program.dispose();
	m_debug_renderer.dispose();
	m_debug_program.dispose();
	m_sprite_font.dispose();
}
