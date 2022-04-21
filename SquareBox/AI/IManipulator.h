#pragma once
#include <string>
#include <GWOM/Entities.h>
#include <algorithm>
namespace SquareBox {
	namespace AI {
		class IManipultor
		{
		public:
			~IManipultor() {};
			float manipulation_duration = 0.0f;
			std::string manipulation_object_name;
			virtual bool update(float delta_time_, SquareBox::GWOM::ClusterObject & cluster_object_) = 0;
			bool manipulation_complete = false;
		};
	}
}