#pragma once
#include <string>
#include <map>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include <glm/glm.hpp>
#include <vector>
#include "SoundEffect.h"
#include <SDL/include/SDL_mixer.h>

namespace SquareBox {
	namespace AudioSystem {
		class SoundBank {
		public:
			friend class AudioEngine;
			SoundBank();
			SoundBank(SquareBox::FallOffEnum fall_off_, float min_radius_, float max_radius_,glm::vec2 listerner_location_);
			~SoundBank();
			//Plays the SoundBank
			//@parm loops: if loos==-1 loop forever,otherwise play it loops+1 times
			void play(int loops = 0);
			void stop();
			bool isLoaded()const { m_is_loaded; };
			glm::vec2 orign;
			std::vector<SoundEffect> sound_effects;
		private:
			SquareBox::FallOffEnum m_fall_off;
			float m_min_radius;
			float m_max_radius;
			unsigned int m_last_play_sound_effect_index = 0;
			int m_total_channels = 7;
			bool m_is_loaded = false;
		};
	};
};