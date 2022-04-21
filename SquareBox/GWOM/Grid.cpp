#include "Grid.h"
namespace SquareBox {
	namespace GWOM {
		Grid::Grid()
		{
		}

		void Grid::init(float origin_x_, float origin_y_, float width_, float height_, float cell_size_)
		{
			if (width_ > 0 && height_ > 0 && cell_size_ > 0) {
				m_width = width_;
				m_height = height_;
				m_cell_size = cell_size_;

				orign_x = origin_x_;
				orign_y = origin_y_;
				m_num_x_cells = (int)std::ceil((float)m_width / m_cell_size);
				m_num_y_cells = (int)std::ceil((float)m_height / m_cell_size);

				// Allocate all the cells
				/* We are representing a 2D array as a 1D array */
				m_cells.resize(m_num_y_cells * m_num_x_cells);
				//allocate all the Cells
				const int MEMBERS_TO_RESERVE = 50;
				int current_cell_index = 0;
				for (auto x = 0; x < m_num_x_cells; x++)
				{
					for (auto y = 0; y < m_num_y_cells; y++)
					{
						SquareBox::GWOM::Cell* current_cell = getCell(x, y);

						current_cell->member_cordinates.reserve(MEMBERS_TO_RESERVE);
						current_cell->position = glm::vec2(getGridOrigin().x + (x * getCellSize()), getGridOrigin().y + (y * getCellSize()));
						current_cell->index = current_cell_index;
						current_cell_index += 1;
					}

				}
				m_is_initialised = true;
			}
			else {
				SBX_CORE_ERROR("Invalid Grid Dimensions width : {} height : {} cellSize : {}",width_,height_,cell_size_);
			}
			
		}



		void Grid::resetGrid()
		{
			m_cells.clear();
			m_cell_size = 0.0f;
			m_width = 0.0f;
			m_height = 0.0f;
			orign_x = 0.0f;
			orign_y = 0.0f;
			m_num_x_cells = 0;
			m_num_y_cells = 0;
			m_is_initialised = false;
		}

		Grid::~Grid()
		{
		}

		void Grid::addObject(SquareBox::GWOM::ClusterObject& cluster_object_)
		{
			//get the Cell the clusterObject belongs to
			Cell* cell = getCell(cluster_object_.position);
			if (cell != nullptr) {
				addObject(cluster_object_, cell);
			}
			else {
				SBX_ERROR("Invalid Cell posiion {} {} ", cluster_object_.position.x, cluster_object_.position.y);
			}

		}

		void Grid::addObject(SquareBox::GWOM::ClusterObject& cluster_object_, Cell* cell_)
		{
			if (cell_ != nullptr) {
				//all the clusterObjects Coordinates to that cell
				cell_->member_cordinates.push_back(std::pair<int, int>(cluster_object_.cluster_index, cluster_object_.index));

				//give the clusterObject Knowledge about which cell it is
				cluster_object_.associated_pointer = cell_;
				//cluster_object_.pod_int = cell_->member_cordinates.size() - 1;
			}
			else {
				SBX_ERROR("Cell pointer was null ! ");
			}
		}

		void Grid::removeObject(SquareBox::GWOM::ClusterObject& cluster_object_)
		{
			//get the cell and remove it from its cell
			Cell* cell = (Cell*)cluster_object_.associated_pointer;

			if (cell != nullptr)
			{
				//remove it from the cell its currently in
				for (auto it = cell->member_cordinates.begin(); it != cell->member_cordinates.end();)
				{
					if ((*it) == std::pair<int, int>(cluster_object_.cluster_index, cluster_object_.index)) {
						it = cell->member_cordinates.erase(it);
						cluster_object_.associated_pointer = nullptr;
						break;
					}
					++it;
				}
			}
			else {
				SBX_ERROR("ClusterObject {} Cell pointer was null ! ", cluster_object_.name);
			}

		}

		Cell* Grid::getCell(int x_, int y_)
		{
			//just fix in either first or last cell if things refuse
			if (x_ < 0) x_ = 0;
			if (x_ >= m_num_x_cells) x_ = m_num_x_cells - 1;
			if (y_ < 0) y_ = 0;
			if (y_ >= m_num_y_cells) y_ = m_num_y_cells - 1;
			//if (x_<0 || x_>m_num_x_cells-1) {
			//	return nullptr;
			//}
			//else if (y_<0 || y_>m_num_y_cells-1) {
			//	return nullptr;
			//}

			return &m_cells[y_ * m_num_x_cells + x_];
		}

		Cell* Grid::getCell(const glm::vec2& pos_)
		{
			//we should subtract the grid_orign x and y
			int cell_x = static_cast<int>((pos_.x - orign_x) / m_cell_size);
			int cell_y = static_cast<int>((pos_.y - orign_y) / m_cell_size);

			return getCell(cell_x, cell_y);
		}
		bool Grid::isInGrid(const glm::vec2 pos_)
		{
			const float x_coordinate = pos_.x;
			const float y_coordinate = pos_.y;

			const float  x1 = orign_x;
			const float  y1 = orign_y;
			const float  x2 = x1 + m_width;
			const float  y2 = y1 + +m_height;

			return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);
		}

