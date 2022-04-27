#pragma once
#include <glm/glm.hpp>
#include <SquareBox-Core/Essentials.h>
#include <SquareBox-Logger/Logger.h>
#include<algorithm>
#include <vector>
#include <map>
namespace SquareBox {
	namespace GWOM {
		struct Tile {
			int index = -1;
			glm::vec2 position;
			int key = -1; // tile is empty
		};
		class TileSystem
		{
			/* this is like the grid system but it doesnt store Cluter Objects , just a single element er tile*/
		public:
			TileSystem();
			~TileSystem();
		
			void init(float orign_x_, float orign_y_, float width_, float height_, float tile_size_);
			void init(float orignX_, float orignY_, float width_, float height_, float tile_size_, std::vector<std::vector<int>> & layer_data_);
			void resetTileSystem();
			Tile * getTile(const glm::vec2& pos_);
			Tile * getTile(unsigned row_,unsigned col_);
			bool isInTileSystem(const glm::vec2 pos_);
			std::map<int, SquareBox::GWOM::Tile*>  getAllTilesInBox(glm::vec4& dest_rect_);

			///Getters
			int getNumXCells() const { return m_num_rows; }
			int getNumYCells() const { return m_num_cols; }
			float getWidth() const { return m_width; }
			float getHeight() const { return m_height; }
			float getTileSize() const { return m_tile_size; }
			glm::vec2 getTileSystemOrigin() const { return glm::vec2(m_orign_x, m_orign_y); }

			std::vector<std::vector<Tile>>& getTileSystemTiles() { return m_tiles; }
			bool isInitialised() const { return m_is_initialised; }
			void dispose() {};
			std::vector<std::vector<Tile>> m_tiles;//this is what is actually stored;

			float getOriginX() const { return m_orign_x; }

			float getOriginY() const { return m_orign_y; }

			void setTiling(SquareBox::LayerTilingEnum tiling_) {

				m_tiling = tiling_;
			}

			SquareBox::LayerTilingEnum getTiling() const {
				return m_tiling;
			}
		private:
			float m_tile_size = 0.0f;
			float m_width = 0.0f;
			float m_height = 0.0f;
			float m_orign_x = 0.0f;
			float m_orign_y = 0.0f;
			unsigned m_num_rows = 0;
			unsigned m_num_cols = 0;
			bool m_is_initialised = false;
			SquareBox::LayerTilingEnum m_tiling = SquareBox::LayerTilingEnum::None;
		};


	}

};
