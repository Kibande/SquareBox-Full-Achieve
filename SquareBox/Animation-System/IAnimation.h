#pragma once
#include <string>
#include <GWOM/Entities.h>
#include <Input-Manager/InputManager.h>
namespace SquareBox {
	namespace AnimationSystem
	{
		struct AnimationSpecifications {
		public:
			AnimationMotionStateEnum motionState = AnimationMotionStateEnum::stationary;
			float animationSpeed = 0.2f;
			bool isForwardMovementOrientied = true;
			bool isReversedMovementOrientied = false;
			bool isGroundDependant = true;
			bool isAnimationSpeedLinkedToVerticalVelocity = false;
			bool isAnimationSpeedLinkedToHorizontalVelocity = false;
			bool applyForceAccordingToMass = false;
			bool isPositionEditing = false;
			float gainInX = 0.0f;
			float gainInY = 0.0f;
			int numTiles = 1;
			int startTile = 1;
			//Controller
			int keyActivationSquence = 0;
			/*
			0 - onTap
			1 - onLongPress
			2 - isBeingPressed
			*/
			int keycode = 0;
			//physics
			//if applyingForceAccordingToMass This will be taken as a rate
			glm::vec2 animationForce = glm::vec2(0.0f);
			int duration = 1;
		};

		class IAnimation
		{
		public:
			~IAnimation() {};
			std::string animationObjectName;
			std::pair<int, int> target_cluster_object_coordinates;
			virtual bool Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_) = 0;
			void defineSequence(AnimationSpecifications animationSequence_);
			void resetAnimation();
			int m_current_animation_squence_index = 0;
			std::vector<SquareBox::AnimationSystem::AnimationSpecifications> vec_of_animation_Squence;
			/*Enums*/
			SquareBox::AnimationTypeEnum animation_type;
			int m_currentTileIndex = 1;
			float m_animeTime = 0.0f;
			bool animationComplete = false;
			float m_is_forward_direction = true;
		};
	};
};
