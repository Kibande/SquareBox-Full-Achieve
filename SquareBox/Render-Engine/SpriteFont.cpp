#include "SpriteFont.h"
#include <SquareBox-Logger/Logger.h>
int closestPow2(int i) {
	i--;
	int pi = 1;
	while (i > 0)
	{
		i >>= 1;
		pi <<= 1;
	}
	return pi;
}

#define MAX_TEXTURE_RES 4096 //must be a power of two

namespace SquareBox {
	namespace RenderEngine {
		void SpriteFont::initWithFilePath(std::string font_file_, int size_) {
			initWithFilePath(font_file_, size_, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR);
		}

		void SpriteFont::initWithName(std::string font_name_, int size_) {
			std::string relative_path;
			relative_path = "Assets/Fonts/" + font_name_ + ".ttf";
			initWithFilePath(relative_path, size_, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR);
		}

		void SpriteFont::initWithName(std::string font_name_, int size_, char cs, char ce) {
			std::string relative_path;
			relative_path = "Assets/Fonts/" + font_name_ + "ttf";
			initWithFilePath(relative_path, size_, cs, ce);
		}
		// the font size considers a camera scale of 1
		void SpriteFont::initWithFilePath(std::string font_file_, int size_, char cs, char ce) {
			m_font_size = size_;
			// Initialize SDL_ttf
			if (!TTF_WasInit()) {
				TTF_Init();
			}
			m_font_path = font_file_;
			TTF_Font* f = TTF_OpenFontRW(SDL_RWFromFile(m_font_path.c_str(), "rb"), 1, m_font_size);
			if (f == nullptr) {
				SBX_CORE_CRITICAL("Failed to open TTF font {} ", m_font_path);
				return;
			}
			_fontHeight = TTF_FontHeight(f);
			_regStart = cs;
			_regLength = ce - cs + 1;
			int padding = m_font_size / 8;

			// First measure all the regions
			glm::ivec4* glyphRects = new glm::ivec4[_regLength];
			int i = 0, advance;
			for (char c = cs; c <= ce; c++) {
				TTF_GlyphMetrics(f, c, &glyphRects[i].x, &glyphRects[i].z, &glyphRects[i].y, &glyphRects[i].w, &advance);
				glyphRects[i].z -= glyphRects[i].x;
				glyphRects[i].x = 0;
				glyphRects[i].w -= glyphRects[i].y;
				glyphRects[i].y = 0;
				i++;
			}

			// Find best partitioning of glyphs
			int rows = 1, w, h, bestWidth = 0, bestHeight = 0, area = MAX_TEXTURE_RES * MAX_TEXTURE_RES, bestRows = 0;
			std::vector<int>* bestPartition = nullptr;
			while (rows <= _regLength) {
				h = rows * (padding + _fontHeight) + padding;
				auto gr = createRows(glyphRects, _regLength, rows, padding, w);

				// Desire a power of 2 texture
				w = closestPow2(w);
				h = closestPow2(h);

				// A texture must be feasible
				if (w > MAX_TEXTURE_RES || h > MAX_TEXTURE_RES) {
					rows++;
					delete[] gr;
					continue;
				}

				// Check for minimal area
				if (area >= w * h) {
					if (bestPartition) delete[] bestPartition;
					bestPartition = gr;
					bestWidth = w;
					bestHeight = h;
					bestRows = rows;
					area = bestWidth * bestHeight;
					rows++;
				}
				else {
					delete[] gr;
					break;
				}
			}

			// Can a bitmap font be made?
			if (!bestPartition) {
				SBX_CORE_CRITICAL("Failed to Map TTF font {} to texture. Try lowering resolution.\n", m_font_path);
				return;
			}
			// Create the texture

			SBX_GLCall(glGenTextures(1, &_texID));
			SBX_GLCall(glBindTexture(GL_TEXTURE_2D, _texID));
			SBX_GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bestWidth, bestHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

			// Now draw all the glyphs
			SDL_Color fg = { 255, 255, 255, 255 };
			int ly = padding;
			for (int ri = 0; ri < bestRows; ri++) {
				int lx = padding;
				for (size_t ci = 0; ci < bestPartition[ri].size(); ci++) {
					int gi = bestPartition[ri][ci];

					SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(f, (char)(cs + gi), fg);

					// Pre-multiplication occurs here
					unsigned char* sp = (unsigned char*)glyphSurface->pixels;
					int cp = glyphSurface->w * glyphSurface->h * 4;
					for (int i = 0; i < cp; i += 4) {
						float a = sp[i + 3] / 255.0f;
						sp[i] = (unsigned char)((float)sp[i] * a);
						sp[i + 1] = sp[i];
						sp[i + 2] = sp[i];
					}

					// Save glyph image and update coordinates
					SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, lx, bestHeight - ly - 1 - glyphSurface->h, glyphSurface->w, glyphSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, glyphSurface->pixels));
					glyphRects[gi].x = lx;
					glyphRects[gi].y = ly;
					glyphRects[gi].z = glyphSurface->w;
					glyphRects[gi].w = glyphSurface->h;

					SDL_FreeSurface(glyphSurface);
					glyphSurface = nullptr;

					lx += glyphRects[gi].z + padding;
				}
				ly += _fontHeight + padding;
			}

