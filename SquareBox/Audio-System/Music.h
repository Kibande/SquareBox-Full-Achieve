#pragma once
#include <SDL/include/SDL_mixer.h>
#include <string>
#include <map>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
namespace SquareBox {
	namespace AudioSystem {
		class Music
		{
		public:
			Music();
			Music(std::string file_path_, SquareBox::FallOffEnum fall_off_, float min_radius_, float max_radius_, glm::vec2 listerner_location_, char * data_ = nullptr, int data_length_ = 0);
			~Music();
			//Plays the AudioEngine file
			//@parm loops_: if loops_==-1 loop forever,otherwise play it loops_ times
			void play(int loops_ = 0);
			friend class AudioEngine;

			static void pause();
			static void stop();
			static void resume();
			static void rewind();

		private:
			Mix_Music* m_music = nullptr;
			std::string m_file_path;
			char * m_data = nullptr;
			int m_data_length = 0;
			glm::vec2 m_origin;
			SquareBox::FallOffEnum m_fall_off;
			float m_min_radius;
			float m_max_radius;
		};
	};
};