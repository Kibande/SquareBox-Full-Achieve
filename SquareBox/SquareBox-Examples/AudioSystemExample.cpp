#include "AudioSystemExample.h"

SquareBoxExample::AudioSystemExample::AudioSystemExample()
{
	exampleTitle = "Audio System";
}

void SquareBoxExample::AudioSystemExample::onEntry(SquareBox::RenderEngine::Window* _window)
{
	// set up our audio Engine
	m_input_audio_format_flags = static_cast<int>(SquareBox::AudioInputFormatEnum::MP3_FORMAT);
	m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OGG_FORMAT);
	m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MOD_FORMAT);
	m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MID_FORMAT);
	m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::FLAC_FORMAT);
	m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OPUS_FORMAT);
	m_audio_engine.init(m_input_audio_format_flags, MIX_DEFAULT_FREQUENCY, SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT, SquareBox::AudioChannlesEnum::STEREO, 4096);
}

void SquareBoxExample::AudioSystemExample::onUpdate(float delta_time_, SquareBox::IMainGame* m_game_ptr_, SquareBox::Camera::ParallelCamera& m_parallel_camera)
{
	m_fps = m_game_ptr_->getFps();
}

void SquareBoxExample::AudioSystemExample::onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont* _spriteFont, SquareBox::RenderEngine::DebugRenderer* _debugRenderer)
{

}

void SquareBoxExample::AudioSystemExample::imGuiControls(SquareBoxExample::CustomConsole* _console)
{
	//File Exposer

	ImGui::Spacing();

	ImGui::Text("Music Controls");
	ImGui::Separator();
	if (m_music.isLoaded()) {
	
		if (ImGui::Button("Play")) {
			m_music.play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			m_music.stop();
		}
		ImGui::SameLine();

		if (ImGui::Button("Rewind")) {
			m_music.rewind();
		}

		if (ImGui::Button("unLoad")) {
			m_music.unLoad();
		}
		ImGui::SameLine();

			if (ImGui::Button("Resume")) {
				m_music.resume();
			}
			ImGui::SameLine();

			if (ImGui::Button("Pause")) {
				m_music.pause();
			}
			float current_music_volume = m_music.getVolumeRatio();
			ImGui::SliderFloat("Music Volume", &current_music_volume, 0.0f, 1.0f);
			m_music.setVolume(current_music_volume);
	}
	else {
		ImGui::Text("No Music File is loaded");
	}
	
	if (ImGui::Button("+ New Music")) {
		m_show_open_music_file_dialog = true;
	}

	if (m_show_open_music_file_dialog) {
		ImGui::OpenPopup("Open Music File");
	}

	if (m_file_dialog.showFileDialog("Open Music File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
	{
		m_music.unLoad();
		m_music = SquareBox::AudioSystem::Music(m_file_dialog.selected_path);
		m_audio_engine.loadMusic(m_music);

		
	}
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Sound Effect Controls");
	ImGui::Separator();

	if (m_sound_bank.isLoaded()) {

		ImGui::Text("%d Sound Effects loaded ",m_sound_bank.sound_effects.size());
		ImGui::Spacing();

		if (ImGui::Button("Play_")) {
			m_sound_bank.play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop_")) {
			m_sound_bank.stop();
		}
		ImGui::SameLine();

		
		if (ImGui::Button("unLoad_")) {
			m_sound_bank.unLoad();
		}
	

		if (ImGui::Button("Resume_")) {
			m_sound_bank.resume();
		}
		ImGui::SameLine();

		if (ImGui::Button("Pause_")) {
			m_sound_bank.pause();
		}


		float current_sound_effect_volume = m_sound_bank.getVolumeRatio();
		ImGui::SliderFloat("Sound Effect Volume", &current_sound_effect_volume, 0.0f, 1.0f);
		m_sound_bank.setVolume(current_sound_effect_volume);
	}
	else {
		ImGui::Text("The Sound Bank is not is loaded");
	}
	




	if (ImGui::Button("+ New Sound")) {
		m_show_open_sound_effect_file_dialog = true;
	}

	if (m_show_open_sound_effect_file_dialog) {
		ImGui::OpenPopup("Open Sound Effect File");
	}

	if (m_file_dialog.showFileDialog("Open Sound Effect File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
	{
		m_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect(m_file_dialog.selected_fn, m_file_dialog.selected_path));
		m_audio_engine.loadSoundBank(m_sound_bank);
	}
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::Button("Reset")) {
		reset();
	}
	ImGui::Text("FPS Counter: %.2f", static_cast<float>(m_fps));

	m_show_open_music_file_dialog = false;
	m_show_open_sound_effect_file_dialog = false;

}

void SquareBoxExample::AudioSystemExample::onExit()
{
	m_music.unLoad();
	m_sound_bank.unLoad();
	m_audio_engine.dispose();
}

void SquareBoxExample::AudioSystemExample::reset()
{
}
