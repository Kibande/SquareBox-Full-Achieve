#pragma once
#include<string>
#include<unordered_map>
#include <glm/glm.hpp>
#include<SquareBox-Core/SquareBoxGameEngine.h>
	/*	a simple table like menu system	 */
class MenuObject
{
public:

	MenuObject() {
		m_name = "name";
	}

	MenuObject(const std::string & name) {
		m_name = name;
	}


	MenuObject& setTitleText(std::string title_text_) {
		m_title = title_text_;
		return *this;
	}

	MenuObject& setNumColumns(int num_columns_) {
		m_num_columns = num_columns_;
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
	MenuObject& setTextToBoxHeightScale(float text_scaling_) {
		m_text_to_box_height_scale = text_scaling_;
		return *this;
	}


	MenuObject& setChildrenPadding(glm::vec2 & childPadding_) {
		m_children_padding = childPadding_;
		return *this;
	}
	MenuObject& setTitlePadding(glm::vec2& title_padding_) {
		m_title_padding = title_padding_;
		return *this;
	}


	MenuObject& setCellSize(glm::vec2& cellSize_) {
		m_cell_size = cellSize_;
		return *this;
	}


	MenuObject& setTextColor(SquareBox::RenderEngine::ColorRGBA8 color_) {
		m_text_color = color_;
		return *this;
	}

	MenuObject& setID(int32_t id) {
		m_Id = id;
		return *this;
	}

	MenuObject& setObject(SquareBox::GWOM::ClusterObject  cluster_object_) {
		m_cluster_object = cluster_object_;
		return *this;
	}

	MenuObject& setBackGroundObject(SquareBox::GWOM::ClusterObject cluster_object_) {
		m_back_ground_cluster_object = cluster_object_;
		return *this;
	}

	MenuObject& setOnClickObject(SquareBox::GWOM::ClusterObject cluster_object_) {
		m_on_click_cluster_object = cluster_object_;
		return *this;
	}

	MenuObject& setOnHoverObject(SquareBox::GWOM::ClusterObject cluster_object_) {
		m_on_hover_cluster_object = cluster_object_;
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

	bool hasChildren()
	{
		return !m_items.empty();
	}

	MenuObject& operator[](const std::string& name_) {

		if (m_item_pointer.count(name_) == 0) {
			//create it since it doesn't yet exist
			m_item_pointer[name_] = m_items.size();
			m_items.push_back(MenuObject(name_));
			//inherit all the parents properties
			m_items.back().setChildrenPadding(m_children_padding);
			m_items.back().setTitlePadding(m_title_padding);
			m_items.back().setNumColumns(m_num_columns);
			m_items.back().setTextToBoxHeightScale(m_text_to_box_height_scale);
			m_items.back().setObject(m_cluster_object);
			m_items.back().setOnHoverObject(m_on_hover_cluster_object);
			m_items.back().setOnClickObject(m_on_click_cluster_object);
			m_items.back().setBackGroundObject(m_back_ground_cluster_object);
			m_items.back().setCellSize(m_cell_size);
			m_items.back().setTextColor(m_text_color);
		}
		return m_items[m_item_pointer[name_]];
	}



	MenuObject* drawSelf(SquareBox::IMainGame* game_ptr_, const glm::vec2& mouse_in_world_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_,SquareBox::RenderEngine::SpriteFont & sprite_font_,SquareBox::RenderEngine::DebugRenderer& debug_renderer_,float camera_scale_, glm::vec2 position_) {
		MenuObject* returnie = nullptr;
		glm::vec2 title_dimentions = sprite_font_.measure(m_name.c_str());
		float title_height = (title_dimentions.y / camera_scale_);
		float title_width = (title_dimentions.x / camera_scale_);
		

		//pull the position to the bottom left corner
		glm::vec4 bounding_grid_dest_rect(position_ - (m_table_size * 0.5f), m_table_size);
		
		sprite_batch_.draw(bounding_grid_dest_rect, m_back_ground_cluster_object.texture_info.uv_rect, m_back_ground_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8((m_back_ground_cluster_object.texture_info.color.x), (m_back_ground_cluster_object.texture_info.color.y), (m_back_ground_cluster_object.texture_info.color.z), (m_back_ground_cluster_object.texture_info.color.w)));

		debug_renderer_.drawBox(bounding_grid_dest_rect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);

		//start position for drawing
		glm::vec2 current_drawing_position(bounding_grid_dest_rect.x, bounding_grid_dest_rect.y + m_table_size.y);

		if (m_show_title) {
			
			current_drawing_position.y -= m_title_padding.y;
			current_drawing_position.y -= title_height;
			current_drawing_position.y -= m_title_padding.y;

			//total distance we are working with
			glm::vec2 text_positioning;

			if (m_title_justification == SquareBox::JustificationEnum::LEFT) {
				text_positioning = current_drawing_position;
			}
			else if (m_title_justification == SquareBox::JustificationEnum::MIDDLE) {
				text_positioning = glm::vec2(current_drawing_position.x + bounding_grid_dest_rect.z*0.5f, current_drawing_position.y);
			}
			else if (m_title_justification == SquareBox::JustificationEnum::RIGHT) {
				text_positioning = glm::vec2(current_drawing_position.x + bounding_grid_dest_rect.z, current_drawing_position.y);
			}
			else {
				SBX_CORE_ERROR("Unidentified Text Justification");
			}
				sprite_font_.draw(sprite_batch_, m_title.c_str(), text_positioning, glm::vec2(1 / camera_scale_), 1.0f, m_text_color, m_title_justification);
		}

		if (m_num_columns > 0) {

			current_drawing_position.y -= m_children_padding.y;
			current_drawing_position.x += m_children_padding.x;
			for (unsigned int item_count = 0; item_count < m_items.size();)
			{
				current_drawing_position.y -= m_cell_size.y;
				float x_coordinates = current_drawing_position.x;
				for (unsigned int j = 0; j < m_num_columns; j++) {
					
					auto& m = m_items[item_count];
					glm::vec4 cell_dest_rect(x_coordinates, current_drawing_position.y, m_cell_size);
					// draw the cell
					/*
						desired changes in the way the buttons look like should be done here since we alread have the dest rect with us
					
					*/


			

					debug_renderer_.drawBox(cell_dest_rect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);
				
					bool clicked = false;
					bool hovered = false;
					//testing mouse presense
					if (isInBox(mouse_in_world_, cell_dest_rect) && m.isEnabled()) {
						hovered = true;
						if (game_ptr_->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
							clicked = true;
							if (m.hasChildren()) {
								returnie=&m;
							}
						}
					}

					if (hovered) {
						sprite_batch_.draw(cell_dest_rect, m.m_on_hover_cluster_object.texture_info.uv_rect, m.m_on_hover_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_on_hover_cluster_object.texture_info.color));
					}else if (clicked) {
						sprite_batch_.draw(cell_dest_rect, m.m_on_click_cluster_object.texture_info.uv_rect, m.m_on_click_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_on_click_cluster_object.texture_info.color));
					}
					else {
						sprite_batch_.draw(cell_dest_rect, m.m_cluster_object.texture_info.uv_rect, m.m_cluster_object.texture_info.texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(m.m_cluster_object.texture_info.color));
					}
				

				
					if (m.m_show_text) {
						glm::vec2 text_dimentions = sprite_font_.measure(m.m_name.c_str());
						float text_height = (text_dimentions.y / camera_scale_);
						float text_width = (text_dimentions.x / camera_scale_);

						float text_to_cell_height = cell_dest_rect.w / text_height;

						auto text_position =glm::vec2(cell_dest_rect.x, cell_dest_rect.y);

				

						if (m.m_cell_justification == SquareBox::JustificationEnum::LEFT) {
							//the x is in place now the y
						}
						else if (m.m_cell_justification == SquareBox::JustificationEnum::MIDDLE) {
							text_position.x += cell_dest_rect.z*0.5f;
						}
						else if (m.m_cell_justification == SquareBox::JustificationEnum::RIGHT) {
							text_position.x += cell_dest_rect.z;
						}

						float before_scale = (1 / camera_scale_) * text_to_cell_height;
						float after_scale = (1 / camera_scale_) * text_to_cell_height * m.m_text_to_box_height_scale;

						float difference_in_scales = before_scale - after_scale;
						if (difference_in_scales > 0 && !SquareBox::MathLib::Float::areFloatsEqual(difference_in_scales, 0)) {
							// the scale is making the text smaller the the space its supposed to occupy
							//so shit it so that it appears to be in the center but shitfting it upp by half the difference in propotional
							//cell size
							text_position.y += (difference_in_scales * cell_dest_rect.w) * 0.5f;
						}
						else {
							//ignore making the text bigger that the bounding box
							after_scale = before_scale;
						}

						std::string string_text = m.getName();

						//handling texts that are longer than the cell
						if (text_width>cell_dest_rect.z) {
							//we also accounted for the horizontal scaling too.
							float width_correction_ratio = cell_dest_rect.z/text_width;
							
							string_text = m.m_name.substr(0, m.m_name.length()* (width_correction_ratio + (width_correction_ratio * (1-after_scale))));
						}

						sprite_font_.draw(sprite_batch_, string_text.c_str(), text_position, glm::vec2(after_scale), 1.0f, m.m_text_color, m.m_cell_justification);
					
					}
					x_coordinates += m_cell_size.x;
					x_coordinates += m_children_padding.x;
					item_count += 1;
					if (!(item_count<m_items.size())) {
						//the grid has less elements than its ideal max
						break;
					}
				}
				current_drawing_position.y -= m_children_padding.y;
			}
		}

		return returnie;
	}

