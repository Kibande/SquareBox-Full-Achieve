#pragma once
#include "GLTexture.h"
#include <glm/glm.hpp>
namespace SquareBox {
	namespace AssetManager {
		class TileSheet
		{
		public:
			TileSheet();
			~TileSheet();

			//takes in the texture and the nature of the sprite grid system
			void readGrid(const GLTexture& texture);

			glm::vec4 getUVCords(int index);

			GLuint getTextureId()const { return m_texture.id; }
			glm::ivec2 getDimensions() const { return m_dims; }
		private:
			GLTexture m_texture;

			glm::ivec2 m_dims;
		};
	}
}