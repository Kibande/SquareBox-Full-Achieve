#pragma once
#include "GUIElement.h"
#include <Asset-Manager/Asset-Manager.h>
#include <Render-Engine/SpriteFont.h>
namespace SquareBox {
	namespace GWOM {
		class GuiLayer {
		public:
			std::vector<GUIElement> gui_elements;
			std::vector<SquareBox::AssetManager::GLTexture> single_textures;
			std::vector<SquareBox::AssetManager::GLTexture> tiled_textures;
			std::vector<SquareBox::RenderEngine::SpriteFont> sprite_fonts;
			glm::vec2 editing_screen_dimensions;
			int orientation = 0; // 0 - landspace || 1 - Portriat
		};
	}
}
