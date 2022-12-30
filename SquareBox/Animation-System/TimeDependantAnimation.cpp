#include "TimeDependantAnimation.h"

SquareBox::AnimationSystem::TimeDependantAnimation::TimeDependantAnimation(std::string ClusterObjectName_,std::pair<int,int> target_cluster_object_coordinates_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	target_cluster_object_coordinates = target_cluster_object_coordinates_;
	animation_type = SquareBox::AnimationTypeEnum::timeDependant;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSpecification(animation_specifications_[i]);
	}
	//for timeDependant Animations will set the first recieved animations as the default currentAnimations
	// though others can also still be set up later
	//we are doing these because we dont depend on move state here
	if (animation_specifications_.size() > 0) {
		setCurrentAnimationSpecification(0);
	}

	animation_type = AnimationTypeEnum::timeDependant;
}

void SquareBox::AnimationSystem::TimeDependantAnimation::Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	if (m_current_animation_specification_index >= vec_of_animation_specification.size()) {
		return;
	}

	SquareBox::AnimationSystem::AnimationSpecifications& focus_specification = vec_of_animation_specification[m_current_animation_specification_index];
	if (focus_specification.loops < 1) {
		return;
	}

	if (focus_specification.duration <= 0) {
		SBX_CORE_ERROR("TimeDependantAnimation Can't have a duration of 0 seconds or less");
	
		return;
	}

	float perFrameSpeedGain = (1 / FPS_)*SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(focus_specification.numTiles), static_cast<float>(focus_specification.duration));
	//getting the UvCoords
	m_animeTime += (perFrameSpeedGain * focus_specification.animationSpeed);
	if (m_animeTime > focus_specification.numTiles) {
		m_animeTime = 0.0f;
		focus_specification.loops += -1;
	}

	auto retrieved_texture = AssetManager::TextureManager::getTextureById(clusterObject_.texture_info.texture_id);
	//Apply Animation
	m_currentTileIndex = focus_specification.startTile + (int)m_animeTime;
	//get the uvCoords
	// 
	//SBX_CORE_ERROR("We need to update the the TimeDependantAnimation System"); to be doing what ??
	
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