		std::map<int, SquareBox::GWOM::Cell*> Grid::getAllCellsInBox(glm::vec4& dest_rect_)
		{
			//figure our which cells are in our box while looping through all cells
			float tile_size = getCellSize();
			//replicate the grid layout but inside this box
			int num_x_cells = (int)std::ceil((float)dest_rect_.z / tile_size);
			int num_y_cells = (int)std::ceil((float)dest_rect_.w / tile_size);

			std::map<int, SquareBox::GWOM::Cell*> cells_in_box;
			for (auto x = 0; x < num_x_cells; x++) {
				for (auto y = 0; y < num_y_cells; y++) {

					glm::vec4 destRect(glm::vec2(dest_rect_.x + (x * tile_size), dest_rect_.y + (y * tile_size)), glm::vec2(tile_size));
					//left bottom corner
					auto  left_bottom_cell = getCell(glm::vec2(destRect.x, destRect.y));
					cells_in_box[left_bottom_cell->index] = left_bottom_cell;



					//only do these other checks when they are needed

					if (x == 0 && y == num_y_cells - 1) {
						//left top corner
						auto  left_top_cell = getCell(glm::vec2(destRect.x, destRect.y + tile_size));
						cells_in_box[left_top_cell->index] = left_top_cell;
					}
					else if ((x == num_x_cells - 1 && y == num_y_cells - 1) || (x == num_x_cells - 1 && y == 0)) {
						//right bottom corner
						auto  right_bottom_cell = getCell(glm::vec2(destRect.x + tile_size, destRect.y));
						cells_in_box[right_bottom_cell->index] = right_bottom_cell;
					}

					if (x == num_x_cells - 1) {
						//right bottom corner
						auto  right_top_cell = getCell(glm::vec2(destRect.x + tile_size, destRect.y + tile_size));
						cells_in_box[right_top_cell->index] = right_top_cell;
					}


					if (x >= 0 && x < num_x_cells && y == num_y_cells - 1) {
						auto  left_top_cell = getCell(glm::vec2(destRect.x, destRect.y + tile_size));
						cells_in_box[left_top_cell->index] = left_top_cell;
					}

				}
			}
		
			return cells_in_box;
		}

		std::map<int, SquareBox::GWOM::Cell*> Grid::getPopulatedCellsInBox(glm::vec4& dest_rect_)
		{
			//figure our which cells are in our box while looping through all cells
			float tile_size = getCellSize();
			//replicate the grid layout but inside this box
			int num_x_cells = (int)std::ceil((float)dest_rect_.z / tile_size);
			int num_y_cells = (int)std::ceil((float)dest_rect_.w / tile_size);

			std::map<int, SquareBox::GWOM::Cell*> cells_in_box;
			for (auto x = 0; x < num_x_cells; x++) {
				for (auto y = 0; y < num_y_cells; y++) {

					glm::vec4 destRect(glm::vec2(dest_rect_.x + (x * tile_size), dest_rect_.y + (y * tile_size)), glm::vec2(tile_size));
					//left bottom corner
					auto  left_bottom_cell = getCell(glm::vec2(destRect.x, destRect.y));
					if (left_bottom_cell->member_cordinates.size() > 0) {
						cells_in_box[left_bottom_cell->index] = left_bottom_cell;
					}

					//only do these other checks when they are needed
					if (x == 0 && y == num_y_cells - 1) {
						//left top corner
						auto  left_top_cell = getCell(glm::vec2(destRect.x, destRect.y + tile_size));
						if (left_bottom_cell->member_cordinates.size() > 0) {
							cells_in_box[left_top_cell->index] = left_top_cell;
						}
					}
					else if ((x == num_x_cells - 1 && y == num_y_cells - 1) || (x == num_x_cells - 1 && y == 0)) {
						//right bottom corner
						auto  right_bottom_cell = getCell(glm::vec2(destRect.x + tile_size, destRect.y));
						if (right_bottom_cell->member_cordinates.size() > 0) {
							cells_in_box[right_bottom_cell->index] = right_bottom_cell;
						}
					}

					if (x == num_x_cells - 1) {
						//right bottom corner
						auto  right_top_cell = getCell(glm::vec2(destRect.x + tile_size, destRect.y + tile_size));
						if (right_top_cell->member_cordinates.size() > 0) {
							cells_in_box[right_top_cell->index] = right_top_cell;
						}
					}


					if (x >= 0 && x < num_x_cells && y == num_y_cells - 1) {
						auto  left_top_cell = getCell(glm::vec2(destRect.x, destRect.y + tile_size));
						if (left_bottom_cell->member_cordinates.size() > 0) {
							cells_in_box[left_top_cell->index] = left_top_cell;
						}
					}

				}
			}

			return cells_in_box;
		}


		void Grid::updateObjectGridLocation(SquareBox::GWOM::ClusterObject& cluster_object_)
		{
			//check if we change cell location
			Cell* new_cell = getCell(cluster_object_.position);
			Cell* old_cell = (Cell*)cluster_object_.associated_pointer;
			if (new_cell != old_cell) {
				//we changed location and should update the cell
				//SBX_INFO("Changing Cells");
				//remove it from the cell its currently in
				for (auto it = old_cell->member_cordinates.begin(); it != old_cell->member_cordinates.end();)
				{
					if ((*it).first == cluster_object_.cluster_index && (*it).second == cluster_object_.index) {
						cluster_object_.associated_pointer = nullptr;
						it = old_cell->member_cordinates.erase(it);
						//SBX_INFO("Deleted from old Cell");

						break;
					}
					++it;
				}

				//Add it to the new Cell
				addObject(cluster_object_, new_cell);
				//SBX_INFO("Added to new Cell");

			}
		}

		void Grid::dispose()
		{
		}

	};
};