#pragma once
#include "IAnimation.h"
#include <GWOM/Entities.h>
#include <SquareBox-Logger/Logger.h>
#include <Render-Engine/RenderingEngine.h>
#include <MathLib/MathLib.h>

/*
	This animation is used to change a ClusterObjects Properties like position,
	scaling etc. It carries out this changes when the correct input is received
	we then recreate the object with these new properties , the objects Texure
	uvcoordinates are also edited as need be
*/
namespace SquareBox {
	namespace AnimationSystem
	{
		class PropertiesDependantAnimation :public SquareBox::AnimationSystem::IAnimation
		{
		public:
			PropertiesDependantAnimation(std::string ClusterObjectName_, std::vector<SquareBox::AnimationSystem::AnimationSquence> animation_specifications_);
			// Inherited via IAnimation
			virtual bool Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInput * input_) override;
		private:
		};
	};
};