	void Build(SquareBox::RenderEngine::SpriteFont & sprite_font_,float camera_scale_)
	{
		// Recursively build all children, so they can determine their size, use
		// that size to indicate cell sizes if this object contains more than 
		// one item
		//the name and title are of the same font hence same height and width dimensions
		glm::vec2 font_dimentions = sprite_font_.measure(m_name.c_str());
		float font_height = (font_dimentions.y / camera_scale_);
		float font_width = (font_dimentions.x / camera_scale_);

		for (auto& m : m_items)
		{
			if (m.hasChildren())
			{
				m.Build(sprite_font_,camera_scale_);
			}
		}

		m_table_size.x = 0;
		m_table_size.y = 0;

		float total_width = 0.0f;
		float total_height = 0.0f;

		//add padding on the sides
		if (m_show_title) {
			total_height += m_title_padding.y;//top padding
			total_height += font_height;
			total_height += m_title_padding.y;//bottom padding
		}

		if (m_num_columns>0) {

			total_width += m_children_padding.x;//left padding
			total_height += m_children_padding.y;//top padding

			float grid_width = 0.0f;

			for (unsigned int item_count = 0; item_count < m_items.size();)
			{
				grid_width = 0.0f;
				for (unsigned int j = 0; j < m_num_columns; j++) {
					grid_width += m_cell_size.x;
					grid_width += m_children_padding.x;//right padding
					item_count += 1;
					if (!(item_count < m_items.size())) {
						//the grid has less elements than its ideal max
						break;
					}
				}
				total_height += m_cell_size.y;
				total_height += m_children_padding.y;//bottom padding
			}
			/* 
			* 
				grid with will get over written a few times but we shall still get our desired output
			*/

			total_width += grid_width;
		}

		m_table_size.x = total_width;
		m_table_size.y = total_height;
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
	std::string m_title="title";
	bool m_enabled = true;
	int32_t m_Id = -1;
	glm::vec2 m_table_size = { 0, 0 };
	int m_num_columns = 1;
	glm::vec2 m_cell_size = { 0.0f,0.0f };
	glm::vec2 m_children_padding = { 0.0f,0.0f };
	glm::vec2 m_title_padding = { 0.0f,0.0f };
	float m_text_to_box_height_scale = 1.0f;//ratio the scale should be to its would be normal
	SquareBox::RenderEngine::ColorRGBA8 m_text_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::black);
	bool m_show_title = true;
	bool m_show_text = true;
	SquareBox::JustificationEnum m_title_justification=SquareBox::JustificationEnum::MIDDLE;
	SquareBox::JustificationEnum m_cell_justification = SquareBox::JustificationEnum::MIDDLE;
	float m_left_right_justification_padding = 0.0f;
	std::unordered_map<std::string, size_t> m_item_pointer;
	std::vector<MenuObject> m_items;
	SquareBox::GWOM::ClusterObject m_cluster_object;
	SquareBox::GWOM::ClusterObject m_on_click_cluster_object;
	SquareBox::GWOM::ClusterObject m_on_hover_cluster_object;
	SquareBox::GWOM::ClusterObject m_back_ground_cluster_object;
};

