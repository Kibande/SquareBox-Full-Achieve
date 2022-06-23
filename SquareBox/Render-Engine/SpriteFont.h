#pragma once
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <SDL/include/SDL_ttf.h>
#include "Vertex.h"
#include "SpriteBatch.h"
#include <SDL/include/SDL.h>
#include <SquareBox-Logger/Logger.h>
#include <SquareBox-Core/enums.h>
namespace SquareBox {
	namespace RenderEngine {
		struct CharGlyph {
			char character;
			glm::vec4 uvRect;
			glm::vec2 size;
		};

#define FIRST_PRINTABLE_CHAR ((char)32)
#define LAST_PRINTABLE_CHAR ((char)126)


		class SpriteFont
		{
		public:
			SpriteFont();
			void init(const char* font, int size);
			void init(const char* font, int size, char cs, char ce);

			//Destory the font resources
			void dispose();

			int getFontHeight() const {
				return _fontHeight;
			}
			//Measures the dimensions of the text
			glm::vec2 measure(const char* s);

			//Draw using a spirteBatch
			void draw(SpriteBatch& batch, const char* s, glm::vec2 position, glm::vec2 scaling, float depth, ColorRGBA8 tint, SquareBox::JustificationEnum justification_ = SquareBox::JustificationEnum::LEFT);
			~SpriteFont();

		private:
			bool m_is_initialised = false;
			static std::vector<int>* createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w);

			int _regStart, _regLength;
			CharGlyph* _glyphs;
			int _fontHeight;
			unsigned int _texID;
		};
	}
}
