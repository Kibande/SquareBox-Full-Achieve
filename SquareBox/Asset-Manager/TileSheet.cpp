#include "TileSheet.h"

namespace SquareBox {
	namespace AssetManager {
		TileSheet::TileSheet()
		{
		}

		TileSheet::~TileSheet()
		{
		}

		//takes in the texture and the nature of the sprite grid system
		void TileSheet::readGrid(const GLTexture& texture) {
			m_texture = texture;
			m_dims = texture.tiling;
		}

		glm::vec4 TileSheet::getUVCords(int index) //returns the UVcordinates of a particular tile
		{
			if (m_dims.x > 0 && m_dims.y > 0) {
				int xTile = index % m_dims.x;
				int yTile = index / m_dims.x;

				glm::vec4 uvs;
				uvs.x = xTile / (float)m_dims.x;
				uvs.y = yTile / (float)m_dims.y;
				uvs.z = 1.0f / m_dims.x;
				uvs.w = 1.0f / m_dims.y;
				return uvs;
			}
			else {
				return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			}
		}
	}
}