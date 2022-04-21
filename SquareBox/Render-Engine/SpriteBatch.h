#pragma once
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif

#include "Vertex.h"
#include <glm/glm.hpp>

#include <vector>
#include <algorithm>
#include<iostream>
namespace SquareBox {
	namespace RenderEngine {
		enum  GlyphSortType {
			NONE,
			FRONT_TO_BACK,
			BACK_TO_FRONT,
			TEXTURE
		};

		class Glyph {
			//a single sprite
			//hold the sprite info
		public:

			Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float Depth, const ColorRGBA8& color);
			Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float Depth, const ColorRGBA8& color, float angle);
			GLuint m_texture_id;
			float depth;
			Vertex topLeft;
			Vertex bottomLeft;
			Vertex topRight;
			Vertex bottomRight;
		private:
			glm::vec2 rotatePoint(glm::vec2 position, float angle);
		};

		class RenderBatch {
			//this class will tell the draw function were to start from each time its called
			// a RenderBatch is only created when the previous texture in a grouped draw call to spritebatch is not identical to the
			//cureent texture

			//so a render batch is a group of similar textures , the numVertices of an instance of RenderBatch divided by 6
			//(1-6 , represent 3 corners from the two triangles used to draw the sprite) gives u the number of similar textures that
			//make up that render batch

		public:
			RenderBatch(GLuint Offset, GLuint NumVertices, GLuint TextureId_) :offset(Offset), numVertices(NumVertices), texture_id(TextureId_) {
			};
			GLuint offset;
			GLuint numVertices;
			GLuint texture_id;
		};

		class SpriteBatch
		{
		public:
			SpriteBatch();
			~SpriteBatch();

			void init();
			void dispose();

			void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
			void end();
			//Add a glyph to the spriteBatch
			void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float depth, const ColorRGBA8& color);

			void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float depth, const ColorRGBA8& color, float angle);

			void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float depth, const ColorRGBA8& color, const glm::vec2& dir);

			void renderBatch();

		private:
			void createRenderBatches();
			void createVertexArray();
			void sortGlyphs();
			GLuint _vbo;
			GLuint _vao;

			static bool compareFrontToBack(Glyph * a, Glyph* b);
			static bool compareBackToFront(Glyph * a, Glyph* b);

			static bool compareTexture(Glyph * a, Glyph* b);

			std::vector<Glyph*> _glyphPointers; ////this is for sorting
			std::vector<Glyph> _glyphs; //These are the actual glyphs
			GlyphSortType _sortType;
			std::vector<RenderBatch> _renderBatches;
		};
	}
}
