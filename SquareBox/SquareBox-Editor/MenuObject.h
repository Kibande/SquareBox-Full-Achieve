#pragma once
#include<string>
#include<unordered_map>
#include <glm/glm.hpp>
#include<SquareBox-Core/SquareBoxGameEngine.h>
	/*	a simple table like menu system	 */
	/*	 if using text, this extenstion currenlty only works best for a 32 res font
		i will have to read through the found libraray in order to be able to configure this properly so that it works for 
		all resolution 

		Author : Kibande Steven
		Date : Wenesday 22nd September 2021 17:12

	*/
class MenuObject
{
public:

	MenuObject() {
		m_name = "root";
	}

	MenuObject(const std::string & name) {
		m_name = name;
	}

	MenuObject& setTableArrangement(int nColumns, int nRows) {
		m_table_arrangement = { nColumns,nRows };
		return *this;
	}	

	MenuObject& setTitleJustification(SquareBox::JustificationEnum justification_) {
		m_title_justification = justification_;
		return *this;
	}
	

	MenuObject& setCellJustification(SquareBox::JustificationEnum justification_) {
		m_cell_justification = justification_;
		return *this;
	}


	MenuObject& setLeftRightJustificationPadding(float padding_) {
		m_left_right_justification_padding = padding_;
		return *this;
	}


	MenuObject& setChildrenPadding(glm::vec2 & childPadding_) {
		m_children_padding = childPadding_;
		return *this;
	}

	MenuObject& setCellSize(glm::vec2& cellSize_) {
		m_cell_size = cellSize_;
		return *this;
	}


	MenuObject& setTextScaling(glm::vec2& scaling_) {
		m_text_scaling = scaling_;
		return *this;
	}

	MenuObject& setTextColor(SquareBox::RenderEngine::ColorRGBA8 color_) {
		m_text_color = color_;
		return *this;
	}
	MenuObject& setSpriteFont(SquareBox::RenderEngine::SpriteFont* spriteFontPtr_) {
		m_sprite_font_ptr = spriteFontPtr_;
		return *this;
	}

	MenuObject& setID(int32_t id) {
		m_Id = id;
		return *this;
	}

	MenuObject& setObject(SquareBox::GWOM::ClusterObject  clusterObject_) {
		m_cluster_object = clusterObject_;
		return *this;
	}

	MenuObject& setBackGroundObject(SquareBox::GWOM::ClusterObject clusterObject_) {
		m_back_ground_cluster_object = clusterObject_;
		return *this;
	}

	MenuObject& setOnClickObject(SquareBox::GWOM::ClusterObject clusterObject_) {
		m_on_click_cluster_object = clusterObject_;
		return *this;
	}

	MenuObject& setOnHoverObject(SquareBox::GWOM::ClusterObject clusterObject_) {
		m_on_hover_cluster_object = clusterObject_;
		return *this;
	}

	MenuObject& enable(bool b) {
		m_enabled = b;
		return *this;
	}


	MenuObject& showText(bool b) {
		m_show_text = b;
		return *this;
	}
	MenuObject& showTitle(bool b) {
		m_show_title = b;
		return *this;
	}

	bool isEnabled() {
		return m_enabled;
	}

	int32_t getID() {
		return m_Id;
	}

	std::string &  getName() {
		return m_name;
	}

	glm::vec2  getTableSize()   {
		return m_table_size;
	}
	//For now cells are simply one line strings
	glm::vec2 getNameSize() {
		return m_sprite_font_ptr->measure(m_name.c_str());
	}

	bool hasChildren()
	{
		return !m_items.empty();
	}

	MenuObject& operator[](const std::string& name_) {

		if (m_item_pointer.count(name_) == 0) {
			//create it since it doesn't yet exist
			m_item_pointer[name_] = m_items.size();
			m_items.push_back(MenuObject(name_));
			m_items.back().setSpriteFont(m_sprite_font_ptr);//share the spirte Font 
			m_items.back().setChildrenPadding(m_children_padding);
			m_items.back().setObject(m_cluster_object);
			m_items.back().setOnHoverObject(m_on_hover_cluster_object);
			m_items.back().setOnClickObject(m_on_click_cluster_object);
			m_items.back().setBackGroundObject(m_back_ground_cluster_object);
			m_items.back().setCellSize(m_cell_size);
			m_items.back().setTextScaling(m_text_scaling);
			m_items.back().setTextColor(m_text_color);
		}
		return m_items[m_item_pointer[name_]];
	}



