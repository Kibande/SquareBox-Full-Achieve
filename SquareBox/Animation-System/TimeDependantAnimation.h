#pragma once
#include "IAnimation.h"
#include <SquareBox-Logger/Logger.h>
#include <Render-Engine/RenderingEngine.h>
#include<MathLib/MathLib.h>
/*
	 time dependant animations are ment to be used for when we want a cluster objects
	 texture to change periodically over time which will create an animation effect

	 for example a glowing bulb , where we transission form a dime texture to a bright one and then back again
	 the only clusterObject attribute we shall be affecting here is the texture uvCoordinates, that is  all

	 requirements
	 A tile sheet
	 */
namespace SquareBox {
	namespace AnimationSystem
	{
		class TimeDependantAnimation :public SquareBox::AnimationSystem::IAnimation
		{
		public:
			TimeDependantAnimation(std::string ClusterObjectName_,std::pair<int, int> target_cluster_object_coordinates_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_);

			// Inherited via IAnimation
			virtual bool Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_) override;
			void setAnimationSquence(SquareBox::AnimationSystem::AnimationSpecifications animationSquence_);
		};
	};
};
