#include "VideoPlayerExample.h"

SquareBox::Example::VideoPlayerExample::VideoPlayerExample()
{
	exampleTitle = "Video Player";
}

SquareBox::Example::VideoPlayerExample::~VideoPlayerExample()
{
}

void SquareBox::Example::VideoPlayerExample::onEntry(SquareBox::RenderEngine::Window * _window)
{
	m_window = _window;
	isExampleInitialised = true;
	//this is where the screens default paramiters are set
	m_width = 100, m_height = 100;
	m_filePath = "C:\\SquareBox\\SquareBox\\SquareBox-SandBox\\2067.mp4";
	m_player.init();
}

void SquareBox::Example::VideoPlayerExample::onUpdate()
{
	m_player.decode();
}

void SquareBox::Example::VideoPlayerExample::onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer)
{
	_spriteBatch->draw(glm::vec4(0, 0, m_width, m_height), uvRect, m_texture.id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
}

void SquareBox::Example::VideoPlayerExample::imGuiControls(CustomAppConsole * _console)
{
	//File Expoler
	if (m_file_dialog.showFileDialog("Open Video File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), "*.*"))
	{
		m_filePath = m_file_dialog.selected_path;
	}
	ImGui::Text("Controls");
	if (ImGui::Button("Play")) {
		play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause")) {
		pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		reset();
	}
	ImGui::Separator();
	ImGui::Text("Dimensions");
	ImGui::Text("Height : "); ImGui::SameLine();
	ImGui::InputFloat("###b_h", &m_height, 0.5f, 0, "%.1f");
	ImGui::Text("Width  : "); ImGui::SameLine();
	ImGui::InputFloat("###b_w", &m_width, 0.5f, 0, "%.1f");
	ImGui::Separator();
	static int volume = m_player.getVolumeLevel();
	ImGui::DragInt("Volume", &volume, 1, 0, 100, "%.0f");
	m_player.setVolumeLevel(volume);

	ImGui::Text("Source:"); ImGui::SameLine();
	ImGui::Text(m_filePath.c_str());
	if (ImGui::Button("+ New")) {
		m_showOpenTextureFileDialog = true;
	}
	if (m_showOpenTextureFileDialog) {
		ImGui::OpenPopup("Open Video File");
	}

	m_showOpenTextureFileDialog = false;
}

void SquareBox::Example::VideoPlayerExample::onExit()
{
	m_player.dispose();
}

void SquareBox::Example::VideoPlayerExample::reset()
{
	m_player.setMediaSource(m_filePath, &m_texture, "RV24", isRemoteSource);
	m_width = 100, m_height = 100;
}

void SquareBox::Example::VideoPlayerExample::play()
{
	m_player.play();
}

void SquareBox::Example::VideoPlayerExample::pause()
{
	m_player.pause();
}