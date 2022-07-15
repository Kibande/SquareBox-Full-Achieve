#pragma once
#include<string>
#include<glm/glm.hpp>
#include <SquareBox-Core/Essentials.h>
namespace SquareBox {
	namespace GWOM {

		class FontInfo {
		public:
			FontInfo() {
				std::strcpy(text, "text\0");
			};
			glm::ivec4 color = glm::ivec4(255, 255, 255, 255);
			int font_index=0;
			char text[20];
			bool show_text = false;
			float text_to_box_height_scale = 1.0f;
			int opacity = 100;
			SquareBox::JustificationEnum justification = SquareBox::JustificationEnum::MIDDLE;
		};
	};
};
