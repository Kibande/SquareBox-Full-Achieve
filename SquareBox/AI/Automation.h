#pragma once
#include <GWOM/Entities.h>
#include <string>
#include <memory>
#include <Lua/lua.hpp>
#include <vector>
#include <algorithm>
#include <map>
#include "IManipulator.h"
#include "MovementManipulator.h"
#include "TextureManipulator.h"
namespace SquareBox {
	namespace AI {
		enum class ManipulatorTypeEnum
		{
			movement_manipulator = 0,
			texture_manipulator = 1
		};

		struct LimboAutomation {
			std::string object_name = "NameLess";
			float desired_displacment_in_x = 0.0f;
			float desired_displacment_in_y = 0.0f;
			int start_tile = 0;
			int num_tiles = 1;
			float duration = 0.0f;
			ManipulatorTypeEnum manipulator_type;
			LimboAutomation(std::string object_name_, float desired_displacement_in_x_, float desired_displacement_in_y_, int start_tile_, int num_tiles_, float duration_, ManipulatorTypeEnum manipulator_type_) {
				object_name = object_name_;
				desired_displacment_in_x = desired_displacement_in_x_;
				desired_displacment_in_y = desired_displacement_in_y_;
				start_tile = start_tile_;
				num_tiles = num_tiles_;
				duration = duration_;
				manipulator_type = manipulator_type_;
			}
		};

		class Automation
		{
		public:
			void init();
			bool loadAutomationScript(std::string file_path_);
			void update(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, const std::vector<std::pair<int, int>> & alive_cluster_objects_,float delta_time_);
			void dispose();
		private:
			lua_State *m_l = nullptr;
			bool checkLua(lua_State *l, int r);
			std::vector<std::shared_ptr<SquareBox::AI::IManipultor>> m_manipulators;
		};
	}
}
