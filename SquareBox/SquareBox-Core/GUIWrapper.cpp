#include "GUIWrapper.h"

namespace SquareBox {
	void GUIWrapper::init(SquareBox::IMainGame* game_, std::map<int, int>& gui_element_coordinates_, std::string gui_file_path_)
	{
		//avaliable shapes
		m_gui_element_shapes_ptr_vec.push_back(new SquareBox::GUI::BoxGUIElement());
		m_gui_element_shapes_ptr_vec.push_back(new SquareBox::GUI::CircleGUIElement());

		//set our references
		m_game_ptr = game_;
		m_utilities.init();
		//load the gui elements from storage 
		if (m_gui_file_reader.loadGuiLayerDataAsBinary(gui_file_path_, m_gui_layer)) {
			m_utilities.loadGuiLayerTexturesAndFonts(m_gui_layer, m_game_ptr->getWindow()->getDDPI());
		}
		else {
			SBX_ERROR("Failed to load {}", gui_file_path_);
		}

		//get the respective indexs
		for (unsigned int i = 0; i < m_gui_layer.gui_elements.size(); i++)
		{
			SquareBox::GWOM::GUIElement& focus_gui_element = m_gui_layer.gui_elements[i];

			for (auto it = gui_element_coordinates_.begin(); it != gui_element_coordinates_.end(); it++)
			{
				if ((*it).first == focus_gui_element.id) {
					(*it).second = i;
				}
			}
		}
	}

	void GUIWrapper::update(SquareBox::Camera::ParallelCamera& game_camera_, float visable_area_ratio=0.9f)
	{
		glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
		glm::vec2 working_area_center(screen_dimensions.x * 0.5f, screen_dimensions.y * 0.5f);
		glm::vec2 visable_area_dimenions(screen_dimensions.x * visable_area_ratio, screen_dimensions.y * visable_area_ratio);

		m_visiable_dest_rect = glm::vec4((working_area_center - visable_area_dimenions * 0.5f), visable_area_dimenions);

		//restoring to default button state
		for (unsigned int i = 0; i < m_gui_layer.gui_elements.size(); i++)
		{
			SquareBox::GWOM::GUIElement& focus_gui_element = m_gui_layer.gui_elements[i];
			focus_gui_element.state = SquareBox::GUIElementStateEnum::DEFAULT;
		}

		//processing user input

		const std::vector<SquareBox::InputManager::LocationDetails>& locations_of_pointers_in_world = m_game_ptr->getInputDevice()->getScreenLocations();

		for (unsigned int i = 0; i < locations_of_pointers_in_world.size(); i++)
		{
			const glm::vec2& pointer_in_world = game_camera_.convertScreenToWorld(locations_of_pointers_in_world[i].coordinates);

			for (unsigned int j = 0; j < m_gui_layer.gui_elements.size(); j++)
			{
			
				SquareBox::GWOM::GUIElement& focus_gui_element = m_gui_layer.gui_elements[j];

				if (m_utilities.isInBox(pointer_in_world, m_visiable_dest_rect)) {
					guiElementShapeShellSetter(focus_gui_element);
					if (m_current_gui_element_shape_shell_ptr->containtsPoint(focus_gui_element, pointer_in_world, m_visiable_dest_rect)) {
						focus_gui_element.state = SquareBox::GUIElementStateEnum::HOVER;

#ifndef SQB_PLATFORM_ANDROID
						if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
							focus_gui_element.state = SquareBox::GUIElementStateEnum::ACTIVE;
							if (focus_gui_element.id == 1) {
								int i = focus_gui_element.id * 34;
							}
						}
#else

						focus_gui_element.state = SquareBox::GUIElementStateEnum::ACTIVE;

#endif // !SQB_PLATFORM_ANDROID


					}
				}
			}
		}
	}


	void GUIWrapper::draw(SquareBox::Camera::ParallelCamera& game_camera_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
	{
		for (unsigned int i = 0; i < m_gui_layer.gui_elements.size(); i++)
		{
			auto& focus_gui_element = m_gui_layer.gui_elements[i];
			guiElementShapeShellSetter(focus_gui_element);
			m_current_gui_element_shape_shell_ptr->drawTexture(focus_gui_element, sprite_batch_, m_visiable_dest_rect);
			auto& font_info = focus_gui_element.fonts[static_cast<int>(focus_gui_element.state)];

			if (font_info.show_text && m_gui_layer.sprite_fonts.size() > 0) {
				m_current_gui_element_shape_shell_ptr->drawText(focus_gui_element, m_gui_layer.sprite_fonts[font_info.font_index], sprite_batch_, game_camera_.getScale(), m_visiable_dest_rect);
			}
		}
	}

	void GUIWrapper::setGUIElementLocationRatio(glm::vec2 pointer_coordinates_, int gui_element_index_)
	{
		m_gui_layer.gui_elements[gui_element_index_].location_ratio = ((pointer_coordinates_ - glm::vec2(m_visiable_dest_rect.x, m_visiable_dest_rect.y)) / glm::vec2(m_visiable_dest_rect.z, m_visiable_dest_rect.w));
	}

	bool GUIWrapper::isCoordinateInGUIElement(glm::vec2 coordinate_, int gui_element_index_)
	{
		SquareBox::GWOM::GUIElement& focus_gui_element = m_gui_layer.gui_elements[gui_element_index_];
		guiElementShapeShellSetter(focus_gui_element);
		return m_current_gui_element_shape_shell_ptr->containtsPoint(focus_gui_element, coordinate_, m_visiable_dest_rect);
	}

	void GUIWrapper::dispose()
	{
		for (unsigned int i = 0; i < m_gui_element_shapes_ptr_vec.size(); i++)
		{
			delete m_gui_element_shapes_ptr_vec[i];
		}
		m_gui_element_shapes_ptr_vec.clear();

		m_utilities.dispose();

	}

	void GUIWrapper::guiElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_)
	{
		m_current_gui_element_shape_shell_ptr = nullptr;
		for (unsigned int i = 0; i < m_gui_element_shapes_ptr_vec.size(); i++)
		{
			auto& gui_shape = m_gui_element_shapes_ptr_vec[i];
			if (gui_shape->shape == gui_element_.shape) {
				m_current_gui_element_shape_shell_ptr = gui_shape;
			}
		}
	}

};