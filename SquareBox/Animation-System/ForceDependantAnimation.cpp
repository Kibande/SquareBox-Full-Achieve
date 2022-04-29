#include "ForceDependantAnimation.h"
#include <Asset-Manager/Asset-Manager.h>
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
SquareBox::AnimationSystem::ForceDependantAnimation::ForceDependantAnimation(std::string ClusterObjectName_, glm::vec2 maxLinearVelocity_, bool applyDamping_, std::vector<SquareBox::AnimationSystem::AnimationSquence> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	animation_type = SquareBox::AnimationTypeEnum::forceDependant;
	m_applyDamping = applyDamping_;
	m_maxLinearVelocity = maxLinearVelocity_;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSequence(animation_specifications_[i]);
	}
}

bool SquareBox::AnimationSystem::ForceDependantAnimation::Update(const float & deltaTime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	//Update Our Animation State
	glm::vec2 linerVelocity;
	if (clusterObject_.physics_properties != nullptr && clusterObject_.physics_properties->isIntialised()) {
		linerVelocity = clusterObject_.physics_properties->getLinearVelocity();
		//loop through the animations squences
		bool timeToDamp = true;
		for (unsigned int i = 0; i < vec_of_animation_Squence.size(); i++)
		{
			bool animation_triggered = false;
			if (input_ != nullptr) {
				if (vec_of_animation_Squence[i].keyActivationSquence == 0) {
					if (input_->isInputIdReceived(vec_of_animation_Squence[i].keycode)) {
						animation_triggered = true;
					}
				}
				else if (vec_of_animation_Squence[i].keyActivationSquence == 1) {
					if (input_->isInputIdBeingReceived(vec_of_animation_Squence[i].keycode)) {
						animation_triggered = true;
					}
				}
			}
			else {
				SBX_CORE_ERROR("InputManager is null!");
				break;
			}

			if (animation_triggered) {
				if (vec_of_animation_Squence[i].isForwardMovementOrientied) {
					m_is_forward_direction = true;
				}
				else if (vec_of_animation_Squence[i].isReversedMovementOrientied) {
					m_is_forward_direction = false;
				}

				float reference_anim_speed = vec_of_animation_Squence[i].animationSpeed;

				if (vec_of_animation_Squence[i].isAnimationSpeedLinkedToVerticalVelocity) {
					vec_of_animation_Squence[i].animationSpeed = abs(linerVelocity.y)*vec_of_animation_Squence[i].animationSpeed;
				}

				if (vec_of_animation_Squence[i].isAnimationSpeedLinkedToHorizontalVelocity) {
					if (abs(linerVelocity.x) > 1.0f && ((linerVelocity.x > 0 && m_is_forward_direction) || (linerVelocity.x < 0 && !m_is_forward_direction < 0))) {
						vec_of_animation_Squence[i].animationSpeed = abs(linerVelocity.x)*vec_of_animation_Squence[i].animationSpeed;
					}
				}

				if (vec_of_animation_Squence[i].isGroundDependant) {
					if (clusterObject_.physics_properties->isOnGround()) {
						setMotionState(vec_of_animation_Squence[i].motionState);
						if (vec_of_animation_Squence[i].applyForceAccordingToMass) {
							clusterObject_.physics_properties->applyForceToCenter(glm::vec2(clusterObject_.physics_properties->getMass()*vec_of_animation_Squence[i].animationForce.x, clusterObject_.physics_properties->getMass()*vec_of_animation_Squence[i].animationForce.y), true);
						}
						else {
							clusterObject_.physics_properties->applyForceToCenter(vec_of_animation_Squence[i].animationForce, true);
						}
					}
				}
				else
				{
					//not ground dependant
					setMotionState(vec_of_animation_Squence[i].motionState);
					if (vec_of_animation_Squence[i].applyForceAccordingToMass) {
						clusterObject_.physics_properties->applyForceToCenter(glm::vec2(clusterObject_.physics_properties->getMass()*vec_of_animation_Squence[i].animationForce.x, clusterObject_.physics_properties->getMass()*vec_of_animation_Squence[i].animationForce.y), true);
					}
					else {
						clusterObject_.physics_properties->applyForceToCenter(vec_of_animation_Squence[i].animationForce, true);
					}
				}
				//restoring our animations speed
				vec_of_animation_Squence[i].animationSpeed = reference_anim_speed;
				timeToDamp = false;
			}
		}

		if (timeToDamp && m_applyDamping) {
			//Apply damping
			if (clusterObject_.physics_properties != nullptr) {
				glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x*0.95, linerVelocity.y);
				clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
			}
		}

		if (linerVelocity.x < -m_maxLinearVelocity.x) {
			if (clusterObject_.physics_properties != nullptr) {
				glm::vec2 tmpLinerVelocity = glm::vec2(-m_maxLinearVelocity.x, linerVelocity.y);
				clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
			}
		}
		else if (linerVelocity.x > m_maxLinearVelocity.x) {
			if (clusterObject_.physics_properties != nullptr) {
				glm::vec2 tmpLinerVelocity = glm::vec2(m_maxLinearVelocity.x, linerVelocity.y);

				clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
			}
		}

		if (linerVelocity.y < -m_maxLinearVelocity.y) {
			if (clusterObject_.physics_properties != nullptr) {
				glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x, -m_maxLinearVelocity.y);

				clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
			}
		}
		else if (linerVelocity.y > m_maxLinearVelocity.y) {
			if (clusterObject_.physics_properties != nullptr) {
				glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x, m_maxLinearVelocity.y);
				clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
			}
		}

		float perFrameSpeedGain = SquareBox::MathLib::Float::divideAndGetFloat(m_currentAnimeSpeed, m_currentAnimationSquence.numTiles);
		//	std::cout << m_currentAnimeSpeed << " : " << m_currentAnimationSquence.numTiles << " = " << perFrameSpeedGain << '\n';
			//getting the UvCoords
		m_animeTime += perFrameSpeedGain;
		if (m_animeTime > m_currentAnimationSquence.numTiles) {
			setMotionState(SquareBox::AnimationMotionStateEnum::stationary);
		}

		//Apply Animation
		m_currentTileIndex = m_currentAnimationSquence.startTile + m_animeTime;
		//should not go past the animations specified
		// this for the case were the stationary position is already our stationary position
		if (m_currentTileIndex > (m_currentAnimationSquence.startTile + m_currentAnimationSquence.numTiles)) {
			m_currentTileIndex = m_currentAnimationSquence.startTile + m_currentAnimationSquence.numTiles;
		}
		//std::cout << "SpeedGain : " << perFrameSpeedGain << " m_animeTime " << m_animeTime << " numtiles " << m_currentAnimationSquence.numTiles<<" index "<<m_currentTileIndex << '\n';

		//get the uvCoords
		auto retrieved_texture = AssetManager::TextureManager::getTextureById(clusterObject_.texture_info.texture_id);

		SBX_CORE_ERROR("We need to update the the ForceDependantAnimation System");
		glm::vec4 uvRect = retrieved_texture.getUVReactAtIndex(m_currentTileIndex);

		//change spriteDirection
		if (!m_is_forward_direction)
		{
			//flip the uv
			uvRect.x += 1.0f / retrieved_texture.tiling.x;
			uvRect.z *= -1;
		}

		clusterObject_.texture_info.uv_rect = uvRect;
	}
	else
	{
		SBX_CORE_INFO("Can't Do A force dependant Animation for a non physics Object {} ", animationObjectName.c_str());
	}

	return animationComplete;
}

void SquareBox::AnimationSystem::ForceDependantAnimation::setMotionState(SquareBox::AnimationMotionStateEnum movementState_)
{
	if (movementState_ != m_currentAnimationSquence.motionState)
	{
		bool animationDefined = false;
		for (unsigned int i = 0; i < vec_of_animation_Squence.size(); i++)
		{
			//get the animation squence for this movestate
			if (vec_of_animation_Squence[i].motionState == movementState_)
			{
				//make it our current, we we shall get all reference values
				m_currentAnimationSquence = vec_of_animation_Squence[i];
				animationDefined = true;
			}
		}
		if (!animationDefined) {
			SBX_CORE_INFO("%s animation not Defined ", ToString(movementState_));
		}
		//this will ensure we always start our animation at the right time
		m_animeTime = 0.0f;
		m_currentAnimeSpeed = m_currentAnimationSquence.animationSpeed;
		m_currentTileIndex = m_currentAnimationSquence.startTile;
	}
}