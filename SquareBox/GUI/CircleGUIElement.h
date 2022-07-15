#pragma once
#include "IGUIElement.h"
namespace SquareBox {
	namespace GUI {
		class CircleGUIElement : public IGUIElement
		{
		public:
			CircleGUIElement() {
				shape = SquareBox::GUIElementShapeEnum::CIRCLE;
				needs_radius = true;
				needs_height_width = false;
			};

			~CircleGUIElement() {};
			

			// Inherited via IGUIElement
			virtual void drawTexture(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, glm::vec4& visiable_dest_rect_) override;

			virtual void drawText(const SquareBox::GWOM::GUIElement& gui_element_,SquareBox::RenderEngine::SpriteFont & sprite_font_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, float camera_scale_, glm::vec4& visiable_dest_rect_) override;

			virtual bool containtsPoint(const SquareBox::GWOM::GUIElement& gui_element_, const glm::vec2& coordinates_, glm::vec4& visiable_dest_rect_) override;

			virtual void debugDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_, glm::vec4& visiable_dest_rect_) override;

			virtual void traceDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const float cameraScale_, const SquareBox::RenderEngine::ColorRGBA8& borderColor_, glm::vec4 & visible_area_) override;

		};
	};
};

