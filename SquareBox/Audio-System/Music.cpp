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
			//only one music can play at one time
			if (Mix_PlayMusic(m_music, loops_) == -1) {
				SBX_CORE_ERROR("Failed to play music {} {} : {} ",__FILE__,__LINE__, std::string(Mix_GetError()));
			};
		}

		void Music::pause()
		{
			Mix_PauseMusic();
		}

		void Music::stop()
		{
			Mix_HaltMusic();
		}

		void Music::resume()
		{
			Mix_ResumeMusic();
		}

		void Music::rewind()
		{
			Mix_RewindMusic();
		}
	};
};