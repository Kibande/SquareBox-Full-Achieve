#pragma once
#include "IAnimation.h"
#include <GWOM/Entities.h>
#include <SquareBox-Logger/Logger.h>
#include <Render-Engine/RenderingEngine.h>
#include <MathLib/MathLib.h>

/*
  This animations is used to customize how a physics Objects' texture coordinates are manipulated due to
  the various  directed thrusts a body expriences
  e.g which set of texture indexs to play over as a player which has physics is
  running (force in +x or -x)
  or
  jumping , falling or sqauting (force in +y or -y)

  Force Dependant only works with physics objects
	 requirements
	 A tile sheet
	 A well intialised physics Object
*/
namespace SquareBox {
	namespace AnimationSystem
	{
		class ForceDependantAnimation :public SquareBox::AnimationSystem::IAnimation
		{
		public:
			ForceDependantAnimation() {
				animation_type = AnimationTypeEnum::forceDependant;
			};

			ForceDependantAnimation(std::string ClusterObjectName_, std::pair<int, int> target_cluster_object_coordinates_, glm::vec2 maxLinearVelocity_, bool applyDamping_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_);
			// Inherited via IAnimation
			virtual bool Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_) override;
			void setMotionState(SquareBox::AnimationMotionStateEnum movementState_);
		private:
			float m_currentAnimeSpeed = 0.0f;//this is important , it helps us adjust speed accordig to velocity
			bool m_applyDamping = true;
			/*glm::vecs*/
			glm::vec2 m_maxLinearVelocity;
		};
	};
};
