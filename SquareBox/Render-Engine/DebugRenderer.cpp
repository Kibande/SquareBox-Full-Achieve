#include "DebugRenderer.h"
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif
const float PI = 3.1415926536f;
namespace SquareBox {
	namespace RenderEngine {

		DebugRenderer::DebugRenderer()
		{
			//Empty
		}

		DebugRenderer::~DebugRenderer()
		{
			dispose();
		}

		void DebugRenderer::init()
		{
			//Set up buffers

			SBX_GLCall(glGenVertexArrays(1, &m_vao));
			SBX_GLCall(glGenBuffers(1, &m_vbo));
			SBX_GLCall(glGenBuffers(1, &m_ibo));

			SBX_GLCall(glBindVertexArray(m_vao));
			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
			SBX_GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));

			SBX_GLCall(glEnableVertexAttribArray(0));
			SBX_GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position)));
			SBX_GLCall(glEnableVertexAttribArray(1));
			SBX_GLCall(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color)));

			SBX_GLCall(glBindVertexArray(0));
		}

		void DebugRenderer::begin() {
			/*
			I have only put this here for API consistance
			*/
		}
		void DebugRenderer::end()
		{
			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
			//Ophan the m_buffer , this is for optimization
			SBX_GLCall(glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW));
			//Upload the data
			SBX_GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(DebugVertex), m_verts.data()));
			//bind the ibo and upload its data as well
			SBX_GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

			SBX_GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
			//Ophan the m_buffer , this is for optimization
			SBX_GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
			//Upload the data
			SBX_GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(GLuint), m_indices.data()));
			SBX_GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

			//clear our indices and verties in the end
			m_numElements = (unsigned)m_indices.size();
			m_indices.clear();
			m_verts.clear();
		}
		glm::vec2 rotatePoint(glm::vec2 position, float angle)
		{
			glm::vec2 newvec;

			newvec.x = position.x * cos(angle) - position.y * sin(angle);
			newvec.y = position.x * sin(angle) + position.y * cos(angle);

			return newvec;
		}
		void DebugRenderer::drawLine(const glm::vec2& a, const glm::vec2& b, const ColorRGBA8& color) {
			int i = m_verts.size();
			m_verts.resize(m_verts.size() + 2);

			m_verts[i].position = a;
			m_verts[i].color = color;

			m_verts[i + 1].position = b;
			m_verts[i + 1].color = color;

			m_indices.push_back(i);
			m_indices.push_back(i + 1);
		}

		void DebugRenderer::drawBox(const glm::vec4 & destRect, const ColorRGBA8 & color, float angle)
		{
			unsigned i = (unsigned)m_verts.size();//the location of the first vertex we shall be modifiying
			m_verts.resize(m_verts.size() + 4);
			//lets get the vertices that represent our sprite centered at the m_origin
			glm::vec2 halfDims(destRect.z * 0.5f, destRect.w * 0.5f);

			//Get points centered at m_origin
			glm::vec2 top_left(-halfDims.x, halfDims.y);
			glm::vec2 bottom_left(-halfDims.x, -halfDims.y);
			glm::vec2 bottom_right(halfDims.x, -halfDims.y);
			glm::vec2 top_right(halfDims.x, halfDims.y);

			glm::vec2 positionOffset(destRect.x, destRect.y);
			//rotate the points
			//The +halfDims; is to go back to considering the whole external box arounf the agent
			// and not only the center
			m_verts[i].position = rotatePoint(top_left, angle) + halfDims + positionOffset;//first vertex
			m_verts[i + 1].position = rotatePoint(bottom_left, angle) + halfDims + positionOffset;
			m_verts[i + 2].position = rotatePoint(bottom_right, angle) + halfDims + positionOffset;
			m_verts[i + 3].position = rotatePoint(top_right, angle) + halfDims + positionOffset;//fourth vertex

			//set the color
			for (unsigned j = i; j < i + 4; j++)
			{
				m_verts[j].color = color;
			}

			//set up our indices
			m_indices.reserve(m_indices.size() + 8);

			m_indices.push_back(i);
			m_indices.push_back(i + 1);

			m_indices.push_back(i + 1);
			m_indices.push_back(i + 2);

			m_indices.push_back(i + 2);
			m_indices.push_back(i + 3);

			m_indices.push_back(i + 3);
			m_indices.push_back(i);
		}

		void DebugRenderer::drawCapsule(const glm::vec4 & destRect_, const ColorRGBA8 & color, float angle, bool fullCpasule, bool topCapsule)
		{
			glm::vec4 destRect;

			if (fullCpasule) {
				destRect = glm::vec4(destRect_.x, destRect_.y, destRect_.z, destRect_.w - destRect_.z);
			}
			else
			{
				destRect = glm::vec4(destRect_.x, destRect_.y, destRect_.z, destRect_.w - destRect_.z*0.5f);
			}

			glm::vec2 halfDims(destRect.z * 0.5f, destRect.w * 0.5f);

			//Get points centered at m_origin
			glm::vec2 top_left(-halfDims.x, halfDims.y);
			glm::vec2 bottom_left(-halfDims.x, -halfDims.y);
			glm::vec2 bottom_right(halfDims.x, -halfDims.y);
			glm::vec2 top_right(halfDims.x, halfDims.y);

			glm::vec2 positionOffset(destRect.x, destRect.y);

			glm::vec2 top_left_corner = rotatePoint(top_left, angle) + halfDims + positionOffset;//first corner
			glm::vec2 bottom_left_corner = rotatePoint(bottom_left, angle) + halfDims + positionOffset;
			glm::vec2 bottom_right_corner = rotatePoint(bottom_right, angle) + halfDims + positionOffset;
			glm::vec2 top_right_corner = rotatePoint(top_right, angle) + halfDims + positionOffset;//fourth corner

			drawBox(destRect, color, angle);

			if (fullCpasule) {
				//top Circle
				drawCircle(glm::vec2(float(top_left_corner.x + top_right_corner.x) / 2, float(top_left_corner.y + top_right_corner.y) / 2), color, sqrt(pow(top_right_corner.x - top_left_corner.x, 2) + pow(top_right_corner.y - top_left_corner.y, 2)*1.0) * 0.5f);
				//bottom
				drawCircle(glm::vec2(float(bottom_left_corner.x + bottom_right_corner.x) / 2, float(bottom_left_corner.y + bottom_right_corner.y) / 2), color, sqrt(pow(bottom_right_corner.x - bottom_left_corner.x, 2) + pow(bottom_right_corner.y - bottom_left_corner.y, 2)*1.0) * 0.5f);
			}
			else {
				if (topCapsule) {
					//top Circle
					drawCircle(glm::vec2(float(top_left_corner.x + top_right_corner.x) / 2, float(top_left_corner.y + top_right_corner.y) / 2), color, sqrt(pow(top_right_corner.x - top_left_corner.x, 2) + pow(top_right_corner.y - top_left_corner.y, 2)*1.0) * 0.5f);
				}
				else
				{
					//bottom
					drawCircle(glm::vec2(float(bottom_left_corner.x + bottom_right_corner.x) / 2, float(bottom_left_corner.y + bottom_right_corner.y) / 2), color, sqrt(pow(bottom_right_corner.x - bottom_left_corner.x, 2) + pow(bottom_right_corner.y - bottom_left_corner.y, 2)*1.0) * 0.5f);
				}
			}
		}

		void DebugRenderer::drawCircle(const glm::vec2 & center, const ColorRGBA8 & color, float radius)
		{
			static const int NUM_VERTS = 100;//TODO: Add more vertices if its bigger on the screen

			unsigned start = (unsigned)m_verts.size();
			m_verts.resize(m_verts.size() + NUM_VERTS);
			for (int i = 0; i < NUM_VERTS; i++)
			{
				//Some trig
				float angle = ((float)i / NUM_VERTS)*PI*2.0f;//this will give us the angle and radians of each point
				//around the circle
				//getting th actual position
				m_verts[start + i].position.x = cos(angle)* radius + center.x;
				m_verts[start + i].position.y = sin(angle)* radius + center.y;
				m_verts[start + i].color = color;
			}
			//set up indices for indexed drawing
			m_indices.reserve(m_indices.size() + NUM_VERTS * 2);
			for (int i = 0; i < NUM_VERTS - 1; i++)
			{
				m_indices.push_back(start + i);
				m_indices.push_back(start + i + 1);
			}
			m_indices.push_back(start + NUM_VERTS - 1);
			m_indices.push_back(start);
		}

		void DebugRenderer::drawPolygon(const std::vector<glm::vec2>& vertices_, const ColorRGBA8 & color_, float angle_)
		{
			int vecSize = (vertices_.size() - 1);
			if (vecSize < 2)//polygon must have a min of 3 vertices , the size is less than one from above
			{
				vecSize = 0;
			}
			for (auto j = 0; j < vecSize; j++)
			{
				drawLine(m_rotatePoint(vertices_[j], angle_), m_rotatePoint(vertices_[j + 1], angle_), color_);
			}
			//draw the last line from the start to end
			if (vecSize > 0) {
				drawLine(m_rotatePoint(vertices_[0], angle_), m_rotatePoint(vertices_[vecSize], angle_), color_);
			}
		}

		void DebugRenderer::drawEdge(const std::vector<glm::vec2>& vertices_, const ColorRGBA8 & color_, float angle_)
		{
			int vecSize = (vertices_.size() - 1);
			if (vecSize < 1)//line must have a min of 2 vertices
			{
				vecSize = 0;
			}
			for (auto j = 0; j < vecSize; j++)
			{
				drawLine(m_rotatePoint(vertices_[j], angle_), m_rotatePoint(vertices_[j + 1], angle_), color_);
			}
		}

		void DebugRenderer::render(float lineWidth)
		{

			SBX_GLCall(glLineWidth(lineWidth));
			SBX_GLCall(glBindVertexArray(m_vao));
			//the actual draw function
			SBX_GLCall(glDrawElements(GL_LINES, m_numElements, GL_UNSIGNED_INT, 0));

			SBX_GLCall(glBindVertexArray(0));
		}

		void DebugRenderer::dispose()
		{
			if (m_vao) {
				SBX_GLCall(glDeleteVertexArrays(1, &m_vao));
				m_vao = 0;
			}
			if (m_vbo) {
				SBX_GLCall(glDeleteBuffers(1, &m_vbo));
				m_vbo = 0;
			}
			if (m_ibo) {
				SBX_GLCall(glDeleteBuffers(1, &m_ibo));
				m_ibo = 0;
			}
		}
		glm::vec2 DebugRenderer::m_rotatePoint(glm::vec2 position, float angle)
		{
			glm::vec2 newvec;

			newvec.x = position.x * cos(angle) - position.y * sin(angle);
			newvec.y = position.x * sin(angle) + position.y * cos(angle);

			return newvec;
		}
	}
}