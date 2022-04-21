#include "Automation.h"

namespace SquareBox {
	namespace AI {
		std::vector<LimboAutomation> m_limbo_manipulators;

		void moveObject(std::string object_name_, float x_, float y_, float duration_) {
			m_limbo_manipulators.emplace_back(LimboAutomation(object_name_, x_, y_, 0, 0, duration_, ManipulatorTypeEnum::movement_manipulator));
			//std::cout << ObjectName_ << " Added to Limbo \n";
		}

		void changeObjectTexture(std::string object_name_, int start_tile_, int num_tiles_, float duration_) {
			m_limbo_manipulators.emplace_back(LimboAutomation(object_name_, 0, 0, start_tile_, num_tiles_, duration_, ManipulatorTypeEnum::texture_manipulator));
		}

		int wrap_MoveObject(lua_State* L) {
			//confirm we have the required arguments on the stack
			if (lua_gettop(L) != 4) return -1;
			std::string retrieved_lua_object_name = lua_tostring(L, 1);
			float retrieved_lua_x =static_cast<float>(lua_tonumber(L, 2));
			float retrieved_lua_y = static_cast<float>(lua_tonumber(L, 3));
			float retrieved_lua_duration = static_cast<float>(lua_tonumber(L, 4));
			moveObject(retrieved_lua_object_name, retrieved_lua_x, retrieved_lua_y, retrieved_lua_duration);
			return 0;
		}

		int wrap_ChangeObjectTexture(lua_State * L) {
			if (lua_gettop(L) != 4) return -1;
			std::string retrieved_lua_object_name = lua_tostring(L, 1);
			int retrieved_lua_start_tile = static_cast<int>(lua_tointeger(L, 2));
			int retrieved_lua_num_tiles = static_cast<int>(lua_tointeger(L, 3));
			float retrieved_lua_duration = static_cast<float>(lua_tonumber(L, 4));
			changeObjectTexture(retrieved_lua_object_name, retrieved_lua_start_tile, retrieved_lua_num_tiles, retrieved_lua_duration);
			return 0;

		}

		void Automation::init()
		{
			m_l = luaL_newstate();
			luaL_openlibs(m_l);//give our Lua Machine access to some math libs
		}
		bool Automation::loadAutomationScript(std::string filePath_)
		{
			//register the functions we want lua to be able to call

			lua_register(m_l, "_MoveObject", wrap_MoveObject);
			lua_register(m_l, "_ChangeObjectTexture", wrap_ChangeObjectTexture);
			m_manipulators.clear();
			m_limbo_manipulators.clear();

			std::string lua_file_string = "";

			std::pair<char *, int> lua_script_file_info = SquareBox::AssetManager::IOManager::getRawDataFromFile(filePath_);
			char * lua_script_file_buffer = lua_script_file_info.first;
			int lua_script_file_buffer_size = lua_script_file_info.second;
			if (lua_script_file_buffer != nullptr) {
				std::string raw_string_data((const char*)lua_script_file_buffer, lua_script_file_buffer_size);
				delete[] lua_script_file_buffer;
				lua_script_file_buffer = nullptr;
				lua_file_string = raw_string_data;
			}

			if (checkLua(m_l, luaL_dostring(m_l, lua_file_string.c_str()))) {

				return true;
			}
			SBX_CORE_ERROR("Error while loading automation Script {} ", filePath_);
			return false;
		}
		void Automation::update(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, const std::vector<std::pair<int, int >> & alive_cluster_objects_, float delta_time_)
		{
			//create a map to make accesing faster 
			std::map<std::string, SquareBox::GWOM::ClusterObject * > alive_cluster_objects_map;
			for (unsigned int i = 0; i < alive_cluster_objects_.size(); i++)
			{
				SquareBox::GWOM::ClusterObject & cwobj = world_clusters_[alive_cluster_objects_[i].first].cluster_objects[alive_cluster_objects_[i].second];
				alive_cluster_objects_map[cwobj.name] = &cwobj;
			}

			//Update the Manipulators
			for (unsigned int i = 0; i < m_limbo_manipulators.size(); i++)
			{
				LimboAutomation & CLA = m_limbo_manipulators[i];
				//find the Object Referenced first
				auto it = alive_cluster_objects_map.find(CLA.object_name);

				if (it != alive_cluster_objects_map.end()) {
					//add it to the vector of Manipulators
					if (CLA.manipulator_type == ManipulatorTypeEnum::movement_manipulator) {
						m_manipulators.emplace_back(std::make_shared<SquareBox::AI::MovementManipulator>((*it->second), CLA.desired_displacment_in_x, CLA.desired_displacment_in_y, CLA.duration));
						//std::cout << it->second.name << " Added to vec of manipulators " << std::endl;
					}
					else if (CLA.manipulator_type == ManipulatorTypeEnum::texture_manipulator) {
						if ((*it->second).texture_info.texture_type == SquareBox::TextureEnum::TILESHEET) {
							m_manipulators.emplace_back(std::make_shared<SquareBox::AI::TextureManipulator>((*it->second), CLA.start_tile, CLA.num_tiles, CLA.duration));
						}
						else {
							std::string error_objects_name = (*it->second).name;
							SBX_CORE_ERROR("Failed to set texture automation for {} ", error_objects_name.c_str());
							SBX_CORE_ERROR("Only {} Texture types allowed ", ToString(SquareBox::TextureEnum::TILESHEET));
						}
					}
					else {
						SBX_CORE_INFO("Undefined Manipultor type");
					}
				}
				else {
					//std::cout << it.second.name << " Not Equal To  "<< CLA.ObjectName <<" So skipped" << std::endl;
				}
			}

			//clear the former list
			m_limbo_manipulators.clear();

			for (auto& m : m_manipulators) {
				if (m != nullptr) {//using remove if leaves behind a nullptr
					bool not_found = true;

					auto it = alive_cluster_objects_map.find(m->manipulation_object_name);

					if (it != alive_cluster_objects_map.end()) {
						not_found = false;
						if (m->update(delta_time_, (*it->second))) {
							//std::cout << "length of Manipulators " << m_Manipulators.size() << std::endl;
							//Manipulation has Completed , request Next
							lua_getglobal(m_l, "IssueNextTask");
							if (lua_isfunction(m_l, -1)) {
								lua_pushstring(m_l, m->manipulation_object_name.c_str());
								if (!checkLua(m_l, lua_pcall(m_l, 1, 0, 0)))
								{
									//Script was bad
									SBX_ERROR("Bad Script Encountered when IssueNextTask called for {} ", m->manipulation_object_name.c_str());
								}
							}

							m = nullptr;//so that we skip this in the next run
						}
					}

					if (not_found) {
						m->manipulation_complete = true;//its time here is done
					}
				}
			}

			//Remove Completed manipulators, we are checking for also null instances due to using remove_if
			std::remove_if(m_manipulators.begin(), m_manipulators.end(), [](const std::shared_ptr<SquareBox::AI::IManipultor>& m) {
				return m == nullptr ? true : m->manipulation_complete; });
		}
		void Automation::dispose()
		{
			lua_close(m_l);
		}
		bool Automation::checkLua(lua_State * L, int r)
		{
			//since every Lua Command returns a number
			//this should be an effective error checker
			if (r != LUA_OK) {
				std::string errormsg = lua_tostring(L, -1);
				SBX_CORE_INFO(errormsg.c_str());
				return false;
			}
			return true;
		}
	}
}