#include "ForceDependantAnimation.h"
#include <Asset-Manager/Asset-Manager.h>
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
SquareBox::AnimationSystem::ForceDependantAnimation::ForceDependantAnimation(std::string ClusterObjectName_, std::pair<int, int> target_cluster_object_coordinates_, glm::vec2 maxLinearVelocity_, bool applyDamping_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	target_cluster_object_coordinates = target_cluster_object_coordinates_;
	animation_type = SquareBox::AnimationTypeEnum::forceDependant;
	m_applyDamping = applyDamping_;
	m_maxLinearVelocity = maxLinearVelocity_;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSpecification(animation_specifications_[i]);
	}
	animation_type = AnimationTypeEnum::forceDependant;
}

void SquareBox::AnimationSystem::ForceDependantAnimation::Update(const float & deltaTime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	//SquareBox::AnimationSystem::AnimationSpecifications& focus_animation = vec_of_animation_specification[m_current_animation_specification_index];

	////Update Our Animation State
	//glm::vec2 linerVelocity;
	//if (clusterObject_.physics_properties != nullptr && clusterObject_.physics_properties->isIntialised()) {
	//	linerVelocity = clusterObject_.physics_properties->getLinearVelocity();
	//	//loop through the animations squences
	//	bool timeToDamp = true;
	//	for (unsigned int i = 0; i < vec_of_animation_specification.size(); i++)
	//	{
	//		bool animation_triggered = false;
	//		if (input_ != nullptr) {
	//			if (vec_of_animation_specification[i].keyActivationSquence == 0) {
	//				if (input_->isInputIdReceived(vec_of_animation_specification[i].keycode)) {
	//					animation_triggered = true;
	//				}
	//			}
	//			else if (vec_of_animation_specification[i].keyActivationSquence == 1) {
	//				if (input_->isInputIdBeingReceived(vec_of_animation_specification[i].keycode)) {
	//					animation_triggered = true;
	//				}
	//			}
	//		}
	//		else {
	//			SBX_CORE_ERROR("InputManager is null!");
	//			break;
	//		}

	//		if (animation_triggered) {
	//			if (vec_of_animation_specification[i].isForwardMovementOrientied) {
	//				m_is_forward_direction = true;
	//			}
	//			else if (vec_of_animation_specification[i].isReversedMovementOrientied) {
	//				m_is_forward_direction = false;
	//			}

	//			float reference_anim_speed = vec_of_animation_specification[i].animationSpeed;

	//			if (vec_of_animation_specification[i].isAnimationSpeedLinkedToVerticalVelocity) {
	//				vec_of_animation_specification[i].animationSpeed = abs(linerVelocity.y)*vec_of_animation_specification[i].animationSpeed;
	//			}

	//			if (vec_of_animation_specification[i].isAnimationSpeedLinkedToHorizontalVelocity) {
	//				if (abs(linerVelocity.x) > 1.0f && ((linerVelocity.x > 0 && m_is_forward_direction) || (linerVelocity.x < 0 && !m_is_forward_direction < 0))) {
	//					vec_of_animation_specification[i].animationSpeed = abs(linerVelocity.x)*vec_of_animation_specification[i].animationSpeed;
	//				}
	//			}

	//			if (vec_of_animation_specification[i].isGroundDependant) {
	//				if (clusterObject_.physics_properties->isOnGround()) {
	//					setCurrentAnimationSpecification(vec_of_animation_specification[i].specification_id);
	//					m_animeTime = 0.0f;
	//					m_currentAnimeSpeed = vec_of_animation_specification[m_current_animation_specification_index].animationSpeed;
	//					m_currentTileIndex = vec_of_animation_specification[m_current_animation_specification_index].startTile;

	//					if (vec_of_animation_specification[i].applyForceAccordingToMass) {
	//						clusterObject_.physics_properties->applyForceToCenter(glm::vec2(clusterObject_.physics_properties->getMass()*vec_of_animation_specification[i].animationForce.x, clusterObject_.physics_properties->getMass()*vec_of_animation_specification[i].animationForce.y), true);
	//					}
	//					else {
	//						clusterObject_.physics_properties->applyForceToCenter(vec_of_animation_specification[i].animationForce, true);
	//					}
	//				}
	//			}
	//			else
	//			{
	//				//not ground dependant
	//				setCurrentAnimationSpecification(vec_of_animation_specification[i].specification_id);
	//				m_animeTime = 0.0f;
	//				m_currentAnimeSpeed = vec_of_animation_specification[m_current_animation_specification_index].animationSpeed;
	//				m_currentTileIndex = vec_of_animation_specification[m_current_animation_specification_index].startTile;

	//				if (vec_of_animation_specification[i].applyForceAccordingToMass) {
	//					clusterObject_.physics_properties->applyForceToCenter(glm::vec2(clusterObject_.physics_properties->getMass()*vec_of_animation_specification[i].animationForce.x, clusterObject_.physics_properties->getMass()*vec_of_animation_specification[i].animationForce.y), true);
	//				}
	//				else {
	//					clusterObject_.physics_properties->applyForceToCenter(vec_of_animation_specification[i].animationForce, true);
	//				}
	//			}
	//			//restoring our animations speed
	//			vec_of_animation_specification[i].animationSpeed = reference_anim_speed;
	//			timeToDamp = false;
	//		}
	//	}

	//	if (timeToDamp && m_applyDamping) {
	//		//Apply damping
	//		if (clusterObject_.physics_properties != nullptr) {
	//			glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x*0.95, linerVelocity.y);
	//			clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
	//		}
	//	}

	//	if (linerVelocity.x < -m_maxLinearVelocity.x) {
	//		if (clusterObject_.physics_properties != nullptr) {
	//			glm::vec2 tmpLinerVelocity = glm::vec2(-m_maxLinearVelocity.x, linerVelocity.y);
	//			clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
	//		}
	//	}
	//	else if (linerVelocity.x > m_maxLinearVelocity.x) {
	//		if (clusterObject_.physics_properties != nullptr) {
	//			glm::vec2 tmpLinerVelocity = glm::vec2(m_maxLinearVelocity.x, linerVelocity.y);

	//			clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
	//		}
	//	}

	//	if (linerVelocity.y < -m_maxLinearVelocity.y) {
	//		if (clusterObject_.physics_properties != nullptr) {
	//			glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x, -m_maxLinearVelocity.y);

	//			clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
	//		}
	//	}
	//	else if (linerVelocity.y > m_maxLinearVelocity.y) {
	//		if (clusterObject_.physics_properties != nullptr) {
	//			glm::vec2 tmpLinerVelocity = glm::vec2(linerVelocity.x, m_maxLinearVelocity.y);
	//			clusterObject_.physics_properties->setLinerVelocity(tmpLinerVelocity);
	//		}
	//	}

	//	float perFrameSpeedGain = SquareBox::MathLib::Float::divideAndGetFloat(m_currentAnimeSpeed, focus_animation.numTiles);
	//	//	std::cout << m_currentAnimeSpeed << " : " << focus_animation.numTiles << " = " << perFrameSpeedGain << '\n';
	//		//getting the UvCoords
	//	m_animeTime += perFrameSpeedGain;
	//

	//	//Apply Animation
	//	m_currentTileIndex = focus_animation.startTile + m_animeTime;
	//	//should not go past the animations specified
	//	// this for the case were the stationary position is already our stationary position
	//	if (m_currentTileIndex > (focus_animation.startTile + focus_animation.numTiles)) {
	//		m_currentTileIndex = focus_animation.startTile + focus_animation.numTiles;
	//	}
	//	//std::cout << "SpeedGain : " << perFrameSpeedGain << " m_animeTime " << m_animeTime << " numtiles " << focus_animation.numTiles<<" index "<<m_currentTileIndex << '\n';

	//	//get the uvCoords
	//	auto retrieved_texture = AssetManager::TextureManager::getTextureById(clusterObject_.texture_info.texture_id);

	//	glm::vec4 uvRect = retrieved_texture.getUVReactAtIndex(m_currentTileIndex);

	//	//change spriteDirection
	//	if (!m_is_forward_direction)
	//	{
	//		//flip the uv
	//		uvRect.x += 1.0f / retrieved_texture.tiling.x;
	//		uvRect.z *= -1;
	//	}

	//	clusterObject_.texture_info.uv_rect = uvRect;
	//}
	//else
	//{
	//	SBX_CORE_INFO("Can't Do A force dependant Animation for a non physics Object {} ", animationObjectName.c_str());
	//}

	//return discard_animation;
}

