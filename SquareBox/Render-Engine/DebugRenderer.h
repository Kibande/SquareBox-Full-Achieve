#pragma once
#include "Vertex.h"
#include <glm/glm.hpp>
#include <vector>
#include "GLSLProgram.h"

namespace SquareBox {
	namespace RenderEngine {
		class DebugRenderer
		{
		public:
			DebugRenderer();
			~DebugRenderer();
			void init();
			void begin();
			void end();
			void drawLine(const glm::vec2& a, const glm::vec2& b, const ColorRGBA8& color);
			void drawBox(const glm::vec4& destRect, const ColorRGBA8& color, float angle);
			void drawCapsule(const glm::vec4& destRect, const ColorRGBA8& color, float angle, bool fullCpasule = true, bool topCapsule = true);
			void drawCircle(const glm::vec2& center, const ColorRGBA8& color, float radius);
			void drawPolygon(const std::vector<glm::vec2> &vertices_, const ColorRGBA8& color_, float angle_);
			void drawEdge(const std::vector<glm::vec2> &vertices_, const ColorRGBA8& color, float angle);
			void render(float lineWidth=2.0f);
			void dispose();
			struct DebugVertex
			{
				glm::vec2 position;
				SquareBox::RenderEngine::ColorRGBA8 color;
			};
		private:
			glm::vec2 m_rotatePoint(glm::vec2 position, float angle);
			std::vector<DebugVertex> m_verts;
			std::vector<GLuint> m_indices;
			GLuint m_vbo = 0, m_vao = 0, m_ibo = 0;
			unsigned m_numElements = 0;
		};
	}
}
