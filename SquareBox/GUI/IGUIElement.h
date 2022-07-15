#pragma once
#include <SquareBox-Core/enums.h>
#include <GWOM/Entities.h>
#include <Render-Engine/RenderingEngine.h>
#include<glm/gtx/rotate_vector.hpp>

namespace SquareBox {
	namespace GUI {
		class IGUIElement {
		public:
			IGUIElement() {};
			~IGUIElement() {};
			virtual void drawTexture(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, glm::vec4& visiable_dest_rect_) = 0;
			virtual void drawText(const SquareBox::GWOM::GUIElement& gui_element_,SquareBox::RenderEngine::SpriteFont & sprite_font_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_ ,float camera_scale_,glm::vec4& visiable_dest_rect_) = 0;
			virtual bool containtsPoint(const SquareBox::GWOM::GUIElement& gui_element_, const glm::vec2& coordinates_ ,glm::vec4& visiable_dest_rect_) = 0;
			virtual void debugDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_ ,glm::vec4& visiable_dest_rect_) = 0;
			virtual void traceDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const float cameraScale_, const SquareBox::RenderEngine::ColorRGBA8 & borderColor_, glm::vec4& visiable_dest_rect_) = 0;

			bool needs_height_width = false;
			bool needs_radius = false;
			SquareBox::GUIElementShapeEnum shape;

		};
	}
}
