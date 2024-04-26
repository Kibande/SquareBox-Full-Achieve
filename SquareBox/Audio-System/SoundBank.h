#pragma once
#include <string>
#include <map>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
#include <vector>
#include "SoundEffect.h"
#include <SDL_mixer.h>

namespace SquareBox {
	namespace AudioSystem {
		class SoundBank {
		public:
			friend class AudioEngine;
			SoundBank();
			SoundBank(SquareBox::FallOffEnum fall_off_, float min_radius_=0.1f, float max_radius_ = 1.0f,glm::vec2 listerner_location_=glm::vec2(0.0f));
			~SoundBank();
			//Plays the SoundBank
			//@parm loops: if loos==-1 loop forever,otherwise play it loops+1 times
			void play(int loops = 0, int sound_effect_index_ = -1); // 0 == play the sample once a -1 sound_effect_index means that a random sound effect will be picked
			void timedPlay(int milliseconds_,int loops = 0,int sound_effect_index_ = -1); // 0 == play the sample once
			
			/* Sound fade in functionalty can be added in later if needed*/
			
			void unLoad();
			void pause();
			void resume();
			void stop();
			bool isLoaded()const { return m_is_loaded; };
			void setSoundBankVolume(float volume_percentage_);
			void setSoundEffectVolume(std::string unique_sound_effect_name_, float volume_percentage_);


			unsigned int getNumSoundEffectDecorders() { return Mix_GetNumChunkDecoders(); };
			int getAvaliableSoundEffectMixingChannels() {
				return Mix_AllocateChannels(-1);
			}

			void setNumMixingChannels(int num_channels_) {
				Mix_AllocateChannels(num_channels_);
			}
			void setVolume(float volume_percentage_) {
				Mix_Volume(-1, MIX_MAX_VOLUME * volume_percentage_);
			}
			float getVolumeRatio() {
				return (float)(Mix_Volume(-1,-1) / (float)MIX_MAX_VOLUME);
			}

			void setChannelVolume(int channel_index_, float volume_percentage_) {
				Mix_Volume(channel_index_, MIX_MAX_VOLUME * volume_percentage_);
			}

			void listSoundEffectDecorders();
			int getActiveChannel() {
				return m_active_channel;
			}
			glm::vec2 orign;
			int active_sound_effect_index = 0;
			std::vector<SoundEffect> sound_effects;
		private:
			SquareBox::FallOffEnum m_fall_off;
			float m_min_radius;
			float m_max_radius;
			int m_active_channel = -1;
			bool m_is_loaded = false;

		};
	};
};