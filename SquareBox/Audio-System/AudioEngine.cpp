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

		void AudioEngine::init(int input_audio_format_flags_,int frequency_, SquareBox::AudioOutputFormatEnum audio_output_format_, SquareBox::AudioChannlesEnum channels_, int chunk_size_)
		{
			if (m_is_initialized) {
				SBX_CORE_ERROR("The Audio Engine is already initialised !");
			}
			else
			{
				//make sure that SDL is initialized before this is called
				// Parameter can be a bitwise combination of MIX_INIT_FAC,
				//MIX_INIT_MOD,MIX_INIT_MP3,MIX_INIT_OGG
				
				int initted = Mix_Init(input_audio_format_flags_);
				if (initted&input_audio_format_flags_ != input_audio_format_flags_) {
					SBX_CORE_ERROR("Failed to load support for some audio formats {} {} ",__FILE__,__LINE__);
					SBX_CORE_ERROR("Mix_Init error {} : ", std::string(Mix_GetError()));
				};

				//initialize the frequency and format
				//and the number of bits to handle at once
				if (Mix_OpenAudio(frequency_, static_cast<int>(audio_output_format_), static_cast<int>(channels_), chunk_size_) == -1) {
					SBX_CORE_ERROR("Mix_Init error :{} ", std::string(Mix_GetError()));
				};
				//step up channels of channels
				m_total_channel = Mix_AllocateChannels(m_total_channel);
				SBX_CORE_INFO(" {} Audio Mix Channels Initialised ", m_total_channel);
				m_is_initialized = true;
			}
			//__debugbreak();
				SBX_CORE_INFO("Audio Engine has a read me file at {} {} and is scheduled for renovation",__FILE__,__LINE__);
				/*
				i dont have time to continue with this right now with papers coming up.
				i started a small personal drive to always have a project out every weekend .
				for this weekend i set a target to have a local hosted communication radio system .
				My plan is to record audio in small small frames and send it to over or network lib,
				and play it on the recieving end . CUrrenet issues are , 
				. Our Engine currently can't handle recording audio
				. Our Network Lib has an intellisense error that forced me to exclude it from the general
				  exposed API.

				This whole project idea came in while i was working on the audio Engine. I had decied
				to read through SDL_Mixers documentation while furnishing out nd fully builidng out the,
				audio Engine . The desire to enable recording , audio recording sent me down a rabbit hole in 
				search for answers.

				This rbbit hole delivered me to the door steps to Lazy Foo' Production. 
				A site i had been at before , but who glory had been buried in the hip of work 
				This site has airticles on some many topic that if i learn , the engine will  
				be propelled to really go far. Some of these features are Gamepads and Joysticks, Force Feedback,
				Timing, Advanced Timers, Audio Recording, Multiple Displays, Mutexes and Conditions, Touches
				MultiTouche

				Link: http://lazyfoo.net/tutorials/SDL/index.php
				SDL homepage tutorials: https://wiki.libsdl.org/Tutorials
				Game Dev with SDL tutorials: https://www.parallelrealities.co.uk/tutorials/
				*/
		}

		void AudioEngine::loadSoundBank(SoundBank & bank_) {
			//Try to find the AudioEngine in the cache

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
				//else it is already cached

			}
			bank_.m_total_channels = m_total_channel;
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
				}
				else {
					mixMusic = Mix_LoadMUS(music.m_file_path.c_str());
					if (mixMusic == nullptr) {
						std::string errorMessage = std::string(Mix_GetError());
						SBX_CORE_ERROR("Failed to load AudioEngine file {} : {} ",music.m_file_path, errorMessage);
					}
				}

				//cache the chuck
				music.m_music = mixMusic;
				m_music_map[music.m_file_path] = mixMusic;
			}
			//else it is already cached
		}
		void AudioEngine::update(glm::vec2 listerner_location_) {
			//this is where the reducing in volume , pitch and other stuff will be done from
		}

		SquareBox::AudioOutputFormatEnum AudioEngine::getAudioOutputFormatEnum()
		{
			switch (static_cast<int>(m_output_format))
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
				while (Mix_Init(0))
				{
					Mix_Quit();
				}
				//Close the AudioEngine
				int num_opened_times = Mix_QuerySpec(&m_frequency, &m_output_format, &m_channel);
				for (unsigned int i = 0; i < num_opened_times; i++)
				{
					Mix_CloseAudio();
				}
			}
		}
		void AudioEngine::refreshAudioProperties()
		{
			if (!Mix_QuerySpec(&m_frequency, &m_output_format, &m_channel)) {
				SBX_CORE_ERROR("Failed to refresh Audio Properties");
				SBX_CORE_ERROR(" {}: {} {} ", std::string(Mix_GetError()),__FILE__,__LINE__);
			}
		}
	}
}