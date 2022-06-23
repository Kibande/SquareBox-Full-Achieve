#include "TileSystem.h"

void SquareBox::GWOM::TileSystem::init(float orign_x_, float orign_y_, float width_, float height_, float tile_size_)
{
	if (width_ > 0 && height_ > 0 && tile_size_ > 0) {
			//adopt all the initialization properties
			m_width = width_;
			m_height = height_;
			m_tile_size = tile_size_;

			m_orign_x = orign_x_;
			m_orign_y = orign_y_;
			m_num_x_tiles = (int)std::ceil((float)m_width / m_tile_size);
			m_num_y_tiles = (int)std::ceil((float)m_height / m_tile_size);
			/* We are representing a 2D array as a 1D array */
			m_tiles.resize(m_num_x_tiles * m_num_y_tiles);
			

			int current_tile_index = 0;
			for (auto y = 0; y < m_num_y_tiles; y++)
			{
				for (auto x = 0; x < m_num_x_tiles; x++)
				{
					Tile* tile = getTile(x, y);
					tile->coordinates = std::pair<int, int>(x, y);
					tile->position = glm::vec2(getTileSystemOrigin().x + (x * getTileSize()), getTileSystemOrigin().y + (y * getTileSize())) + (glm::vec2(getTileSize())*0.5f);
					tile->index = current_tile_index;
					current_tile_index += 1;
				}

			}
	}
	else {
		SBX_CORE_ERROR("Invalid Grid Dimensions width : {} height : {} tileSize : {}", width_, height_, tile_size_);
	}
}


void SquareBox::GWOM::TileSystem::init(float orign_x_, float orign_y_, float width_, float height_, float tile_size_ , std::vector<int> & layer_data_)
{
	if (width_ > 0 && height_ > 0 && tile_size_ > 0 && layer_data_.size()>0) {
		

		
			//adopt all the initialization properties
			m_width = width_;
			m_height = height_;
			m_tile_size = tile_size_;

			m_orign_x = orign_x_;
			m_orign_y = orign_y_;
			m_num_x_tiles = (int)std::ceil((float)m_width / m_tile_size);
			m_num_y_tiles = (int)std::ceil((float)m_height / m_tile_size);
			/* We are representing a 2D array as a 1D array */
			m_tiles.resize(m_num_x_tiles * m_num_y_tiles);


			int current_tile_index = 0;
			for (auto y = 0; y < m_num_y_tiles; y++)
			{
				for (auto x = 0; x < m_num_x_tiles; x++)
				{
					Tile* tile = getTile(x, y);
					tile->coordinates = std::pair<int, int>(x, y);
					tile->position = glm::vec2(getTileSystemOrigin().x + (x * getTileSize()), getTileSystemOrigin().y + (y * getTileSize()))+(glm::vec2(getTileSize())*0.5f);
					tile->index = current_tile_index;
					tile->key = layer_data_[current_tile_index];
					if (tile->key != -1) {
						active_tiles.push_back(tile->coordinates);
					}
					current_tile_index += 1;
				}

			}
		
	}
}
void SquareBox::GWOM::TileSystem::resetTileSystem()
{
	m_tiles.clear();
	m_tile_size = 0.0f;
	m_width = 0.0f;
	m_height = 0.0f;
	m_orign_x = 0.0f;
	m_orign_y = 0.0f;
	m_num_y_tiles = 0;
	m_num_x_tiles = 0;
	active_tiles.clear();
	m_is_initialised = false;
	/* we are not reseting the tiling system because the way we set it is external from the whole tiling grid*/
}

SquareBox::GWOM::Tile* SquareBox::GWOM::TileSystem::getTile(const glm::vec2& pos_)
{
	//we should subtract the grid_orign x and y
	int tile_x = static_cast<int>((pos_.x - m_orign_x) / m_tile_size);
	int tile_y = static_cast<int>((pos_.y - m_orign_y) / m_tile_size);

	return getTile(tile_x, tile_y);
}

