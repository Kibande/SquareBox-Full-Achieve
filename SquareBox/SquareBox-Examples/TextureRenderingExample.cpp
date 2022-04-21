#include "TextureRenderingExample.h"

SquareBox::Example::TextureRenderingExample::TextureRenderingExample()
{
	exampleTitle = "Texture Rendering";
}

SquareBox::Example::TextureRenderingExample::~TextureRenderingExample()
{
}

void SquareBox::Example::TextureRenderingExample::onEntry(SquareBox::RenderEngine::Window * _window)
{
	m_window = _window;
	isExampleInitialised = true;
	//this is where the screens default paramiters are set
	m_texture = SquareBox::RenderEngine::ResourceManager::getTexture("Assets/Textures/glass.png");
	m_width = 100, m_height = 100;
}

void SquareBox::Example::TextureRenderingExample::onUpdate()
{
}

void SquareBox::Example::TextureRenderingExample::onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer)
{
	_spriteBatch->draw(glm::vec4(0, 0, m_width, m_height), uvRect, m_texture.id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
}

void SquareBox::Example::TextureRenderingExample::imGuiControls(CustomAppConsole * _console)
{
	//File Exposer
	if (m_file_dialog.showFileDialog("Open Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png"))
	{
		m_texture = SquareBox::AssetManager::TextureManager::getTexture(m_file_dialog.selected_path);
		m_texture.asset_file_path = m_file_dialog.selected_path;
		m_texture.display_name = m_file_dialog.selected_fn;
	}
	ImGui::Text("Controls");
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
	ImGui::Text("uvRect ");
	ImGui::InputFloat("uv_x", &uvRect.x, 0.01f, 0, "%.2f");
	ImGui::InputFloat("uv_y", &uvRect.y, 0.01f, 0, "%.2f");
	ImGui::InputFloat("uv_z", &uvRect.z, 0.01f, 0, "%.2f");
	ImGui::InputFloat("uv_w", &uvRect.w, 0.01f, 0, "%.2f");
	ImGui::Separator();

	ImGui::Text("Source:"); ImGui::SameLine();
	ImGui::Text(m_texture.asset_file_path.c_str());
	if (ImGui::Button("+ New")) {
		m_showOpenTextureFileDialog = true;
	}
	if (m_showOpenTextureFileDialog) {
		ImGui::OpenPopup("Open Texture File");
	}

	m_showOpenTextureFileDialog = false;
}

void SquareBox::Example::TextureRenderingExample::onExit()
{
}

void SquareBox::Example::TextureRenderingExample::reset()
{
	m_texture = SquareBox::AssetManager::TextureManager::getTexture("Assets/Textures/glass.png");
	m_width = 100, m_height = 100;
}