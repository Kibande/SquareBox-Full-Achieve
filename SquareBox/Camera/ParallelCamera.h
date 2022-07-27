#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <SquareBox-Logger/Logger.h>
namespace SquareBox {
	namespace Camera {
		class ParallelCamera
		{
		public:
			ParallelCamera();
			~ParallelCamera();
			void init(unsigned camera_width_, unsigned camera_height_);
			void update(unsigned screen_width_, unsigned screen_height_);

			bool isBoxInView(const glm::vec2 & position_, const glm::vec2& dimensions_);

			//setters
			void setPosition(glm::vec2 new_position_);
			void setScale(float new_scale_) { if (new_scale_ > 0.00001f) { m_scale = new_scale_; } m_needs_matrix_update = true; }
			void setCameraRotation(glm::vec2 rotation_) { m_rotation = rotation_; m_needs_matrix_update = true; }

			void offsetPosition(const glm::vec2& offset_) { setPosition(m_position + offset_); }
			void offsetScale(float offset_) { setScale(m_scale + offset_); }
			void offsetCameraRotation(glm::vec2 offset_) { m_rotation += offset_; m_needs_matrix_update = true; }

			//getters
			glm::vec2 convertScreenToWorld(glm::vec2 screen_coords_);
			glm::vec2 getPosition() { return m_position; }
			glm::vec2 getCameraDimensions() { return glm::vec2(m_screen_width, m_screen_height) * (1 / m_scale); }
			unsigned int getAspectRatio() const { return m_screen_width / m_screen_height; }

			float getScale() { return m_scale; }
			glm::mat4 getModalViewProjectionMatrix() { return m_camera_matrix; }
			glm::vec2 getCameraRotation() const { return m_rotation; }
			void bindCameraPositionToDestRect(glm::vec4& dest_rect_) {
				m_bounding_dest_rect = dest_rect_;
				m_bound_to_dest_rect = true;
			};
			void unbindCameraPositionToDestRect() { m_bound_to_dest_rect = false; }
			void dispose() {};
		private:
			bool isInBox(const  glm::vec2& testCoordinates_, const glm::vec4& testBoxDestRect_);
			glm::vec4 m_bounding_dest_rect;
			bool m_bound_to_dest_rect = false;
			unsigned int m_screen_width, m_screen_height;
			bool m_needs_matrix_update;
			float m_scale;
			glm::vec2 m_rotation;//rotation in x coordinate and rotation  in y cooridinate
			glm::vec2 m_position; //This is the same as the GLSM vec2
			glm::mat4 m_camera_matrix;//this is the ModalViewProjectionMatrix
			glm::mat4 m_ortho_matrix;//The View Matrix
		};
	}
}
