#include "AnimationCreator.h"
namespace SquareBox {
	namespace AnimationSystem
	{
		std::vector<SquareBox::AnimationSystem::LimboAnimation> limboAnimations;

		void AnimationCreator::init()
		{
			L = luaL_newstate();
			luaL_openlibs(L);//give our Lua Machine access to some math libs
		}

		int lua_HostLogFunction(lua_State * L)
		{
			std::string msg = (std::string)lua_tostring(L, 1);
			SBX_CORE_INFO(msg.c_str());
			return 0;//number of args passed back to lua
		}
		bool AnimationCreator::loadAnimationScript(std::string filePath_)
		{
			lua_register(L, "lua_HostLogFunction", lua_HostLogFunction);
			limboAnimations.clear();
			m_Animations.clear();

			std::string lua_file_string = "";

			std::pair<char *, int> vertextShaderfileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(filePath_);
			char * lua_file_buffer = vertextShaderfileInfo.first;
			int lua_file_buffer_size = vertextShaderfileInfo.second;
			if (lua_file_buffer != nullptr) {
				std::string vex((const char*)lua_file_buffer, lua_file_buffer_size);
				delete[] lua_file_buffer;
				lua_file_string = vex;
			}

			if (CheckLua(L, luaL_dostring(L, lua_file_string.c_str()))) {
				lua_getglobal(L, "GetNumAnimations");
				if (lua_isfunction(L, -1)) {
					//call the function while specifying all its attributes
					if (CheckLua(L, lua_pcall(L, 0, 1, 0))) {
						int totalNumAnimations = lua_tointeger(L, -1);
						for (unsigned int i = 0; i < totalNumAnimations; i++)
						{
							limboAnimations.emplace_back();
							int currentAnimationIndex = limboAnimations.size() - 1;
							lua_getglobal(L, "GetAnimation");
							if (lua_isfunction(L, -1)) {
								//push the arguments onto the stack
								lua_pushnumber(L, currentAnimationIndex);

								//call the function while specifying all its attributes
								if (CheckLua(L, lua_pcall(L, 1, 1, 0))) {
									//for the animation we have got
									lua_pushstring(L, "TargetClusterObject");
									lua_gettable(L, -2);
									limboAnimations[currentAnimationIndex].target_cluster_object = lua_tostring(L, -1);
									lua_pop(L, 1);

									lua_pushstring(L, "AnimationType");
									lua_gettable(L, -2);
									limboAnimations[currentAnimationIndex].animation_type = static_cast<SquareBox::AnimationTypeEnum>(lua_tointeger(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "ApplyDamping");
									lua_gettable(L, -2);
									limboAnimations[currentAnimationIndex].applyDamping = static_cast<bool>(lua_toboolean(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "MaxLinearVelocityX");
									lua_gettable(L, -2);
									limboAnimations[currentAnimationIndex].maxLinearVelocity.x = static_cast<float>(lua_tonumber(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "MaxLinearVelocityY");
									lua_gettable(L, -2);
									limboAnimations[currentAnimationIndex].maxLinearVelocity.y = static_cast<float>(lua_tonumber(L, -1));
									lua_pop(L, 1);

									lua_getglobal(L, "GetNumAnimationSpecification");
									if (lua_isfunction(L, -1)) {
										lua_pushnumber(L, currentAnimationIndex);

										//call the function while specifying all its attributes
										if (CheckLua(L, lua_pcall(L, 1, 1, 0))) {
											int totalNumAnimationSpecifications = lua_tointeger(L, -1);
											for (unsigned int j = 0; j < totalNumAnimationSpecifications; j++)
											{
												limboAnimations[currentAnimationIndex].animation_specifications.emplace_back();
												int currentAnimationSpecificationIndex = limboAnimations[currentAnimationIndex].animation_specifications.size() - 1;

												lua_getglobal(L, "GetAnimationSpecification");
												if (lua_isfunction(L, -1)) {
													//push the arguments onto the stack
													lua_pushnumber(L, currentAnimationIndex);
													lua_pushnumber(L, currentAnimationSpecificationIndex);

													//call the function while specifying all its attributes
													if (CheckLua(L, lua_pcall(L, 2, 1, 0))) {
														//for the animation Specifications we have got
														lua_pushstring(L, "AnimationForceX");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].animationForce.x = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "AnimationForceY");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].animationForce.y = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "AnimationSpeed");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].animationSpeed = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsPositionEditing");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isPositionEditing = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "GainInX");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].gainInX = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "GainInY");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].gainInY = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "Duration");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].duration = lua_tointeger(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "ApplyForceAccordingToMass");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].applyForceAccordingToMass = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "MotionState");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].motionState = static_cast<SquareBox::AnimationMotionStateEnum>(lua_tointeger(L, -1));
														lua_pop(L, 1);

														lua_pushstring(L, "IsAnimationSpeedLinkedToHorizontalVelocity");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isAnimationSpeedLinkedToHorizontalVelocity = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsAnimationSpeedLinkedToVerticalVelocity");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isAnimationSpeedLinkedToVerticalVelocity = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsForwardMovementOrientied");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isForwardMovementOrientied = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsGroundDependant");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isGroundDependant = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsReversedMovementOrientied");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].isReversedMovementOrientied = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "KeyActivationSquence");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].keyActivationSquence = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "KeyCode");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].keycode = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "NumTiles");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].numTiles = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "StartTile");
														lua_gettable(L, -2);
														limboAnimations[currentAnimationIndex].animation_specifications[currentAnimationSpecificationIndex].startTile = lua_tonumber(L, -1);
														lua_pop(L, 1);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else {
			SBX_CORE_ERROR("Error while loading animation Script {} ",filePath_);
				return false;
			}
			return true;
		}
		void AnimationCreator::update(const float & deltatime_, std::vector<SquareBox::GWOM::WorldCluster>& worldClusters_, const std::vector<std::pair<int, int>>& aliveClusterObjects_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
		{
			//Update the Manipulators
			for (unsigned int i = 0; i < limboAnimations.size(); i++)
			{
				LimboAnimation & CLA = limboAnimations[i];

				//find the Object Referenced first
				for (unsigned int j = 0; j < aliveClusterObjects_.size(); j++)
				{
					SquareBox::GWOM::ClusterObject & cwobj = worldClusters_[aliveClusterObjects_[j].first].cluster_objects[aliveClusterObjects_[j].second];
					if (std::string(cwobj.name) == CLA.target_cluster_object) {
						if (CLA.animation_type == SquareBox::AnimationTypeEnum::forceDependant) {
							m_Animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::ForceDependantAnimation>(CLA.target_cluster_object, CLA.maxLinearVelocity, CLA.applyDamping, CLA.animation_specifications));
							//	std::cout << cwobj.name << " Added to vec of Animtions " << std::endl;
						}
						else if (CLA.animation_type == SquareBox::AnimationTypeEnum::timeDependant) {
							m_Animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::TimeDependantAnimation>(CLA.target_cluster_object, CLA.animation_specifications));
							//	std::cout << cwobj.name << " Added to vec of Animtions " << std::endl;
						}
						else if (CLA.animation_type == SquareBox::AnimationTypeEnum::propertiesDependant) {
							m_Animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::PropertiesDependantAnimation>(CLA.target_cluster_object, CLA.animation_specifications));
							//	std::cout << cwobj.name << " Added to vec of Animtions " << std::endl;
						}
						else {
							SBX_CORE_INFO("Undefined Animation Type");
						}
					}
					else {
						//	std::cout << cwobj.name << " Not Equal To  " << CLA.target_cluster_object << " So skipped" << std::endl;
					}
				}
			}

			limboAnimations.clear();

			for (auto& a : m_Animations) {
				if (a != nullptr) {//using remove if leaves behind a nullptr
					bool notFound = true;

					for (unsigned int i = 0; i < aliveClusterObjects_.size() && a != nullptr; i++)
					{
						SquareBox::GWOM::ClusterObject & cwobj = worldClusters_[aliveClusterObjects_[i].first].cluster_objects[aliveClusterObjects_[i].second];
						if (cwobj.name == a->animationObjectName) {
							notFound = false;
							if (a->Update(deltatime_, fElapsedTime, cwobj, FPS_, fElapsedTime, input_)) {
								//	std::cout << "Animation Completed for "<<a->animationObjectName << std::endl;
									//Remove from m_Animations
								a = nullptr;//so that we skip this in the next run , the best option is to
								//ofcourse fix our remove if to erase if
							}
						}
					}

					if (notFound) {
						a->animationComplete = true;//its time here is done
					}
				}
			}

			//Remove Completed manipulators, we are checking for also null instances due to using remove_if
			std::remove_if(m_Animations.begin(), m_Animations.end(), [](const std::shared_ptr<SquareBox::AnimationSystem::IAnimation>& a) {
				return a == nullptr ? true : a->animationComplete; });
		}
		void AnimationCreator::dispose()
		{
			lua_close(L);
		}
		bool AnimationCreator::CheckLua(lua_State * L, int r)
		{
			//since every Lua Command return a number
		//this should be an effective error checker
			if (r != LUA_OK) {
				std::string errormsg = lua_tostring(L, -1);
				SBX_CORE_INFO(errormsg.c_str());
				return false;
			}
			return true;
		}
	};
}