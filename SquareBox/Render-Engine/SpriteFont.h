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
			void init(std::string font_file_, int size);
			void init(std::string font_file_, int size, char cs, char ce);
			void setProperties(std::string font_file_, std::string font_display_name_, int font_size_);
			//Destory the font resources
			void dispose();

			int getFontHeight() const {
				return _fontHeight;
			}

			int getFontSize() const {
				return m_font_size;
			}

			std::string getFontFilePath() const {
				return m_font_path;
			}
			void setDisplayName(std::string display_name_) {
				m_display_name = display_name_;
			}

			std::string getDisplayName() const {
				return m_display_name;
			}
			//Measures the dimensions of the text
			glm::vec2 measure(const char* s);

			//Draw using a spirteBatch
			void draw(SpriteBatch& batch, const char* s, glm::vec2 coordinates_, glm::vec2 scaling, float depth, ColorRGBA8 tint, SquareBox::JustificationEnum justification_ = SquareBox::JustificationEnum::LEFT);
			~SpriteFont();

		private:
			bool m_is_initialised = false;
			static std::vector<int>* createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w);
			int _regStart, _regLength;
			CharGlyph* _glyphs;
			/*
				fonts having glyphs makes us having a GLTexture like class hard because there is that clean up needed
				and fonts files are way bigger than texures, they are like a batch of texures
			*/
			int _fontHeight;
			unsigned int _texID;
			int m_font_size = 0;
			std::string m_font_path;
			std::string m_display_name;
		};
	}
}
