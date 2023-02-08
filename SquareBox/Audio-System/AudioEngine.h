#pragma once
#include <SDL/include/SDL_mixer.h>
#include <string>
#include <map>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
#include "SoundBank.h"
#include "SoundEffect.h"
#include "Music.h"
namespace SquareBox {
	namespace AudioSystem {
		/*	LINEAR - Good for sounds heard over a large area to make it easier for
			listeners to gauge distance from source
			LOGARITHMIC - Good for continuous complex environmental sounds like wind through trees
			REVERSELOG - Loud Sounds like the core of an explosion or fire gun;
			INVERSE - For Sounds heard clearly up close but at any distance beyond that only give a hint that
				they are nearby . Works well for terminals and machines with softer sounds
		*/

		class AudioEngine
		{
		public:
			AudioEngine();
			~AudioEngine();
			void init();
			void init(int input_audio_format_flags_,int audio_frequency_=static_cast<int>(MIX_DEFAULT_FREQUENCY), SquareBox::AudioOutputFormatEnum audio_output_format_= SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT,SquareBox::AudioChannlesEnum audio_channels_=SquareBox::AudioChannlesEnum::MONO,int audio_buffer_size_=4096);
			void dispose();
			void queryAudioParameters();
			void loadSoundBank(SoundBank & bank_);
			void loadMusic(Music & music_data_);//orign of -1,-1 means that its a general sound
			void update(glm::vec2 listerner_location_);//the location of the person hearing this audio engines out put
			SquareBox::AudioOutputFormatEnum getAudioOutputFormatEnum();


			
			
		private:
			int m_buffer_size;
			bool m_is_initialized = false;
			//our variable to hold the cached AudioEngine files
			std::map<std::string, Mix_Music*> m_music_map;
			std::map<std::string, Mix_Chunk*> m_effect_map;
			std::vector<SoundEffect> m_active_sound_effects;
		};
	};
};
