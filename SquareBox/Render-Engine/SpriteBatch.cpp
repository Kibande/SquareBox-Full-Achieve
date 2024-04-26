#include "SpriteBatch.h"

namespace SquareBox {
	namespace RenderEngine {
		Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float Depth, const ColorRGBA8& color) :

			m_texture_id(textureId_), depth(Depth) {
			topLeft.color = color;
			topLeft.setPosition(destRect.x, destRect.y + destRect.w);
			topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x, destRect.y);
			bottomLeft.setUV(uvRect.x, uvRect.y);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
			bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
			topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
		};

		Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float Depth, const ColorRGBA8& color, float angle) :

			m_texture_id(textureId_), depth(Depth) {
			//this is the rotated version

		   //lets get the vertices that represent our sprite centered at the m_origin
			glm::vec2 halfDims(destRect.z * 0.5f, destRect.w * 0.5f);

			//Get points centered at m_origin
			glm::vec2 top_left(-halfDims.x, halfDims.y);
			glm::vec2 bottom_left(-halfDims.x, -halfDims.y);
			glm::vec2 bottom_right(halfDims.x, -halfDims.y);
			glm::vec2 top_right(halfDims.x, halfDims.y);

			//rotate the points
			//The +halfDims; is to go back to considering the whole external box arounf the agent
			// and not only the center
			top_left = rotatePoint(top_left, angle) + halfDims;
			bottom_left = rotatePoint(bottom_left, angle) + halfDims;
			bottom_right = rotatePoint(bottom_right, angle) + halfDims;
			top_right = rotatePoint(top_right, angle) + halfDims;

			topLeft.color = color;
			topLeft.setPosition(destRect.x + top_left.x, destRect.y + top_left.y);
			topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x + bottom_left.x, destRect.y + bottom_left.y);
			bottomLeft.setUV(uvRect.x, uvRect.y);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + bottom_right.x, destRect.y + bottom_right.y);
			bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + top_right.x, destRect.y + top_right.y);
			topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
		};

		void SpriteBatch::dispose() {
			if (_vao != 0) {
				SBX_GLCall(glDeleteVertexArrays(1, &_vao));
				_vao = 0;
			}
			if (_vbo != 0) {
				glDeleteBuffers(1, &_vbo);
				SBX_GLCall(_vbo = 0);
			}
		}

		glm::vec2 Glyph::rotatePoint(glm::vec2 position, float angle)
		{
			glm::vec2 newvec;

			newvec.x = position.x * cos(angle) - position.y * sin(angle);
			newvec.y = position.x * sin(angle) + position.y * cos(angle);

			return newvec;
		}

		SpriteBatch::SpriteBatch() : _vbo(0), _vao(0)
		{
		}

		SpriteBatch::~SpriteBatch()
		{
		}

		void SpriteBatch::init()
		{
			createVertexArray();
		}

		void SpriteBatch::begin(GlyphSortType sortType)
		{
			//set up any state we need to actually start rendering
			//how we want it to sort
			_sortType = sortType;
			_renderBatches.clear();

			_glyphs.clear();
		}

		void SpriteBatch::end()
		{
			_glyphPointers.resize(_glyphs.size());
			//set up app pointer for fast sorting
			for (size_t i = 0; i < _glyphs.size(); i++)
			{
				_glyphPointers[i] = &_glyphs[i];
			}
			sortGlyphs();
			createRenderBatches();
		}

		void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float depth, const ColorRGBA8& color)
		{
			_glyphs.emplace_back(destRect, uvRect, textureId_, depth, color);
		}

		//Rotates the sprite in respect to its angle
		void SpriteBatch::draw(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint textureId_, float depth, const ColorRGBA8 & color, float angle)
		{
			_glyphs.emplace_back(destRect, uvRect, textureId_, depth, color, angle);
		}

		//Rotates the sprite in respect to its direction its going
		void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint textureId_, float depth, const ColorRGBA8& color, const glm::vec2& dir)
		{
			//first compute the angle
			//our default rotation is to the right
			const glm::vec2 right(1.0f, 0.0f);
			//now the dot product
			float angle = acos(glm::dot(right, dir));
			//account for negative angle rotations

			if (dir.y < 0.0f) angle = -angle;
			_glyphs.emplace_back(destRect, uvRect, textureId_, depth, color, angle);
		}

		void SpriteBatch::renderBatch()
		{
			//loop through all of our render batches and draw them all
			SBX_GLCall(glBindVertexArray(_vao));
			for (size_t i = 0; i < _renderBatches.size(); i++)
			{
				SBX_GLCall(glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture_id));
				SBX_GLCall(glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices));
			}
			SBX_GLCall(glBindVertexArray(0));
		}

		void SpriteBatch::createRenderBatches()
		{
			std::vector<Vertex> vertices;
			vertices.resize(_glyphs.size() * 6);//chache friendlyniess
			if (_glyphs.empty()) {
				return;//since there was nothing to give the render batches
			}
			int offset = 0;
			int cv = 0;//current vertex
			_renderBatches.emplace_back(offset, 6, _glyphPointers[0]->m_texture_id);
			vertices[cv++] = _glyphPointers[0]->topLeft;
			vertices[cv++] = _glyphPointers[0]->bottomLeft;
			vertices[cv++] = _glyphPointers[0]->bottomRight;
			vertices[cv++] = _glyphPointers[0]->bottomRight;
			vertices[cv++] = _glyphPointers[0]->topRight;
			vertices[cv++] = _glyphPointers[0]->topLeft;
			offset += 6;
			//loop through all of the rest of the glyphs

			for (size_t cg = 1; cg < _glyphs.size(); cg++)
			{
				if (_glyphPointers[cg]->m_texture_id != _glyphPointers[cg - 1]->m_texture_id) {
					//only create a new render batch if the previous texture is not the same as the current texture
					_renderBatches.emplace_back(offset, 6, _glyphPointers[cg]->m_texture_id);
				}
				else {
					//just increase the previous render batches size
					_renderBatches.back().numVertices += 6;
				}
				vertices[cv++] = _glyphPointers[cg]->topLeft;
				vertices[cv++] = _glyphPointers[cg]->bottomLeft;
				vertices[cv++] = _glyphPointers[cg]->bottomRight;
				vertices[cv++] = _glyphPointers[cg]->bottomRight;
				vertices[cv++] = _glyphPointers[cg]->topRight;
				vertices[cv++] = _glyphPointers[cg]->topLeft;
				offset += 6;
			}
			//upload the vertices to our vertex m_buffer object
			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
			//orphan the m_buffer
			SBX_GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));
			//upload the Data
			SBX_GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));

			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}

		void SpriteBatch::createVertexArray()
		{
			if (_vao == 0) {
				//generate our vertex array object
				SBX_GLCall(glGenVertexArrays(1, &_vao));
			}

			glBindVertexArray(_vao);
			if (_vbo == 0) {
				//generate a vertex m_buffer object
				SBX_GLCall(glGenBuffers(1, &_vbo));
			}
			//bind the m_buffer as a reflenece to vertext array objects binding
			//hence no need to bind the m_buffer manually each time
			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));

		
			//Enable the attributes
			SBX_GLCall(glEnableVertexAttribArray(0));
			SBX_GLCall(glEnableVertexAttribArray(1));
			SBX_GLCall(glEnableVertexAttribArray(2));

			
			//set the attributesw properties
			//The position Attribute pointer
			SBX_GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
			//The color Attribute pointer
			SBX_GLCall(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
			//This is the UV attribute pointer
			SBX_GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv)));

			//unbind the vertex arry object
			SBX_GLCall(glBindVertexArray(0));
		}

		void SpriteBatch::sortGlyphs()
		{
			switch (_sortType)
			{
			case GlyphSortType::FRONT_TO_BACK:
				std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
				break;
			case GlyphSortType::BACK_TO_FRONT:
				std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
				break;
			case GlyphSortType::TEXTURE:
				std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareTexture);
				break;
			default:
				break;
			}
		}

		bool SpriteBatch::compareFrontToBack(Glyph * a, Glyph * b)
		{
			return (a->depth < b->depth);
		}

		bool SpriteBatch::compareBackToFront(Glyph * a, Glyph * b)
		{
			return (a->depth > b->depth);
		}

		bool SpriteBatch::compareTexture(Glyph * a, Glyph * b)
		{
			return (a->m_texture_id < b->m_texture_id);
		}
	}
}