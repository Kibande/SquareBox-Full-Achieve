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

	//create a white texture on the fly
	const int width = 1000;
	const int height = 1000;
	const int length = (width * height) * 4;

	float* data = new float[length];
	int i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			data[i] = 1.0f;
			data[i + 1] = 1.0f;
			data[i + 2] = 1.0f;
			data[i + 3] = 1.0f;
			i += 4;
		}
	}


	//SquareBox::AssetManager::GLTexture m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/white background.png");
	auto  fileBuffer = std::pair<float*, int>(data, length);
	m_texture = SquareBox::AssetManager::TextureManager::createTextureFromFileBuffer(fileBuffer, width, height);

	delete[] data; // since the data is stored on the cpu

}

void SquareBoxExample::AudioSystemExample::onUpdate(float delta_time_, SquareBox::IMainGame* m_game_ptr_, SquareBox::Camera::ParallelCamera& m_parallel_camera)
{
	m_fps = m_game_ptr_->getFps();


	m_screen_width = m_game_ptr_->getWindow()->getScreenWidth();
	m_screen_height = m_game_ptr_->getWindow()->getScreenHeight();

}

void SquareBoxExample::AudioSystemExample::onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont* _spriteFont, SquareBox::RenderEngine::DebugRenderer* _debugRenderer)
{
	int display_window_width = m_screen_width * 0.4;
	int display_window_height = m_screen_height * 0.4;
	glm::vec4 destRect(m_screen_width*0.1, m_screen_height *0.4, display_window_width, display_window_height);
	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);



	_spriteBatch->draw(destRect, uvRect,m_texture.id,1,SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));

	if (m_sound_bank.isLoaded()) {
		auto& active_sound_chunk = m_sound_bank.sound_effects[m_sound_bank.active_sound_effect_index];

		//// Draw time domain graph
		//for (int i = 0; i < display_window_width; ++i) {
		//	
		//	int audioIndex = i * active_sound_chunk.audioLength / display_window_width;
		//	int lineY = display_window_height * active_sound_chunk.audioData[audioIndex] / active_sound_chunk.audioLength;

		//	_spriteBatch->draw(glm::vec4(i, display_window_height - lineY,10,10), uvRect, m_texture.id, 1, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow));

		//}

		// Draw frequency domain graph
		for (int i = 0; i < display_window_width; ++i) {
			int frequencyIndex = i * active_sound_chunk.magnitude.size() / display_window_width;
			int lineY = display_window_height * active_sound_chunk.magnitude[frequencyIndex] / active_sound_chunk.audioLength;
			_spriteBatch->draw(glm::vec4(i, display_window_height - lineY, 10, 10), uvRect, m_texture.id, 1, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green));

		}

	}
}

void SquareBoxExample::AudioSystemExample::imGuiControls(SquareBoxExample::CustomConsole* _console)
{
	//File Exposer

	ImGui::Spacing();

	ImGui::Text("Heart Sound Controls");
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
			ImGui::SliderFloat("Heart Sound Volume", &current_music_volume, 0.0f, 1.0f);
			m_music.setVolume(current_music_volume);
	}
	else {
		ImGui::Text("No Heart Sound File is loaded");
	}
	
	if (ImGui::Button("+ New Heart Sound")) {
		m_show_open_music_file_dialog = true;
	}

	if (m_show_open_music_file_dialog) {
		ImGui::OpenPopup("Open Heart Sound File");
	}

	if (m_file_dialog.showFileDialog("Open Heart Sound File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
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

std::vector<double> SquareBoxExample::AudioSystemExample::fft(const std::vector<double>& data)
{
	int N = data.size();
	std::vector<double> real(N, 0.0);
	std::vector<double> imag(N, 0.0);

	for (int k = 0; k < N; ++k) {
		for (int n = 0; n < N; ++n) {
			double angle = 2 * M_PI * k * n / N;
			real[k] += data[n] * cos(angle);
			imag[k] -= data[n] * sin(angle);
		}
	}

	std::vector<double> magnitude(N, 0.0);
	for (int k = 0; k < N; ++k) {
		magnitude[k] = sqrt(real[k] * real[k] + imag[k] * imag[k]);
	}

	return magnitude;
}
