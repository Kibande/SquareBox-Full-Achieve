#pragma once
#include <Lua/lua.hpp>
#include "IAnimation.h"
#include "ForceDependantAnimation.h"
#include "TimeDependantAnimation.h"
#include "PropertiesDependantAnimation.h"
#include <memory>

namespace SquareBox {
	namespace AnimationSystem
	{
		struct LimboAnimation
		{
			std::string target_cluster_object = "";
			std::string source_animation_file = "";
			/*bools*/
			bool applyDamping = true;
			/*glm::vecs*/
			glm::vec2 maxLinearVelocity;
			/*Enums*/
			SquareBox::AnimationTypeEnum animation_type = SquareBox::AnimationTypeEnum::forceDependant;
			/*Externals*/
			std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications;
		};

		class AnimationCreator
		{
		public:
			void init();
			bool loadAnimationScript(std::string filePath_);
			void addNewLimboAnimation(LimboAnimation & new_limbo_animation_); // added for the editor
			void update(const float & deltatime_, std::vector<SquareBox::GWOM::WorldCluster>& worldClusters_, const  std::vector<std::pair<int, int >> & aliveClusterObjects_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_, bool run_animations_);
			void dispose();
			std::vector<std::shared_ptr<SquareBox::AnimationSystem::IAnimation>> m_animations;
		private:
			lua_State *L = nullptr;
			bool CheckLua(lua_State *L, int r);
		};
	};
}
