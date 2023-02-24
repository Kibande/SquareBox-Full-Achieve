#pragma once
#include<string>
#include<glm/glm.hpp>
#include <SquareBox-Core/Essentials.h>
#include <Render-Engine/ColorRGBA8.h>

namespace SquareBox {
	namespace GWOM {

		class Text {
		public:
			Text() {
				std::strcpy(text, "text\0");
			};
			void setString(std::string  new_string_) {
				std::strcpy(text, new_string_.c_str());
			}
			void setColor(SquareBox::RenderEngine::ColorRGBA8 color_) {
				color = color_.getIVec4();
			}
			void setPosition(float x_, float y_) {
				position = glm::vec2(x_,y_);
			}
			void setScale(float x_, float y_) {
				scale = glm::vec2(x_, y_);
			}
			glm::ivec4 color = glm::ivec4(255, 255, 255, 255);
			glm::vec2 position;
			glm::vec2 scale;
			unsigned int font_index=0;
			char text[100];
			bool show_text = false;
			float text_to_box_height_scale = 1.0f;
			int opacity = 100;
			SquareBox::JustificationEnum justification = SquareBox::JustificationEnum::MIDDLE;
		};
	};
};
