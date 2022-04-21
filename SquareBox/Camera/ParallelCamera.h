#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

namespace SquareBox {
	namespace Camera {
		class ParallelCamera
		{
		public:
			ParallelCamera();
			~ParallelCamera();
			void init(unsigned camera_width_, unsigned camera_height_);
			void update(unsigned new_camera_width_, unsigned new_camera_height_);

			bool isBoxInView(const glm::vec2 & position_, const glm::vec2& dimensions_);

			//setters
			void setPosition(const glm::vec2 & new_position_) { m_position = new_position_; m_needs_matrix_update = true; }
			void setScale(float new_scale_) { if (new_scale_ > 0.00001f) { m_scale = new_scale_; } m_needs_matrix_update = true; }
			void setCameraRotation(glm::vec2 rotation_) { m_rotation = rotation_; m_needs_matrix_update = true; }

			void offsetPosition(const glm::vec2& offset_) { m_position += offset_; m_needs_matrix_update = true; }
			void offsetScale(float offset_) { m_scale += offset_; if (m_scale < 0.00001f) m_scale = 0.00001f; m_needs_matrix_update = true; }
			void offsetCameraRotation(glm::vec2 offset_) { m_rotation += offset_; m_needs_matrix_update = true; }

			//getters
			glm::vec2 convertScreenToWorld(glm::vec2 screen_coords_);
			glm::vec2 getPosition() { return m_position; }
			glm::vec2 getCameraDimensions() { return glm::vec2(m_camera_width, m_camera_height) * (1 / getScale()); }
			unsigned int getAspectRatio() const { return m_camera_width / m_camera_height; }

			float getScale() { return m_scale; }
			glm::mat4 getModalViewProjectionMatrix() { return m_camera_matrix; }
			glm::vec2 getCameraRotation() const { return m_rotation; }

			void dispose() {};
		private:
			unsigned int m_camera_width, m_camera_height;
			bool m_needs_matrix_update;
			float m_scale;
			glm::vec2 m_rotation;//rotation in x coordinate and rotation  in y cooridinate
			glm::vec2 m_position; //This is the same as the GLSM vec2
			glm::mat4 m_camera_matrix;//this is the ModalViewProjectionMatrix
			glm::mat4 m_ortho_matrix;//The View Matrix
			glm::vec2  m_scaled_screen_dimensions;
		};
	}
}