			// Draw the unsupported glyph
			int rs = padding - 1;
			int* pureWhiteSquare = new int[rs * rs];
			memset(pureWhiteSquare, 0xffffffff, rs * rs * sizeof(int));
			SBX_GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rs, rs, GL_RGBA, GL_UNSIGNED_BYTE, pureWhiteSquare));
			delete[] pureWhiteSquare;
			pureWhiteSquare = nullptr;

			// Set some texture parameters
			SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			SBX_GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

			// Create spriteBatch glyphs
			_glyphs = new CharGlyph[_regLength + 1];
			for (i = 0; i < _regLength; i++) {
				_glyphs[i].character = (char)(cs + i);
				_glyphs[i].size = glm::vec2(glyphRects[i].z, glyphRects[i].w);
				_glyphs[i].uvRect = glm::vec4(
					(float)glyphRects[i].x / (float)bestWidth,
					(float)glyphRects[i].y / (float)bestHeight,
					(float)glyphRects[i].z / (float)bestWidth,
					(float)glyphRects[i].w / (float)bestHeight
				);
			}
			_glyphs[_regLength].character = ' ';
			_glyphs[_regLength].size = _glyphs[0].size;
			_glyphs[_regLength].uvRect = glm::vec4(0, 0, (float)rs / (float)bestWidth, (float)rs / (float)bestHeight);

			SBX_GLCall(glBindTexture(GL_TEXTURE_2D, 0));
			delete[] glyphRects;
			delete[] bestPartition;
			m_is_initialised = true;
			TTF_CloseFont(f);
		}
		void SpriteFont::setProperties(std::string font_file_, std::string font_display_name_, int font_size_)
		{
			m_font_size = font_size_;
			m_font_path=font_file_;
			m_display_name=font_display_name_;
		}
		SpriteFont::SpriteFont()
		{
		}
		SpriteFont::~SpriteFont()
		{
		}

		void SpriteFont::dispose()
		{
			if (!m_is_initialised) return;
			if (_texID != 0) {
				SBX_GLCall(glDeleteTextures(1, &_texID));
				_texID = 0;
			}

			if (_glyphs) {
				delete[] _glyphs;
				_glyphs = nullptr;
			}
			m_is_initialised = false;
		}

		glm::vec2 SpriteFont::measure(const char * s)
		{
			glm::vec2 size(0, _fontHeight);
			float cw = 0;
			for (int si = 0; s[si] != 0; si++)
			{
				char c = s[si];
				if (s[si] == '\n') {
					size.y += _fontHeight;
					if (size.x < cw)
						size.x = cw;
					cw = 0;
				}
				else {
					//Check for correct glyph
					int gi = c - _regStart;
					if (gi < 0 || gi >= _regLength)
						gi = _regLength;
					cw += _glyphs[gi].size.x;
				}
			}
			if (size.x < cw)
				size.x = cw;
			return size;
		}

		void SpriteFont::draw(SpriteBatch & batch, const char * c_str_, glm::vec2 coordinates_, glm::vec2 camera_to_font_height_scaling_, float depth, ColorRGBA8 tint, SquareBox::JustificationEnum justification_)
		{
			if (!m_is_initialised) return;

			glm::vec2 tp = coordinates_;
			//Apply justification
			if (justification_ == SquareBox::JustificationEnum::MIDDLE)
			{
				tp.x -= measure(c_str_).x*camera_to_font_height_scaling_.x / 2;
			}
			else if (justification_ == SquareBox::JustificationEnum::RIGHT) {
				tp.x -= measure(c_str_).x* camera_to_font_height_scaling_.x;
			}

			for (int si = 0; c_str_[si] != 0; si++)
			{
				char c = c_str_[si];
				if (c_str_[si] == '\n')
				{
					tp.y += _fontHeight * camera_to_font_height_scaling_.y;
					tp.x = coordinates_.x;
				}
				else
				{
					//Check for correct glyph
					int gi = c - _regStart;
					if (gi < 0 || gi >= _regLength)
						gi = _regLength;
					glm::vec4 destRect(tp, _glyphs[gi].size* camera_to_font_height_scaling_);
					batch.draw(destRect, _glyphs[gi].uvRect, _texID, depth, tint);
					tp.x += _glyphs[gi].size.x* camera_to_font_height_scaling_.x;
				}
			}
		}

		std::vector<int>* SpriteFont::createRows(glm::ivec4 * rects, int rectsLength, int r, int padding, int & w)
		{
			//Blank Initialize
			std::vector<int>* l = new std::vector<int>[r]();
			int* cw = new int[r]();
			for (int i = 0; i < r; i++)
			{
				cw[i] = padding;
			}

			//Loop thought all glyphs
			for (int i = 0; i < rectsLength; i++)
			{
				//Find row for placement
				int ri = 0;
				for (int rii = 1; rii < r; rii++)
					if (cw[rii] < cw[ri]) ri = rii;

				//add Width to the row
				cw[ri] += rects[i].z + padding;

				//Add glyphs to the row list
				l[ri].push_back(i);
			}

			//Find the max width
			w = 0;
			for (int i = 0; i < r; i++)
			{
				if (cw[i] > w)w = cw[i];
			}
			return l;
		}
	}
}