	MenuObject* drawSelf(SquareBox::IMainGame* gamePtr_, const glm::vec2& mouseinworld_, SquareBox::RenderEngine::SpriteBatch* spriteBatch_, SquareBox::RenderEngine::DebugRenderer* debugRenderer_, glm::vec2 position_) {
		MenuObject* returnie = nullptr;

		//pull the position to the center
		position_ =position_- glm::vec2(m_table_size.x, m_table_size.y) * 0.5f;

		glm::vec4 destRect(position_, m_table_size);
		if (spriteBatch_ != nullptr) {
			spriteBatch_->draw(destRect, m_back_ground_cluster_object.texture_info.uv_rect, m_back_ground_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8((m_back_ground_cluster_object.color.x), (m_back_ground_cluster_object.color.y), (m_back_ground_cluster_object.color.z), (m_back_ground_cluster_object.color.w)));
		}

		if (debugRenderer_ != nullptr) {
			debugRenderer_->drawBox(destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);
		}

		//start position for drawing
		glm::vec2 start_position(position_.x, position_.y + m_table_size.y);

		if (m_show_title) {
			start_position.y -= getNameSize().y;
			if (spriteBatch_ != nullptr) {
				//total distance we are working with
				glm::vec2 point_a = start_position;
				glm::vec2 point_b = start_position;
				point_b.x += m_table_size.x;
				if (m_title_justification == SquareBox::JustificationEnum::LEFT) {
					m_sprite_font_ptr->draw(*spriteBatch_, getName().c_str(), point_a, m_text_scaling, 1.0f, m_text_color, m_title_justification);
				}
				else if (m_title_justification == SquareBox::JustificationEnum::MIDDLE) {
					m_sprite_font_ptr->draw(*spriteBatch_, getName().c_str(), (point_a + point_b) * 0.5f, m_text_scaling, 1.0f, m_text_color, m_title_justification);
				}
				else if (m_title_justification == SquareBox::JustificationEnum::RIGHT) {
					m_sprite_font_ptr->draw(*spriteBatch_, getName().c_str(), point_b, m_text_scaling, 1.0f, m_text_color, m_title_justification);
				}
			}
		}


		int current_item_index = 0;
		for (unsigned int y = 0; y < m_table_arrangement.y && current_item_index < m_items.size(); y++)
		{
			start_position.y -= m_children_padding.y;

				//get the maximum height in  the row that we are going to draw
			float max_height = 0.0f;
			for (unsigned i = 0; i < m_table_arrangement.x && current_item_index+i < m_items.size(); i++)
			{
				max_height = std::max(max_height, m_items[current_item_index+i].m_cell_size.y);
			}
			start_position.y -= max_height;
			float start_x = start_position.x;
			for (unsigned x = 0; x < m_table_arrangement.x && current_item_index < m_items.size(); x++)
			{
				auto& m = m_items[current_item_index];
				start_position.x += m_children_padding.x;
				glm::vec4 destRect(start_position, m.m_cell_size);
			
				if (debugRenderer_ != nullptr) {
					debugRenderer_->drawBox(destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);
				
				}
				bool clicked = false;
				bool hovered = false;
				//testing mouse presense
				if (isInBox(mouseinworld_, destRect)) {
					hovered = true;
					if (gamePtr_->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
						clicked = true;
						if (m.hasChildren()) {
							returnie=&m;
						}
					}
				}

					if (clicked) {
						if (spriteBatch_ != nullptr) {
							spriteBatch_->draw(destRect, m.m_on_click_cluster_object.texture_info.uv_rect, m.m_on_click_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_on_click_cluster_object.color));
						}
					}
					else if (hovered) {
						if (spriteBatch_ != nullptr) {
							spriteBatch_->draw(destRect, m.m_on_hover_cluster_object.texture_info.uv_rect, m.m_on_hover_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_on_hover_cluster_object.color));
						}
					}
					else {
						//just display our normal texture
						if (spriteBatch_ != nullptr) {
							spriteBatch_->draw(destRect, m.m_cluster_object.texture_info.uv_rect, m.m_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_cluster_object.color));
						}
					}
				

				
					if (spriteBatch_ != nullptr && m.m_show_text) {
						//draw texture first
						if (m.m_cell_justification == SquareBox::JustificationEnum::LEFT) {
							auto text_position = start_position + glm::vec2(destRect.z, destRect.w) * 0.5f;
							//the x is in place now the y
							float name_height = m.getNameSize().y;
							text_position.y -= name_height * 0.5f;
							text_position.x = start_position.x + m.m_left_right_justification_padding;
							m_sprite_font_ptr->draw(*spriteBatch_, m.getName().c_str(), text_position, m.m_text_scaling, 1.0f, m.m_text_color, m.m_cell_justification);
						}
						else if (m.m_cell_justification == SquareBox::JustificationEnum::MIDDLE) {
							//position text start in middle of button
							auto text_position = start_position + glm::vec2(destRect.z, destRect.w) * 0.5f;
							//the x is in place now the y
							float name_height = m.getNameSize().y;
							text_position.y -= name_height * 0.5f;
							m_sprite_font_ptr->draw(*spriteBatch_, m.getName().c_str(), text_position, m.m_text_scaling, 1.0f, m.m_text_color, m.m_cell_justification);
						}
						else if (m.m_cell_justification == SquareBox::JustificationEnum::RIGHT) {
							auto text_position = start_position + glm::vec2(destRect.z, destRect.w) * 0.5f;
							//the x is in place now the y
							float name_height = m.getNameSize().y;
							float name_width = m.getNameSize().x;
							text_position.y -= name_height * 0.5f;
							text_position.x = start_position.x + destRect.z - name_width - m.m_left_right_justification_padding;
							m_sprite_font_ptr->draw(*spriteBatch_, m.getName().c_str(), text_position, m.m_text_scaling, 1.0f, m.m_text_color, m.m_cell_justification);
						}
					
					}
				current_item_index += 1;//move to the next item count when ever we draw
				start_position.x += m.m_cell_size.x;//consider the space occupied by the one that 
				//came before
			}
			start_position.x = start_x;
		}

		return returnie;

	}

	void Build()
	{
		
		// Recursively build all children, so they can determine their size, use
		// that size to indicate cell sizes if this object contains more than 
		// one item
		
		for (auto& m : m_items)
		{
			if (m.hasChildren())
			{
				m.Build();

			}
		}

		//computing the tables sizes
		int current_item_index = 0;
		float max_width_length = 0.0f;
		for (unsigned int y = 0; y < m_table_arrangement.y && current_item_index < m_items.size(); y++)
		{
			
			float max_height_length = 0.0f;
			m_table_size.x = 0;
			for (unsigned x = 0; x < m_table_arrangement.x && current_item_index < m_items.size(); x++)
			{
				m_table_size.x += m_children_padding.x;
				m_table_size.x += m_items[current_item_index].m_cell_size.x;
				max_height_length = std::max(max_height_length, m_items[current_item_index].m_cell_size.y);
				current_item_index++;
			}
			m_table_size.y += max_height_length;
			m_table_size.y += m_children_padding.y;
			max_width_length = std::max(max_width_length, m_table_size.x);
		}

		m_table_size.x = max_width_length;
		//add padding on the sides
		m_table_size.x += m_children_padding.x;
		m_table_size.y += m_children_padding.y;

		if (m_show_title) {
			m_table_size.y += getNameSize().y;
		}
	}

	private:
		bool isInBox(const  glm::vec2 & testCoordinates_,const glm::vec4 & testBoxDestRect_) {
			const float x_position = testBoxDestRect_.x;
			const float y_position = testBoxDestRect_.y;
			const float width = testBoxDestRect_.z;
			const float height = testBoxDestRect_.w;

			const float x_coordinate = testCoordinates_.x;
			const float y_coordinate = testCoordinates_.y;

			const float orign_x = x_position;
			const float orign_y = y_position;

			const float  x1 = orign_x;
			const float  y1 = orign_y;
			const float  x2 = x1 + width;
			const float  y2 = y1 + +height;

			return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);
		}
