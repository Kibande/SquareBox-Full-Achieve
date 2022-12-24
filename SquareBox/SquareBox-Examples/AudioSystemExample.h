#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "IExample.h"
#include <ImGUI/imgui.h>
#include <ImGui/ImGuiFileBrowser.h>

namespace SquareBoxExample {

		class AudioSystemExample :public SquareBoxExample::IExample
		{
		public:
			AudioSystemExample();

			// Inherited via IExample
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) override;
			virtual void onUpdate(float delta_time_, SquareBox::IMainGame* m_game_ptr_, SquareBox::Camera::ParallelCamera& m_parallel_camera) override;
			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont* _spriteFont, SquareBox::RenderEngine::DebugRenderer* _debugRenderer) override;
			virtual void imGuiControls(SquareBoxExample::CustomConsole* _console) override;
			virtual void onExit() override;
			virtual void reset() override;

		private:
			bool m_show_open_music_file_dialog = false;
			bool m_show_open_sound_effect_file_dialog = false;

			SquareBox::AudioSystem::AudioEngine m_audio_engine;
			SquareBox::AudioSystem::Music m_music;
			SquareBox::AudioSystem::SoundBank m_sound_bank;
			imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs

		
			float m_fps;
			int m_input_audio_format_flags;
			int m_audio_frequency = 22050;
			SquareBox::AudioOutputFormatEnum m_audio_output_format = SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT;
			SquareBox::AudioChannlesEnum m_audio_channels = SquareBox::AudioChannlesEnum::STEREO;
			int m_audio_buffer_size= 4096;
		};

	};



