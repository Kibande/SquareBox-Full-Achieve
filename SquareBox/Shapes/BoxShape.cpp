#include "BoxShape.h"
#include<Input-Manager/IInputDevice.h>

namespace SquareBox {
	namespace Shapes {
		void BoxShape::draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_)
		{
			if (!clusterObject_.is_hidden) {
				//A box
				const glm::vec2 boxDims(clusterObject_.width, clusterObject_.height);
				glm::vec2  boxOrigin = glm::vec2(clusterObject_.position) - boxDims * 0.5f;
				/* the box padding of 0.01 has been removed from here
				 for boxes  and capsules, we shall just be submitting smaller dimensions to the physics engine
				*/
				spriteBatch_.draw(glm::vec4(boxOrigin, clusterObject_.width, clusterObject_.height), clusterObject_.texture_info.uv_rect, clusterObject_.texture_info.texture_id, 0.0f, SquareBox::RenderEngine::ColorRGBA8((clusterObject_.texture_info.color.x), (clusterObject_.texture_info.color.y), (clusterObject_.texture_info.color.z), (clusterObject_.texture_info.color.w)*(0.01*opacity_)), clusterObject_.angle);
			}
		}

		void BoxShape::keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_)
		{
			if (clusterObject_.physics_properties != nullptr && clusterObject_.physics_properties->isIntialised()) {
				clusterObject_.position = clusterObject_.physics_properties->getXYPosition();
				clusterObject_.angle = clusterObject_.physics_properties->getAngle();
			}
		}

		void BoxShape::debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  SquareBox::RenderEngine::ColorRGBA8 borderColor_)
		{
			const glm::vec2  boxDims(clusterObject_.width, clusterObject_.height);
			glm::vec2 boxOrigin = glm::vec2(clusterObject_.position) - boxDims * 0.5f;
			debugRender_.drawBox(glm::vec4(boxOrigin, clusterObject_.width, clusterObject_.height), borderColor_, clusterObject_.angle);
		}

		void BoxShape::traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_)
		{
			const glm::vec2 boxDims(clusterObject_.width, clusterObject_.height);
			const glm::vec2 & mouseBorderCenter = mouseinworld - boxDims * 0.5f;
			debugRender_.drawBox(glm::vec4(mouseBorderCenter, clusterObject_.width, clusterObject_.height), borderColor_, clusterObject_.angle);
		}

		void BoxShape::scalingDraw(SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2 & mouseinworld_, SquareBox::InputManager::IInputDevice * input_)
		{
			//we are relying on the results of keepTrackOfPosition();
			const glm::vec2 & center = clusterObject_.position;
			float angle = 0.0f;

			//setting up the dimensions for different shapes
			const float & height_scaler_length = clusterObject_.height*1.2f;
			const float & width_scaler_length = clusterObject_.width*1.2f;
			const float & general_scaler_length = glm::max(width_scaler_length, height_scaler_length);
			rotation_radius = std::min(clusterObject_.width, clusterObject_.height)*0.3f;

			//i have rotation all wrong , so angle is const 0.0f for now
			const glm::vec2	vertical_dest_point = glm::rotate(glm::vec2(center.x, center.y + height_scaler_length), angle);
			const glm::vec2	horizontal_dest_point = glm::rotate(glm::vec2(center.x + width_scaler_length, center.y), angle);
			const glm::vec2	general_dest_point = glm::rotate(glm::vec2(center.x + general_scaler_length, center.y + general_scaler_length), angle);

			//Vertical line
			debugRender_.drawLine(center, vertical_dest_point, vertical_scaling_color);
			//the selection circle
			debugRender_.drawCircle(vertical_dest_point, vertical_scaling_color, height_scaler_length*selection_circle_percentage);
			//if the circle is clicked scale according to where the mouse is
			//to select a direction for scaling , ctrl must be pressed
			if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
				if ((glm::length(vertical_dest_point - mouseinworld_) <= height_scaler_length * selection_circle_percentage)) {
					m_horizontal_scaling_selected = false;
					m_vertical_scaling_selected = true;
					m_general_scaling_selected = false;
				}
			}

			if (m_vertical_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
				float difference = mouseinworld_.y - vertical_dest_point.y;
				clusterObject_.height += difference;
				if (clusterObject_.height < 0.0f) {
					clusterObject_.height = 0.0f;
				}
				if (clusterObject_.physics_properties != nullptr) {
					clusterObject_.requesting_recreation = true;
				}
			}

			//Horizontal line
			debugRender_.drawLine(center, horizontal_dest_point, horizontal_scaling_color);
			//the selection circle
			debugRender_.drawCircle(horizontal_dest_point, horizontal_scaling_color, width_scaler_length*selection_circle_percentage);
			//if the circle is clicked scale according to where the mouse is
				//to select a direction for scaling , ctrl must be pressed
			if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
				if ((glm::length(horizontal_dest_point - mouseinworld_) <= width_scaler_length * selection_circle_percentage)) {
					m_horizontal_scaling_selected = true;
					m_vertical_scaling_selected = false;
					m_general_scaling_selected = false;
				}
			}

			if (m_horizontal_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
				float difference = mouseinworld_.x - horizontal_dest_point.x;
				clusterObject_.width += difference;
				if (clusterObject_.width < 0.0f) {
					clusterObject_.width = 0.0f;
				}
				if (clusterObject_.physics_properties != nullptr) {
					clusterObject_.requesting_recreation = true;
				}
			}

			//General line
			debugRender_.drawLine(center, general_dest_point, general_scaling_color);
			//the selection circle
			debugRender_.drawCircle(general_dest_point, general_scaling_color, general_scaler_length*selection_circle_percentage);
			if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
				if ((glm::length(general_dest_point - mouseinworld_) <= width_scaler_length * selection_circle_percentage)) {
					m_horizontal_scaling_selected = false;
					m_vertical_scaling_selected = false;
					m_general_scaling_selected = true;
				}
			}

			if (m_general_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
				float new_length = std::fabs(glm::length(center - mouseinworld_));
				/*new_length will be 0 when the mouse is at the position of the Cluster Object*/
				if (first_time) {
					cluster_object_scaling = 1.0f;
					scaling_object_line_length = new_length;
					first_time = false;
				}
				else {
					if (new_length > 0 && scaling_object_line_length > 0) {
						cluster_object_scaling = (float)new_length / (float)scaling_object_line_length;
						scaling_object_line_length = new_length;
						//do the scaling
						clusterObject_.width *= cluster_object_scaling;
						clusterObject_.height *= cluster_object_scaling;

						if (clusterObject_.height < 0.0f) {
							clusterObject_.height = 0.0f;
						}

						if (clusterObject_.width < 0.0f) {
							clusterObject_.width = 0.0f;
						}

						//reset scaling to 1.0f
						cluster_object_scaling = 1.0f;
						if (clusterObject_.physics_properties != nullptr) {
							clusterObject_.requesting_recreation = true;
						}
					}
				}
			}
			//rotation Circle
			debugRender_.drawCircle(center, rotation_color, rotation_radius);
		}

		bool BoxShape::containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_)
		{
			if (clusterObject_.is_hidden) {
				return false;
			}
			if (clusterObject_.physics_properties != nullptr) {
				return (clusterObject_.physics_properties->testPoint(coordinates_.x, coordinates_.y, 0));
			}
			else {

				//This only works if the the box has 0 rotation
				//A box

				const float x_position = clusterObject_.position.x;
				const float y_position = clusterObject_.position.y;
				const float width = clusterObject_.width;
				const float height = clusterObject_.height;

				const float x_coordinate = coordinates_.x;
				const float y_coordinate = coordinates_.y;

				const float scaled_x = width * 0.5;
				const float scaled_y = height * 0.5;


				const float orign_x = x_position - scaled_x;
				const float orign_y = y_position - scaled_y;

				//when drawing we are going to add 0.01 padding around the box to catter for physics boxes
				//this should be done for all polygons
				const float  padding = 0.01f;
				const float  doublePadding = 0.02f;

				const float  x1 = orign_x - padding;
				const float  y1 = orign_y - padding;
				const float  x2 = x1 + width + doublePadding;
				const float  y2 = y1 + +height + doublePadding;

				return (x_coordinate > x1 && x_coordinate<x2 &&y_coordinate>y1 && y_coordinate < y2);
			}
		}
		glm::vec2 BoxShape::localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_)
		{
			return glm::vec2(clusterObject_.position.x + localCoordinates_.x, clusterObject_.position.y + localCoordinates_.y);
		}
	};
};