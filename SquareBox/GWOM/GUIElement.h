#pragma once
#include "TextureInfo.h"
#include "FontInfo.h"
#include <glm/glm.hpp>
#include<vector>
namespace SquareBox {
	namespace GWOM {
		class GUIElement {
		public:
			GUIElement() {
				std::strcpy(name, "name\0");
			}
			~GUIElement() {};
			GUIElementShapeEnum shape=SquareBox::GUIElementShapeEnum::CIRCLE;
			GUIElementStateEnum state=SquareBox::GUIElementStateEnum::ACTIVE;
			int id = 1;
			char name[100];
			bool is_hidden = false;
			bool is_locked= false;
			glm::vec2 location_ratio = { 0.0f,0.0f };
			float height_ratio = 0.0f;
			float width_ratio = 0.0f;
			float radius_ratio = 0.0f;
			float angle = 0.0f;
			bool is_alive = false;
			bool draw_perfect_square = false; 
			int perfect_square_dimensions = 0;
			bool is_to_be_deleted = false;
			std::vector<Text> fonts = std::vector<Text>(6, Text());//	the number of members should match the possible states
			std::vector<TextureInfo> textures = std::vector<TextureInfo>(6, TextureInfo());//	the number of members should match the possible states
			//	state count.
		};
	};
};