protected:
	std::string m_name;
	bool m_enabled = true;
	glm::vec2 m_text_scaling = {1.0f,1.0f};
	int32_t m_Id = -1;
	glm::vec2 m_table_size = { 0, 0 };
	glm::ivec2 m_table_arrangement = { 1,1 };
	glm::vec2 m_cell_size;
	glm::vec2 m_children_padding=  { 0.0f,0.0f };
	SquareBox::RenderEngine::ColorRGBA8 m_text_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::black);
	bool m_show_title = true;
	bool m_show_text = true;
	SquareBox::JustificationEnum m_title_justification=SquareBox::JustificationEnum::MIDDLE;
	SquareBox::JustificationEnum m_cell_justification = SquareBox::JustificationEnum::MIDDLE;
	float m_left_right_justification_padding = 0.0f;
	std::unordered_map<std::string, size_t> m_item_pointer;
	std::vector<MenuObject> m_items;
	SquareBox::RenderEngine::SpriteFont* m_sprite_font_ptr= nullptr;
	SquareBox::GWOM::ClusterObject m_cluster_object;
	SquareBox::GWOM::ClusterObject m_on_click_cluster_object;
	SquareBox::GWOM::ClusterObject m_on_hover_cluster_object;
	SquareBox::GWOM::ClusterObject m_back_ground_cluster_object;
};

