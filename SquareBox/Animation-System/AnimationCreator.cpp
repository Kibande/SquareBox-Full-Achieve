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

		int lua_HostLogFunction(lua_State* L)
		{
			std::string msg = (std::string)lua_tostring(L, 1);
			SBX_CORE_INFO(msg.c_str());
			return 0;//number of args passed back to lua
		}
		bool AnimationCreator::loadAnimationScript(std::string filePath_)
		{
			lua_register(L, "lua_HostLogFunction", lua_HostLogFunction);
			limboAnimations.clear();
			m_animations.clear();

			std::string lua_file_string = "";

			std::pair<char*, int> vertextShaderfileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(filePath_);
			char* lua_file_buffer = vertextShaderfileInfo.first;
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
							SquareBox::AnimationSystem::LimboAnimation& new_limbo_animation = limboAnimations.back();
							new_limbo_animation.source_animation_file = filePath_;
							lua_getglobal(L, "GetAnimation");
							if (lua_isfunction(L, -1)) {
								//push the arguments onto the stack
								lua_pushnumber(L, currentAnimationIndex);

								//call the function while specifying all its attributes
								if (CheckLua(L, lua_pcall(L, 1, 1, 0))) {
									//for the animation we have got
									lua_pushstring(L, "TargetClusterObject");
									lua_gettable(L, -2);
									new_limbo_animation.target_cluster_object = lua_tostring(L, -1);
									lua_pop(L, 1);

									lua_pushstring(L, "AnimationType");
									lua_gettable(L, -2);
									new_limbo_animation.animation_type = static_cast<SquareBox::AnimationTypeEnum>(lua_tointeger(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "ApplyDamping");
									lua_gettable(L, -2);
									new_limbo_animation.applyDamping = static_cast<bool>(lua_toboolean(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "MaxLinearVelocityX");
									lua_gettable(L, -2);
									new_limbo_animation.maxLinearVelocity.x = static_cast<float>(lua_tonumber(L, -1));
									lua_pop(L, 1);

									lua_pushstring(L, "MaxLinearVelocityY");
									lua_gettable(L, -2);
									new_limbo_animation.maxLinearVelocity.y = static_cast<float>(lua_tonumber(L, -1));
									lua_pop(L, 1);

									lua_getglobal(L, "GetNumAnimationSpecification");
									if (lua_isfunction(L, -1)) {
										lua_pushnumber(L, currentAnimationIndex);

										//call the function while specifying all its attributes
										if (CheckLua(L, lua_pcall(L, 1, 1, 0))) {
											int totalNumAnimationSpecifications = lua_tointeger(L, -1);
											for (unsigned int j = 0; j < totalNumAnimationSpecifications; j++)
											{
												new_limbo_animation.animation_specifications.emplace_back();
												int currentAnimationSpecificationIndex = new_limbo_animation.animation_specifications.size() - 1;

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
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].animationForce.x = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "AnimationForceY");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].animationForce.y = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "AnimationSpeed");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].animationSpeed = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsPositionEditing");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isPositionEditing = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "GainInX");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].gainInX = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "GainInY");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].gainInY = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "Duration");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].duration = lua_tointeger(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "ApplyForceAccordingToMass");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].applyForceAccordingToMass = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "MotionState");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].specification_id = lua_tointeger(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsAnimationSpeedLinkedToHorizontalVelocity");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isAnimationSpeedLinkedToHorizontalVelocity = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsAnimationSpeedLinkedToVerticalVelocity");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isAnimationSpeedLinkedToVerticalVelocity = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsForwardMovementOrientied");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isForwardMovementOrientied = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsGroundDependant");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isGroundDependant = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "IsReversedMovementOrientied");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].isReversedMovementOrientied = lua_toboolean(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "KeyActivationSquence");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].keyActivationSquence = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "KeyCode");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].keycode = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "NumTiles");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].numTiles = lua_tonumber(L, -1);
														lua_pop(L, 1);

														lua_pushstring(L, "StartTile");
														lua_gettable(L, -2);
														new_limbo_animation.animation_specifications[currentAnimationSpecificationIndex].startTile = lua_tonumber(L, -1);
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
				SBX_CORE_ERROR("Error while loading animation Script {} ", filePath_);
				return false;
			}
			return true;
		}

		void AnimationCreator::addNewLimboAnimation(LimboAnimation& new_limbo_animation_)
		{
			bool animation_type_already_defined = false;
			for (unsigned int i = 0; i < m_animations.size(); i++)
			{
				std::shared_ptr<SquareBox::AnimationSystem::IAnimation>& focus_animation = m_animations[i];

				if (focus_animation->animationObjectName == new_limbo_animation_.target_cluster_object)
				{
					if (focus_animation->animation_type==new_limbo_animation_.animation_type) {
						animation_type_already_defined = true;
						focus_animation->defineSpecification(new_limbo_animation_.animation_specifications.back());
					}
				}
			}

			if (!animation_type_already_defined) {
				new_limbo_animation_.source_animation_file = "Created by level Editor";
				limboAnimations.push_back(new_limbo_animation_);
			}
		}

		void AnimationCreator::update(const float& deltatime_, std::vector<SquareBox::GWOM::WorldCluster>& worldClusters_, const std::vector<std::pair<int, int>>& aliveClusterObjects_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice* input_, bool run_animations_)
		{
			//Update the Manipulators
			for (unsigned int i = 0; i < limboAnimations.size(); i++)
			{
				LimboAnimation& CLA = limboAnimations[i];

				//find the Object Referenced first
				for (unsigned int j = 0; j < aliveClusterObjects_.size(); j++)
				{
					SquareBox::GWOM::ClusterObject& cwobj = worldClusters_[aliveClusterObjects_[j].first].cluster_objects[aliveClusterObjects_[j].second];

					if (std::string(cwobj.name) == CLA.target_cluster_object) {
						if (CLA.animation_type == SquareBox::AnimationTypeEnum::forceDependant) {
							m_animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::ForceDependantAnimation>(CLA.target_cluster_object, aliveClusterObjects_[j], CLA.maxLinearVelocity, CLA.applyDamping, CLA.animation_specifications));
						}
						else if (CLA.animation_type == SquareBox::AnimationTypeEnum::timeDependant) {
							m_animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::TimeDependantAnimation>(CLA.target_cluster_object, aliveClusterObjects_[j], CLA.animation_specifications));
						}
						else if (CLA.animation_type == SquareBox::AnimationTypeEnum::propertiesDependant) {
							m_animations.emplace_back(std::make_unique<SquareBox::AnimationSystem::PropertiesDependantAnimation>(CLA.target_cluster_object, aliveClusterObjects_[j], CLA.animation_specifications));
						}
						else {
							SBX_CORE_INFO("Undefined Animation Type in {}", CLA.source_animation_file);
						}
					}
					else {
						SBX_CORE_ERROR("No Cluster Object found to match {} referrence {} animation script {} {}", CLA.target_cluster_object, CLA.source_animation_file, __FILE__, __LINE__);
					}
				}
			}
			limboAnimations.clear();

			bool animation_got_discarded = false;

			// this helps when the animation is ended externally 

			for (auto& a : m_animations) {
				if (a != nullptr) {//using remove if leaves behind a nullptr

					animation_got_discarded = a->discard_animation ? true : animation_got_discarded;
				}
			}

			if (run_animations_) {
				for (auto& a : m_animations) {
					if (a != nullptr) {//using remove if leaves behind a nullptr

						animation_got_discarded = a->discard_animation ? true : animation_got_discarded;

						SquareBox::GWOM::ClusterObject& cwobj = worldClusters_[a->target_cluster_object_coordinates.first].cluster_objects[a->target_cluster_object_coordinates.second];

						// if this is to ever give an error, it means that the coordinates system changed else where 
						// and we forgot to update the animation system.

						if (cwobj.name == a->animationObjectName) {

							a->Update(deltatime_, fElapsedTime, cwobj, FPS_, fElapsedTime, input_);
						}
					}
				}
	
			}
			
			if (animation_got_discarded) {
				std::vector<std::shared_ptr<SquareBox::AnimationSystem::IAnimation>> m_original_animations = m_animations;
				m_animations.clear();
				for (auto& a : m_original_animations) {
					bool keep = false;
					if (a != nullptr) {
						if (!a->discard_animation) {
							keep = true;
						}
					}

					if (keep) {
						m_animations.push_back(a);
					}
				}
			}
		}
		void AnimationCreator::dispose()
		{
			lua_close(L);
		}
		bool AnimationCreator::CheckLua(lua_State* L, int r)
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