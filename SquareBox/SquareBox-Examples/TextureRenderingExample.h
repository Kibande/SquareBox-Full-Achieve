#pragma once
#include "IExample.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGui/ImGuiFileBrowser.h>

namespace SquareBox {
	namespace Example {
		class TextureRenderingExample :public SquareBox::Example::IExample
		{
		public:
			TextureRenderingExample();
			~TextureRenderingExample();

			// Inherited via IExample
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) override;

			virtual void onUpdate() override;

			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer) override;

			virtual void imGuiControls(CustomAppConsole * _console) override;

			virtual void onExit() override;

			virtual void reset() override;
		private:
			glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			SquareBox::RenderEngine::Window* m_window;
			SquareBox::RenderEngine::GLTexture m_texture;
			bool m_showOpenTextureFileDialog = false;
			imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
			float m_width, m_height;
		};
	};
};
