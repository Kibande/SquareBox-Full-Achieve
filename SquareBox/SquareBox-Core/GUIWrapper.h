#pragma once
#include <GWOM/GWOM.h>
#include <GUI/GUI.h>
#include<Camera/Camera.h>
#include "IMainGame.h"
#include "Utilities.h"
#include <glm/glm.hpp>
namespace SquareBox {
		class GUIWrapper
		{
		public:
			void init(SquareBox::IMainGame* game_, std::map<int, int>& gui_element_coordinates_, std::string gui_file_path_);
			void update(SquareBox::Camera::ParallelCamera& game_camera_);
			void draw(SquareBox::Camera::ParallelCamera& game_camera_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_);
			void setGUIElementLocationRatio(glm::vec2 pointer_coordinates_, int gui_element_index_);
			SquareBox::GWOM::GUIElement& getGuiElementByIndex(int index_) { return m_gui_layer.gui_elements[index_]; };
			glm::vec2 getGuiElementWorldPositionByIndex(int index_) { 
				SquareBox::GWOM::GUIElement& focus_element = m_gui_layer.gui_elements[index_];
				return glm::vec2(m_visiable_dest_rect.x, m_visiable_dest_rect.y) + glm::vec2(focus_element.location_ratio.x * m_visiable_dest_rect.z, focus_element.location_ratio.y * m_visiable_dest_rect.w);
				};
			bool isCoordinateInGUIElement(glm::vec2 coordinate_, int gui_element_index_);
			void dispose();
		private:
			void guiElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_);
			SquareBox::Utilities  m_utilities;
			SquareBox::IMainGame* m_game_ptr;
			SquareBox::GWOM::GuiLayer m_gui_layer;
			SquareBox::GWOM::GuiReaderWriter m_gui_file_reader;
			SquareBox::GUI::IGUIElement* m_current_gui_element_shape_shell_ptr = nullptr;
			std::vector<SquareBox::GUI::IGUIElement*> m_gui_element_shapes_ptr_vec;
			glm::vec4 m_visiable_dest_rect;
		};
}

