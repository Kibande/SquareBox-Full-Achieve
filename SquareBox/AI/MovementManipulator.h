#pragma once
#include "IManipulator.h"
#include <GWOM/Entities.h>
#include <glm/glm.hpp>
#include <MathLib/MathLib.h>
namespace SquareBox {
	namespace AI {
		class MovementManipulator :public SquareBox::AI::IManipultor
		{
		public:
			MovementManipulator(SquareBox::GWOM::ClusterObject & cluster_object_, float desired_displacement_in_x_, float desired_displacement_in_y_, float duration_);

			// Inherited via IManipultor
			virtual bool update(float delta_time_, SquareBox::GWOM::ClusterObject & cluster_object_) override;
		private:
			glm::vec2 m_start_pos;
			glm::vec2 m_target_pos;
			float m_gradual_x_displacement = 0.0f;
			float m_accumulated_x = 0.0f;
			float m_accumulated_y  = 0.0f;
			float m_max_x_displacement = 0.0f;
			float m_max_y_displacement = 0.0f;
			float m_gradual_y_displacement = 0.0f;

		};
	};
};
