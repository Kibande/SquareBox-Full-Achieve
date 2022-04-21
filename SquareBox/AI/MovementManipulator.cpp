#include "MovementManipulator.h"
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>

namespace SquareBox {
	namespace AI {
		MovementManipulator::MovementManipulator(SquareBox::GWOM::ClusterObject & clusterObject_, float desired_displacement_in_x_, float desired_displacement_in_y_, float duration_)
		{
			manipulation_object_name = clusterObject_.name;
			if (clusterObject_.physics_properties != nullptr) {
				m_start_pos = clusterObject_.physics_properties->getXYPosition();
			}
			else {
				m_start_pos = clusterObject_.position;
			}

			m_target_pos = glm::vec2(m_start_pos.x + desired_displacement_in_x_, m_start_pos.y + desired_displacement_in_y_);
			manipulation_duration = duration_;
			manipulation_complete = false;
			m_max_x_displacement = std::abs(desired_displacement_in_x_);
			m_max_y_displacement = std::abs(desired_displacement_in_y_);
			m_gradual_x_displacement = SquareBox::MathLib::Float::divideAndGetFloat(desired_displacement_in_x_, duration_);
			m_gradual_y_displacement = SquareBox::MathLib::Float::divideAndGetFloat(desired_displacement_in_y_, duration_);

		}

		bool MovementManipulator::update(float delta_time_, SquareBox::GWOM::ClusterObject & clusterObject_)
		{
			//Edit the Cluster Objects Position From Here
			if (clusterObject_.physics_properties != nullptr) {
				SBX_CORE_WARN("MANUALLY DISPLACING PHYSICS OBJECT");
			}

			bool no_x_movement = true;
			bool no_y_movement = true;

			if (!(SquareBox::MathLib::Float::areFloatsEqual(clusterObject_.position.x, m_target_pos.x))) {
				float x_gain = m_gradual_x_displacement * delta_time_;
				clusterObject_.position.x += x_gain;
				m_accumulated_x += std::abs(x_gain);
				no_x_movement = false;  
				if (m_accumulated_x > m_max_x_displacement) {
					clusterObject_.position.x = m_target_pos.x;
					no_x_movement = true;
				}
			}

			if (!(SquareBox::MathLib::Float::areFloatsEqual(clusterObject_.position.y, m_target_pos.y))) {
				float y_gain = m_gradual_y_displacement * delta_time_;
				clusterObject_.position.y += y_gain;
				m_accumulated_y += std::abs(y_gain);
				no_y_movement = false;
				if (m_accumulated_y > m_max_y_displacement) {
					clusterObject_.position.y = m_target_pos.y;
					no_y_movement = true;
				}
			}

			manipulation_complete = no_x_movement && no_y_movement;
			return manipulation_complete;
		}
	};
};