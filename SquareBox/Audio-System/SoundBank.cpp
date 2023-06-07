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

void SquareBox::AudioSystem::SoundBank::play(int loops, int sound_effect_index_)
{
	if (sound_effects.size() > 0) {
		static std::mt19937 randomEngine(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution<unsigned int> randonIndex(0, sound_effects.size() - 1);
		active_sound_effect_index = sound_effect_index_ == -1 ? randonIndex(randomEngine) : sound_effect_index_;

		static int m_last_sound_effect_channel = 0;
		//We shall randomly select a sound to play
		SoundEffect & sound = sound_effects[active_sound_effect_index];
		m_active_channel = Mix_PlayChannel(-1, sound.m_chunk, loops);
		if (m_active_channel == -1) {
			//if the there was no available channel
			//manually force a channel

			//allocate a new channel
			Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
			m_active_channel = Mix_PlayChannel(-1, sound.m_chunk, loops);
			if (m_active_channel == -1) {//if we have an issue again
				//then throw an error , we really have a problem
				SBX_CORE_ERROR("Failed to play effect audio {} {} ", sound.m_file_path, std::string(Mix_GetError()));
			}
		};

	}
	else {
		SBX_CORE_ERROR("Sound Bank is Empty");
	}
	
}
void SquareBox::AudioSystem::SoundBank::timedPlay(int milliseconds_, int loops, int sound_effect_index_)
{
	if (sound_effects.size() > 0) {
		static std::mt19937 randomEngine(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution<unsigned int> randonIndex(0, sound_effects.size() - 1);
		active_sound_effect_index = sound_effect_index_ == -1 ? randonIndex(randomEngine) : sound_effect_index_;

		static int m_last_sound_effect_channel = 0;
		//We shall randomly select a sound to play
		SoundEffect& sound = sound_effects[active_sound_effect_index];
		m_active_channel = Mix_PlayChannelTimed(-1, sound.m_chunk, loops, milliseconds_);
		if (m_active_channel == -1) {
			//if the there was no available channel
			//manually force a channel

			//allocate a new channel
			Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
			m_active_channel = Mix_PlayChannelTimed(-1, sound.m_chunk, loops, milliseconds_);
			if (m_active_channel == -1) {//if we have an issue again
				//then throw an error , we really have a problem
				SBX_CORE_ERROR("Failed to play effect audio {} {} ", sound.m_file_path, std::string(Mix_GetError()));
			}
		};

	}
	else {
		SBX_CORE_ERROR("Sound Bank is Empty");
	}
}
void SquareBox::AudioSystem::SoundBank::unLoad()
{
	// actual freeing of data is done on dispose of the audio Engine
	stop();
	sound_effects.clear();
	m_is_loaded = false;
}

void SquareBox::AudioSystem::SoundBank::pause()
{
	Mix_Pause(-1);
}

void SquareBox::AudioSystem::SoundBank::resume()
{
	Mix_Resume(-1);
}

void SquareBox::AudioSystem::SoundBank::stop()
{
	Mix_HaltChannel(-1);
}

void SquareBox::AudioSystem::SoundBank::setSoundBankVolume(float volume_percentage_)
{

	for (unsigned i = 0; i < sound_effects.size(); i++)
	{
		SoundEffect& sound = sound_effects[i];
		Mix_VolumeChunk(sound.m_chunk, MIX_MAX_VOLUME * volume_percentage_);

	}
}

void SquareBox::AudioSystem::SoundBank::setSoundEffectVolume(std::string unique_sound_effect_name_, float volume_percentage_)
{
	for (unsigned i = 0; i < sound_effects.size(); i++)
	{
		SoundEffect& sound = sound_effects[i];
		if (sound.m_unique_name == unique_sound_effect_name_) {
			Mix_VolumeChunk(sound.m_chunk, MIX_MAX_VOLUME * volume_percentage_);
			return;
		}
	}
}

void SquareBox::AudioSystem::SoundBank::listSoundEffectDecorders()
{
	unsigned int num_sound_effect_decoders = getNumSoundEffectDecorders();
	for (unsigned int i = 0; i < num_sound_effect_decoders; i++)
	{
		SBX_CORE_INFO("Sound Effect Decorder {} ", Mix_GetChunkDecoder(i));
	}
}
