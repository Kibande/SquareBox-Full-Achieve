#pragma once
#include "IExample.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include <ImGui/ImGuiFileBrowser.h>

namespace SquareBox {
	namespace Example {
		class VideoPlayerExample :public SquareBox::Example::IExample
		{
		public:
			VideoPlayerExample();
			~VideoPlayerExample();

			// Inherited via IExample
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) override;

			virtual void onUpdate() override;

			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont * _spriteFont, SquareBox::RenderEngine::DebugRenderer * _debugRenderer) override;

			virtual void imGuiControls(CustomAppConsole * _console) override;

			virtual void onExit() override;

			virtual void reset() override;

			void play();
			void pause();
		private:
			glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			SquareBox::RenderEngine::Window* m_window;
			SquareBox::RenderEngine::GLTexture m_texture;
			SquareBox::MediaPlayer::VLCplayer m_player;
			bool isRemoteSource = false;//we are not loading any urls in this example yet
			bool m_showOpenTextureFileDialog = false;
			std::string m_filePath;
			imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
			float m_width, m_height;
		};
	};
};