SquareBox::GWOM::Tile* SquareBox::GWOM::TileSystem::getTile(unsigned x_, unsigned y_)
{
	//just fix in either first or last cell if out of range
	if (x_ >= m_num_x_tiles) x_ = m_num_x_tiles - 1;
	if (y_ >= m_num_y_tiles) y_ = m_num_y_tiles - 1;
	return &m_tiles[y_ * m_num_x_tiles + x_];
}

bool SquareBox::GWOM::TileSystem::isInTileSystem(const glm::vec2 pos_)
{
			const float x_coordinate = pos_.x;
			const float y_coordinate = pos_.y;

			const float  x1 = m_orign_x;
			const float  y1 = m_orign_y;
			const float  x2 = x1 + m_width;
			const float  y2 = y1 + +m_height;

			return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);
}

std::map<int, SquareBox::GWOM::Tile*> SquareBox::GWOM::TileSystem::getAllTilesInDestRect(glm::vec4& dest_rect_,bool only_active_tiles_)
{
	/*
			Because we are using std::ceil, the total virtual tile system tiles arrangement is going to over flow on the right
			and top , since while using dest rect , the increase in size are felt in those directions
		*/
	std::map<int, Tile*> tiles_in_box;
	//figure our which tiles are in our box while looping through all tiles
	float tile_size = getTileSize();
	//replicate the grid layout but inside this box
	int num_x_tiles = (int)std::ceil((float)dest_rect_.z / tile_size);
	int num_y_tiles = (int)std::ceil((float)dest_rect_.w / tile_size);

	for (auto x = 0; x < num_x_tiles; x++) {
		for (auto y = 0; y < num_y_tiles; y++) {

			glm::vec4 destRect(glm::vec2(dest_rect_.x + (x * tile_size), dest_rect_.y + (y * tile_size)), glm::vec2(tile_size));
			//left bottom corner
			auto  left_bottom_tile = getTile(glm::vec2(destRect.x, destRect.y));
			if (only_active_tiles_) {
				if (left_bottom_tile->key != -1) {
					tiles_in_box[left_bottom_tile->index] = left_bottom_tile;
				}
			}
			else {
				tiles_in_box[left_bottom_tile->index] = left_bottom_tile;
			}


			//only do these other checks when they are needed
			if (x == 0 && y == num_y_tiles - 1) {
				//left top corner
				auto  left_top_tile = getTile(glm::vec2(destRect.x, destRect.y + tile_size));
				if (only_active_tiles_) {
					if (left_top_tile->key != -1) {
						tiles_in_box[left_top_tile->index] = left_top_tile;
					}
				}
				else {
					tiles_in_box[left_top_tile->index] = left_top_tile;
				}

			}
			else if ((x == num_x_tiles - 1 && y == num_y_tiles - 1) || (x == num_x_tiles - 1 && y == 0)) {
				//right bottom corner
				auto  right_bottom_tile = getTile(glm::vec2(destRect.x + tile_size, destRect.y));
				if (only_active_tiles_) {
					if (right_bottom_tile->key != -1) {
						tiles_in_box[right_bottom_tile->index] = right_bottom_tile;
					}
				}
				else {
					tiles_in_box[right_bottom_tile->index] = right_bottom_tile;
				}

			}

			if (x == num_x_tiles - 1) {
				//right top corner
				auto  right_top_tile = getTile(glm::vec2(destRect.x + tile_size, destRect.y + tile_size));
				if (only_active_tiles_) {
					if (right_top_tile->key != -1) {
						tiles_in_box[right_top_tile->index] = right_top_tile;
					}
				}
				else {
					tiles_in_box[right_top_tile->index] = right_top_tile;
				}
			}


			if (x >= 0 && x < num_x_tiles && y == num_y_tiles - 1) {
				auto  left_top_tile = getTile(glm::vec2(destRect.x, destRect.y + tile_size));
				if (only_active_tiles_) {
					if (left_top_tile->key != -1) {
						tiles_in_box[left_top_tile->index] = left_top_tile;
					}
				}
				else {
					tiles_in_box[left_top_tile->index] = left_top_tile;
				}
			}
		}
	}
	return tiles_in_box;
}



SquareBox::GWOM::TileSystem::TileSystem()
{
}

SquareBox::GWOM::TileSystem::~TileSystem()
{
}
