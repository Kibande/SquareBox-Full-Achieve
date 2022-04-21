#pragma once
#include <SDL/include/SDL_mixer.h>
#include <string>
namespace SquareBox {
	namespace AudioSystem {
		class SoundEffect
		{
		public:

			SoundEffect();
			/// can only load wave, aiff, ogg, and voc files
			/// and even before this is done make sure the audio 
			/// engine inti support it flags
			SoundEffect(std::string  file_path_,char * data_=nullptr, int data_length_=0);
			~SoundEffect();

			friend class AudioEngine;
			friend class SoundBank;
		private:
			Mix_Chunk* m_chunk = nullptr;
			std::string m_file_path;
			int channel = 0;
			char * m_data = nullptr;
			int m_data_length = 0;
			static int m_last_sound_effect_channel;
		};
	};
};