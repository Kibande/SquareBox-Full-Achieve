#include "ParallelCamera.h"
#include <iostream>
namespace SquareBox {
	namespace Camera {
		ParallelCamera::ParallelCamera() :
			m_screen_width(500),
			m_screen_height(500),
			m_needs_matrix_update(1),
			m_scale(1.0f),
			m_position(0.0f, 0.0f),
			m_camera_matrix(1.0f),
			m_ortho_matrix(1.0f)
		{
		}

		ParallelCamera::~ParallelCamera()
		{
		}

		void ParallelCamera::init(glm::ivec2 screen_dimensions_)
		{
			init(screen_dimensions_.x, screen_dimensions_.y);
		}

		void ParallelCamera::init(unsigned camera_width_, unsigned camera_height_)
		{
			m_screen_width = camera_width_;
			m_screen_height = camera_height_;
			m_ortho_matrix = glm::ortho(0.0f, (float)m_screen_width, 0.0f, (float)m_screen_height);
		}

		void ParallelCamera::update(unsigned screen_width_, unsigned screen_height_)
		{
			if (m_screen_width != screen_width_ || m_screen_height != screen_height_) {
				//only do when we get a new screen resolution

				//adjusting position_ accordingly
				m_position.x = (m_position.x / m_screen_width) * screen_width_;
				m_position.y = (m_position.y / m_screen_height) * screen_height_;

				//adjust the values used to convert screen to world
				m_screen_width = screen_width_;
				m_screen_height = screen_height_;

				m_needs_matrix_update = true;
			}
			//only update the Camera2D matrix if we need to

			if (m_needs_matrix_update) {
				//Camera2D Translation
				//build our own orthoMatrix when the screen width or Height changes
				m_ortho_matrix = glm::ortho(0.0f, (float)m_screen_width, 0.0f, (float)m_screen_height);

				//create a vector that will encode the translation
				glm::vec3 translate(-m_position.x + m_screen_width * 0.5f, -m_position.y + m_screen_height * 0.5f, 0.0f);//The Modal Matrix

				m_camera_matrix = glm::translate(m_ortho_matrix, translate);

				// Camera Scale
				//scale the translation

				//create a vector that will encode the scaling
				glm::vec3 scale(m_scale, m_scale, 0.0f);//The Projection Matrix
				m_camera_matrix = glm::scale(glm::mat4(1.0f), scale) * m_camera_matrix;
				m_needs_matrix_update = false;
			}
		}

		glm::vec2 ParallelCamera::convertScreenToWorld(glm::vec2 screen_coords_)
		{
			//Invert y direction
			//cos in graphics the plane is up side down
			screen_coords_.y = m_screen_height - screen_coords_.y;
			//change to a (0,0) center coordinates system
			screen_coords_ -= glm::vec2(m_screen_width / 2, m_screen_height / 2);

			//account for the scaling(Zooming)
			screen_coords_ /= m_scale;
			//translate with the Camera2D position_
			screen_coords_ += m_position;

			return screen_coords_;
		}

		bool ParallelCamera::isInBox(const  glm::vec2& testCoordinates_, const glm::vec4& testBoxDestRect_) {
			const float x_position = testBoxDestRect_.x;
			const float y_position = testBoxDestRect_.y;
			const float width = testBoxDestRect_.z;
			const float height = testBoxDestRect_.w;

			const float x_coordinate = testCoordinates_.x;
			const float y_coordinate = testCoordinates_.y;

			const float  x1 = x_position;
			const float  y1 = y_position;
			const float  x2 = x1 + width;
			const float  y2 = y1 + +height;

			return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);
		}

		// Simple AABB test to see if a box is in the camera view
		bool ParallelCamera::isBoxInView(const glm::vec2& position_, const glm::vec2& dimensions_) {
			glm::vec2 scaledScreenDimensions = glm::vec2(m_screen_width, m_screen_height) / (m_scale);

			// The minimum distance before a collision occurs
			const float MIN_DISTANCE_X = dimensions_.x / 2.0f + scaledScreenDimensions.x / 2.0f;
			const float MIN_DISTANCE_Y = dimensions_.y / 2.0f + scaledScreenDimensions.y / 2.0f;

			// Center position of the parameters
			glm::vec2 centerPos = m_position + dimensions_ / 2.0f;
			// Center position of the camera
			glm::vec2 centerCameraPos = m_position;
			// Vector from the input to the camera
			glm::vec2 distVec = centerPos - centerCameraPos;

			// Get the depth of the collision
			float xDepth = MIN_DISTANCE_X - std::abs(distVec.x);
			float yDepth = MIN_DISTANCE_Y - std::abs(distVec.y);

			// If both the depths are > 0, then we collided
			if (xDepth > 0 && yDepth > 0) {
				// There was a collision
				return true;
			}
			return false;

		}
		void ParallelCamera::setPosition(glm::vec2 new_position_)
		{
			if (m_bound_to_dest_rect) {
				// make sure we are in the bounding area
				const glm::vec2& camera_dimensions = getCameraDimensions();
				const glm::vec4 desired_camera_destRect(new_position_ - (camera_dimensions * 0.5f), camera_dimensions);

				glm::vec2 top_right(desired_camera_destRect.x + desired_camera_destRect.z, desired_camera_destRect.y + desired_camera_destRect.w);
				glm::vec2 bottom_left(desired_camera_destRect.x, desired_camera_destRect.y);

				glm::vec2 world_top_right(m_bounding_dest_rect.x + m_bounding_dest_rect.z, m_bounding_dest_rect.y + m_bounding_dest_rect.w);
				glm::vec2 world_bottom_left(m_bounding_dest_rect.x, m_bounding_dest_rect.y);

				if (!isInBox(top_right, m_bounding_dest_rect)) {
					if (top_right.x > world_top_right.x) {
						new_position_.x -= (top_right.x - world_top_right.x);
					}

					if (top_right.y > world_top_right.y) {
						new_position_.y -= (top_right.y - world_top_right.y);
					}
				}


				if (!isInBox(bottom_left, m_bounding_dest_rect)) {
					if (bottom_left.x < world_bottom_left.x) {
						new_position_.x += (world_bottom_left.x - bottom_left.x);
					}

					if (bottom_left.y < world_bottom_left.y) {
						new_position_.y += (world_bottom_left.y - bottom_left.y);
					}
				}

				m_position = new_position_; m_needs_matrix_update = true;

			}
			else {
				m_position = new_position_; m_needs_matrix_update = true;
			}
		}
	}
}