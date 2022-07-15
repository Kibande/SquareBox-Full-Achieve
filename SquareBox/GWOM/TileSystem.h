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
			std::pair<int, int> coordinates;
			int key = -1; // tile is empty
		};
		class TileSystem
		{
			/* this is like the grid system but it doesnt store Cluter Objects , just a single element er tile*/
		public:
			TileSystem();
			~TileSystem();
			void init(float orign_x_, float orign_y_, float width_, float height_, float tile_size_);
			void init(float orignX_, float orignY_, float width_, float height_, float tile_size_, std::vector<int> & layer_data_);
			void resetTileSystem();

			Tile * getTile(const glm::vec2& pos_);
			void setTile(const glm::vec2& pos_,int key_) { getTile(pos_)->key = key_; };

			Tile * getTile(unsigned x_,unsigned y_);
			void setTile(unsigned x_, unsigned y_, int key_) { getTile(x_, y_)->key = key_; };

			bool isInTileSystem(const glm::vec2 pos_);
			std::map<int, SquareBox::GWOM::Tile*>  getAllTilesInDestRect(glm::vec4& dest_rect_,bool only_active_tiles_=false);

			///Getters
			unsigned getNumXCells() const { return m_num_x_tiles; }
			unsigned getNumYCells() const { return m_num_y_tiles; }
			float getWidth() const { return m_width; }
			float getHeight() const { return m_height; }
			float getTileSize() const { return m_tile_size; }
			glm::vec2 getTileSystemOrigin() const { return glm::vec2(m_orign_x, m_orign_y); }
			Tile * getTileByIndex(const int index_) { return &m_tiles[index_]; }
			std::vector<Tile>& getAllTileSystemTiles() { return m_tiles; }
			void dispose() { resetTileSystem(); };
			std::vector<Tile> m_tiles;//this is what is actually stored;

			float getOriginX() const { return m_orign_x; }

			float getOriginY() const { return m_orign_y; }

			std::vector<std::pair<int, int>> active_tiles;
		private:
			float m_tile_size = 0.0f;
			float m_width = 0.0f;
			float m_height = 0.0f;
			float m_orign_x = 0.0f;
			float m_orign_y = 0.0f;
			unsigned m_num_y_tiles = 0;
			unsigned m_num_x_tiles = 0;
		};


	}

};

