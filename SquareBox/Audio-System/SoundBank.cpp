#include "SoundBank.h"
#include <random>
SquareBox::AudioSystem::SoundBank::SoundBank()
{
}
SquareBox::AudioSystem::SoundBank::SoundBank(SquareBox::FallOffEnum fall_off_, float min_radius_, float max_radius_, glm::vec2 listerner_location_)
{
	m_fall_off = fall_off_;
	m_min_radius = min_radius_;
	m_max_radius = max_radius_;
}

SquareBox::AudioSystem::SoundBank::~SoundBank()
{
}

void SquareBox::AudioSystem::SoundBank::play(int loops)
{
	
	if (sound_effects.size() > m_last_play_sound_effect_index) {
		static std::mt19937 randomEngine(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution<unsigned int> randonIndex(0, sound_effects.size() - 1);

		static int m_last_sound_effect_channel = 0;
		//We shall randomly select a sound to play
		m_last_play_sound_effect_index = randonIndex(randomEngine);
		SoundEffect & sound = sound_effects[m_last_play_sound_effect_index];

		if (Mix_PlayChannel(-1, sound.m_chunk, loops) == -1) {
			//if the there was no available channel
			//manually force a channel

			//this is not a very good solution as it causes the AudioEngine to chock
			if (m_last_sound_effect_channel < m_total_channels) {
				m_last_sound_effect_channel++;
			}
			else {
				m_last_sound_effect_channel = 0;
			}
			if (Mix_PlayChannel(m_last_sound_effect_channel, sound.m_chunk, loops) == -1) {//if we have an issue again
				//then throw an error , we really have a problem
				SBX_CORE_ERROR("Failed to play effect audio {} {} ", sound.m_file_path, std::string(Mix_GetError()));
			}
		};

	}
	else {
		SBX_CORE_ERROR("Sound Bank is Empty");
	}
	
}
void SquareBox::AudioSystem::SoundBank::stop()
{

	if (sound_effects.size() > m_last_play_sound_effect_index) {
		SoundEffect & sound = sound_effects[m_last_play_sound_effect_index];
		if (sound.m_chunk != nullptr) {
			Mix_FreeChunk(sound.m_chunk);
		}
	}
	
}