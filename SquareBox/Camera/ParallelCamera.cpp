#include "ParallelCamera.h"
#include <iostream>
namespace SquareBox {
	namespace Camera {
		ParallelCamera::ParallelCamera() :
			m_camera_width(500),
			m_camera_height(500),
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

		void ParallelCamera::init(unsigned camera_width_, unsigned camera_height_)
		{
			m_camera_width = camera_width_;
			m_camera_height = camera_height_;
			m_ortho_matrix = glm::ortho(0.0f, (float)m_camera_width, 0.0f, (float)m_camera_height);
		}

		void ParallelCamera::update(unsigned new_camera_width_, unsigned new_camera_height_)
		{
			if (m_camera_width != new_camera_width_ || m_camera_height != new_camera_height_) {
				//only do when we get a new screen resolution
				
				//adjusting position_ accordinlgly
				m_position.x = (m_position.x / m_camera_width)* new_camera_width_;
				m_position.y = (m_position.y / m_camera_height)* new_camera_height_;
				
				//adjust the values used to convert screen to world
				m_camera_width = new_camera_width_;
				m_camera_height = new_camera_height_;

				m_needs_matrix_update = true;
			}
			//only update the Camera2D matrix if we need to

			if (m_needs_matrix_update) {
				m_camera_width = m_camera_width;
				m_camera_height = m_camera_height;
				//Camera2D Translation
				//build our own orthoMatrix when the screen width or Height changes
				m_ortho_matrix = glm::ortho(0.0f, (float)m_camera_width, 0.0f, (float)m_camera_height);

				//create a vector that will encode the transalation
				glm::vec3 translate(-m_position.x + m_camera_width / 2, -m_position.y + m_camera_height / 2, 0.0f);//The Modal Matrix

				m_camera_matrix = glm::translate(m_ortho_matrix, translate);

				// Camere Scale
				//scale the transalation

				//create a vector that will encode the scalling
				glm::vec3 scale(m_scale, m_scale, 0.0f);//The Projection Matrix
				m_camera_matrix = glm::scale(glm::mat4(1.0f), scale)*m_camera_matrix;
				m_needs_matrix_update = false;
			}
		}

		glm::vec2 ParallelCamera::convertScreenToWorld(glm::vec2 screen_coords_)
		{
			//Invert y direction
			//cos in graphics the plane is up side down
			screen_coords_.y = m_camera_height - screen_coords_.y;
			//change to a (0,0) center coordinates system
			screen_coords_ -= glm::vec2(m_camera_width / 2, m_camera_height / 2);

			//account for the scaling(Zooming)
			screen_coords_ /= m_scale;
			//translate with the Camera2D position_
			screen_coords_ += m_position;

			return screen_coords_;
		}

		// Simple AABB test to see if a box is in the camera view
		bool ParallelCamera::isBoxInView(const glm::vec2& position_, const glm::vec2& dimensions_) {
			//return true;//TO FIX
			//const glm::vec2  m_scaled_screen_dimensions = glm::vec2(m_camera_width, m_camera_height) / (m_scale);

			m_scaled_screen_dimensions.x = (float)m_camera_width / m_scale;
			m_scaled_screen_dimensions.y = (float)m_camera_height / m_scale;

			/*
			the minimum distance for a collision to occur is distance from the camera center to its edge + the dimensions_ of the
			box
			*/
			const float & MIN_DISTANCE_X = dimensions_.x + (m_scaled_screen_dimensions.x / 2.0f);
			const float & MIN_DISTANCE_Y = dimensions_.y + (m_scaled_screen_dimensions.y / 2.0f);

			// Center position_ of the parameters
			const glm::vec2 & boxCenterPos = position_ + (dimensions_ / 2.0f);
			// Center position_ of the camera
			const glm::vec2 & centerCameraPos = m_position;
			// Vector from the input to the camera
			const glm::vec2 & distVec = boxCenterPos - centerCameraPos;

			// Get the depth of the collision

			const float & xDepth = MIN_DISTANCE_X - std::abs(distVec.x);
			const float & yDepth = MIN_DISTANCE_Y - std::abs(distVec.y);

			// If both the depths are > 0, then we collided
			if (xDepth > 0 && yDepth > 0) {
				// There was a collision
				return true;
			}
			return false;
		}
	}
}