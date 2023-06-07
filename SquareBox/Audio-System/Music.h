#pragma once
#include <SDL/include/SDL_mixer.h>
#include <string>
#include <map>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
#include<vector>
namespace SquareBox {
	namespace AudioSystem {
		class Music
		{
		public:
			Music();
			Music(std::string file_path_, SquareBox::FallOffEnum fall_off_ = SquareBox::FallOffEnum::LINEAR, float min_radius_ =0.1f, float max_radius_=1.0f, glm::vec2 listerner_location_=glm::vec2(0.0f), char * data_ = nullptr, int data_length_ = 0);
			~Music();
			//Plays the AudioEngine file
			//@parm loops_: if loops_==-1 loop forever,otherwise play it loops_ times
			void play(int loops_ = 0);
			friend class AudioEngine;

			 void pause();
			void fadeIn(int milliseconds_, int loops_ = 0); 
			void fadeOut(int milliseconds_);// applied to the currently playing music
			 void stop();
			 void unLoad();
			 void resume();
			 void rewind();

			 bool isPlaying() { return m_is_music_loaded && static_cast<bool>(Mix_Playing(m_active_channel)); }
			 bool isLoaded() { return m_is_music_loaded; }
			void listMusicDecorders();
			unsigned int getNumMusicDecorders() { return Mix_GetNumMusicDecoders(); };

			void setVolume(float volume_percentage_) {
				Mix_VolumeMusic(MIX_MAX_VOLUME * volume_percentage_);
			}
			float getVolumeRatio() {
				return (float)Mix_VolumeMusic(-1) / (float)MIX_MAX_VOLUME;
			}

			void setPosition(double position_);

			int getActiveChannel() {
				return m_active_channel;
			}


		private:
			int audioLength;
			std::vector<double> audioData;
			std::vector<double> magnitude;
			Mix_Music* m_music = nullptr;

			std::string m_file_path;
			char * m_data = nullptr;
			int m_data_length = 0;
			glm::vec2 m_origin;
			SquareBox::FallOffEnum m_fall_off;
			float m_min_radius;
			float m_max_radius;
			int m_active_channel = -1;
			bool m_is_music_loaded = false;
		};
	};
};