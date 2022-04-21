#pragma once
#include <algorithm>
#include <vector>
#include<SquareBox-Core/Essentials.h>
#include <glm/glm.hpp>
namespace SquareBox {
	namespace GWOM {
		class Joint {
		public:
			Joint() {};
			~Joint() {};
			/*
			 let the joints data be kept by only body A when storing the joint as a reload able file , this will
			 make the recreation process manageable
			 but while the game is running both body A and body B will both have a reference to the joint , for easy referencing

			*/
			/*int*/
			std::pair<int, int> body_a_coordinates;
			std::pair<int, int> body_b_coordinates;
			/*floats*/
			/*bools*/
			bool collide_connected_bodies = false;
			bool is_joint_alive = false;
			float reference_angle = 0.0f;
			float frequency_hz = 4.0f;
			float damping_ratio = 0.5f;
			float max_length = 0.0f;
			float min_length = 0.0f;
			float length = 0.0f;
			float length_a = 0.0f;
			float length_b = 0.0f;
			float lower_angle = static_cast<float>(-0.5f * SBX_PI);
			float lower_translation = static_cast<float>(-0.5f * SBX_PI);
			float upper_angle = static_cast<float>(0.25f * SBX_PI);
			float upper_translation = static_cast<float>(0.25f * SBX_PI);
			float max_motor_torque = 10.0f;
			float max_motor_force = 10.0f;
			float motor_speed = 0.0f;
			bool enable_motor = false;
			bool enable_limit = true;
			glm::ivec4 color = glm::ivec4(255,255,255,255);
			float thickness = 1.0f;
			float pulley_ratio = 1.0f;
			float stiffness = 0.5f;
			float damping = 0.5f;
			/*glm::vecs*/
			glm::vec2 ground_anchor_a = glm::vec2(0.0f, 0.0f);
			glm::vec2 ground_anchor_b = glm::vec2(0.0f, 0.0f);
			glm::vec2 local_anchor_a = glm::vec2(0.0f, 0.0f);
			glm::vec2 local_anchor_b = glm::vec2(0.0f, 0.0f);
			std::vector<glm::vec2> vec_of_points;
			// the direction which B can move according to As point of view
			glm::vec2 local_axis_a = glm::vec2(0.0f, 0.0f);
			/*Enum*/
			/*Externals*/
			SquareBox::JointTypeEnum joint_type = SquareBox::JointTypeEnum::distance_joint;
			/*methods*/
		};
	};
};


