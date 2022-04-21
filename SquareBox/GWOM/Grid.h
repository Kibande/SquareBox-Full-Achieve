#pragma once
#include <vector>
#include <algorithm>
#include "ClusterObject.h"
#include <glm/glm.hpp>
namespace SquareBox {
	namespace GWOM {
		struct Cell {
			std::vector<std::pair<int, int>> member_cordinates;
			glm::vec2 position;
			int index = -1;
		};

		class Grid
		{
		public:
			Grid();
			void init(float origin_x_, float orign_y_, float width_, float height_, float cell_size_);
			void resetGrid();
			~Grid();
			/// and object to the appropriate cell
			void addObject(SquareBox::GWOM::ClusterObject& cluster_object_);
			/// adds an object to a specific cell
			void addObject(SquareBox::GWOM::ClusterObject& cluster_object_, Cell* cell_);
			void removeObject(SquareBox::GWOM::ClusterObject& cluster_object_);
			///Get cel based on cell coordinates
			Cell* getCell(int x_, int y_);
			///Get Cell based on window coordinates
			Cell* getCell(const glm::vec2& pos_);
			bool isInGrid(const glm::vec2 pos_);
			std::map<int, SquareBox::GWOM::Cell*> getAllCellsInBox(glm::vec4 & dest_rect_);
			std::map<int, SquareBox::GWOM::Cell*> getPopulatedCellsInBox(glm::vec4 & dest_rect_);
			void updateObjectGridLocation(SquareBox::GWOM::ClusterObject& cluster_object_);

			///Getters
			int getNumXCells() const { return m_num_x_cells; }
			int getNumYCells() const { return m_num_y_cells; }
			float getWidth() const { return m_width; }
			float getHeight() const { return m_height; }
			float getCellSize() const { return m_cell_size; }
			glm::vec2 getGridOrigin() const { return glm::vec2(orign_x, orign_y); }

			Cell* getCellByIndex(const int index_) { return &m_cells[index_]; }

			std::vector<Cell>& getGridCells() { return m_cells; }
			bool isInitialised() const { return m_is_initialised;}
			void dispose();
		private:
			std::vector<Cell> m_cells;
			float m_cell_size = 0.0f;
			float m_width = 0.0f;
			float m_height = 0.0f;
			float orign_x = 0.0f;
			float orign_y = 0.0f;
			int m_num_x_cells = 0;
			int m_num_y_cells = 0;
			bool m_is_initialised = false;
		};

	};
};

