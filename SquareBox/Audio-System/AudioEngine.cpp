#include "AudioEngine.h"
#include<SquareBox-Logger/Logger.h>
#include <SDL/include//SDL.h>

namespace SquareBox {
	namespace AudioSystem {
		AudioEngine::AudioEngine()
		{
		}

		AudioEngine::~AudioEngine()
		{
			dispose();
		}

		void AudioEngine::init() {
			
			int m_input_audio_format_flags = static_cast<int>(SquareBox::AudioInputFormatEnum::MP3_FORMAT);
			m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OGG_FORMAT);
			m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MOD_FORMAT);
			m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MID_FORMAT);
			m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::FLAC_FORMAT);
			m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OPUS_FORMAT);
			init(m_input_audio_format_flags, MIX_DEFAULT_FREQUENCY, SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT, SquareBox::AudioChannlesEnum::STEREO, 4096);
		
		}

		void AudioEngine::init(int input_audio_format_flags_,int audio_frequency_, SquareBox::AudioOutputFormatEnum audio_output_format_, SquareBox::AudioChannlesEnum audio_channels_, int audio_buffer_size_)
		{
			if (m_is_initialized) {
				SBX_CORE_ERROR("The Audio Engine is already initialised !");
			}
			else
			{
				//make sure that SDL is initialized before this is called
				// Parameter can be a bitwise combination of MIX_INIT_FAC,
				//MIX_INIT_MOD,MIX_INIT_MP3,MIX_INIT_OGG
				
	

				//initialize the frequency and format
				m_buffer_size = audio_buffer_size_;
				if (Mix_OpenAudio(audio_frequency_, static_cast<int>(audio_output_format_), static_cast<int>(audio_channels_), audio_buffer_size_) <0) {
					SBX_CORE_ERROR("Mix_Init error :{} ", std::string(Mix_GetError()));
					dispose();
				}
				else {
					int initted = Mix_Init(input_audio_format_flags_);
					if ((initted&input_audio_format_flags_) != input_audio_format_flags_) {
						SBX_CORE_ERROR("Failed to load support for some audio formats {} {} ",__FILE__,__LINE__);
						SBX_CORE_ERROR("Mix_Init error {} : ", std::string(Mix_GetError()));
					};

					

				};

				m_is_initialized = true;
			}
		
		}

		void AudioEngine::loadSoundBank(SoundBank & bank_) {

			//load all sound  Effects
			for (unsigned int i = 0; i < bank_.sound_effects.size(); i++)
			{
				SoundEffect & sound = bank_.sound_effects[i];
				auto it = m_effect_map.find(sound.m_file_path);
				if (it == m_effect_map.end())
				{
					Mix_Chunk* chunk = nullptr;
					//Failed to find it, so must load
					if (sound.m_data != nullptr) {
						chunk = Mix_LoadWAV_RW(SDL_RWFromMem(sound.m_data, sound.m_data_length),0);
						if (chunk == nullptr) {
							SBX_CORE_ERROR("Failed to load SoundEffect  error : {} ", std::string(Mix_GetError()));
						}
				
					}
					else {
						 chunk = Mix_LoadWAV(sound.m_file_path.c_str());
						if (chunk == nullptr) {
							SBX_CORE_ERROR("Failed to load SoundEffect file {}  error : {} ", sound.m_file_path, std::string(Mix_GetError()));
						}
					}
					//cache the chuck
					sound.m_chunk = chunk;
					m_effect_map[sound.m_file_path] = chunk;
					
				}
				else {
					//else it is already cached
					sound.m_chunk = it->second;
				}
				

			}
			bank_.m_is_loaded = true;

		}
		void AudioEngine::loadMusic(Music & music) {
			//Try to find the AudioEngine in the cache
			auto it = m_music_map.find(music.m_file_path);
			if (it == m_music_map.end())
			{
				Mix_Music* mixMusic = nullptr;
				if (music.m_data != nullptr) {
					mixMusic = Mix_LoadMUS_RW(SDL_RWFromMem(music.m_data,music.m_data_length),0);
					if (mixMusic == nullptr) {
						std::string errorMessage = std::string(Mix_GetError());
						SBX_CORE_ERROR("Failed to load AudioEngine file : {} ", errorMessage);
					}
					else {
						music.m_is_music_loaded = true;
					}
				}
				else {
					mixMusic = Mix_LoadMUS(music.m_file_path.c_str());
					if (mixMusic == nullptr) {
						std::string errorMessage = std::string(Mix_GetError());
						SBX_CORE_ERROR("Failed to load AudioEngine file {} : {} ",music.m_file_path, errorMessage);
					}
					else {
						music.m_is_music_loaded = true;
					}
				}

				//cache the chuck
				music.m_music = mixMusic;
				m_music_map[music.m_file_path] = mixMusic;
			}
			else {
				music.m_is_music_loaded = true;
				//else it is already cached so retrive the cached data
				music.m_music = it->second;
			}
		
		}
		void AudioEngine::update(glm::vec2 listerner_location_) {
			//this is where the reducing in volume , pitch and other stuff will be done from
		}

		SquareBox::AudioOutputFormatEnum AudioEngine::getAudioOutputFormatEnum()
		{
			int quered_audio_rate;
			Uint16  quered_format;
			int quered_channels;

			Mix_QuerySpec(&quered_audio_rate, &quered_format, &quered_channels);

			switch (static_cast<int>(quered_format))
			{
				case static_cast<int>(SquareBox::AudioOutputFormatEnum::S8_AUDIO_OUTPUT) :
					return SquareBox::AudioOutputFormatEnum::S8_AUDIO_OUTPUT;
				case static_cast<int>(SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT) :
					return SquareBox::AudioOutputFormatEnum::S8_AUDIO_OUTPUT;
				case static_cast<int>(SquareBox::AudioOutputFormatEnum::U8_AUDIO_OUTPUT) :
					return SquareBox::AudioOutputFormatEnum::U8_AUDIO_OUTPUT;
				case static_cast<int>(SquareBox::AudioOutputFormatEnum::U16_AUDIO_OUTPUT) :
					return SquareBox::AudioOutputFormatEnum::U16_AUDIO_OUTPUT;
			default:
				SBX_CORE_ERROR("Error Unidentified audio output");
					return SquareBox::AudioOutputFormatEnum::UNIDENTIFIED_AUDIO_OUTPUT;
				break;
			}
		}

		void AudioEngine::dispose()
		{
			if (m_is_initialized) {
				m_is_initialized = false;


				//free all the cached audios
				for (auto& it : m_effect_map) {
					if (it.second != nullptr) {
						Mix_FreeChunk(it.second);
					}
				}

				for (auto& it : m_music_map) {
					if (it.second != nullptr) {
						Mix_FreeMusic(it.second);
					}
				}

				m_effect_map.clear();
				m_music_map.clear();


				/* 
				* 
					this while loop once called will close up all the 
					Instances of the AudioEngine that are currently open 
				*
				*/


			

				while (Mix_Init(0))
				{
					Mix_Quit();
				}
				//Close the AudioEngine
				int quered_audio_rate;
				Uint16  quered_format;
				int quered_channels;

				
				int num_opened_times = Mix_QuerySpec(&quered_audio_rate, &quered_format, &quered_channels);
				for (unsigned int i = 0; i < num_opened_times; i++)
				{
					Mix_CloseAudio();
				}
			}
		}
		void AudioEngine::queryAudioParameters()
		{
			int quered_audio_rate;
			Uint16  quered_format;
			int quered_channels;

			Mix_QuerySpec(&quered_audio_rate, &quered_format, &quered_channels);
			SBX_CORE_INFO("Opened audio at {} Hz {} bit {} {} {} bytes audio buffer\n", quered_audio_rate,
				(quered_format & 0xFF),
				(SDL_AUDIO_ISFLOAT(quered_format) ? " (float)" : ""),
				(quered_channels > 2) ? "surround" : (quered_channels > 1) ? "stereo" : "mono",
				m_buffer_size);
		}
	}
}