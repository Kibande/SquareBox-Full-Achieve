#include "Music.h"

namespace SquareBox {
	namespace AudioSystem {
		Music::Music()
		{
		}
		Music::Music(std::string file_path_, SquareBox::FallOffEnum fall_off_, float min_radius_, float max_radius_, glm::vec2 listerner_location_, char * data_, int data_length_)
		{
			m_file_path = file_path_;
			m_fall_off = fall_off_;
			m_min_radius = min_radius_;
			m_max_radius = max_radius_;
			m_data = data_;
			m_data_length = data_length_;
		}

		Music::~Music()
		{
		}

		void Music::play(int loops_)
		{
			if (m_is_music_loaded) {
				//only one music can play at one time
				m_active_channel = Mix_PlayMusic(m_music, loops_);
				if (m_active_channel == -1) {
					SBX_CORE_ERROR("Failed to play music {} {} : {} ", __FILE__, __LINE__, std::string(Mix_GetError()));
				};
			}
			else {
				SBX_CORE_ERROR("No Music is loaded");
			}
			

		}

		void Music::pause()
		{
			Mix_PauseMusic();
		}

		void Music::fadeIn(int milliseconds_, int loops_)
		{
			if (m_is_music_loaded) {
				m_active_channel = Mix_FadeInMusic(m_music, loops_, milliseconds_);
				if (m_active_channel == -1) {
					SBX_CORE_ERROR("Failed to Fade in music {} {} : {} ", __FILE__, __LINE__, std::string(Mix_GetError()));
				}
			}
			else {
				SBX_CORE_ERROR("No Music is loaded");
			}
		}

		void Music::fadeOut(int milliseconds_)
		{
			if (Mix_FadeOutMusic(milliseconds_) == -1) {
				SBX_CORE_ERROR("Failed to Fade out music {} {} : {} ", __FILE__, __LINE__, std::string(Mix_GetError()));
			}
		}

		void Music::stop()
		{
			Mix_HaltMusic();
		}

		void Music::unLoad()
		{
			// actual freeing of data is done on dispose of the audio Engine

			if (m_is_music_loaded) {
				stop();
				m_is_music_loaded = false;
				m_file_path = "";
				m_fall_off = SquareBox::FallOffEnum::LINEAR;
				m_min_radius = 0.1f;
				m_max_radius = 1.0f;
				m_data = nullptr;
				m_data_length = 0;
			}
		}

		void Music::resume()
		{
			Mix_ResumeMusic();
			
		}

		void Music::rewind()
		{
			Mix_RewindMusic();
		}
		void Music::listMusicDecorders()
		{
			unsigned int num_music_decoders = getNumMusicDecorders();
			for (unsigned int i = 0; i < num_music_decoders; i++)
			{
				SBX_CORE_INFO("Music Decorder {} ", Mix_GetMusicDecoder(i));
			}
		}
		void Music::setPosition(double position_)
		{
			if (Mix_SetMusicPosition(position_) == -1) {
				SBX_CORE_ERROR("Mix_SetMusicPosition: {}", Mix_GetError());
			}

		}
	};
};