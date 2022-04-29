#include "TimeDependantAnimation.h"

SquareBox::AnimationSystem::TimeDependantAnimation::TimeDependantAnimation(std::string ClusterObjectName_, std::vector<SquareBox::AnimationSystem::AnimationSquence> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	animation_type = SquareBox::AnimationTypeEnum::timeDependant;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSequence(animation_specifications_[i]);
	}
	//for timeDependant Animations will set the first recieved animations as the default currentAnimations
	// though others can also still be set up later
	//we are doing these because we dont depend on move state here
	animationComplete = true;//Clear the way so that we can set the current animation
	if (animation_specifications_.size() > 0) {
		setAnimationSquence(m_currentAnimationSquence = animation_specifications_[0]);
	}
}

bool SquareBox::AnimationSystem::TimeDependantAnimation::Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	float perFrameSpeedGain = 0.0f;

	if (m_currentAnimationSquence.duration <= 0) {
		SBX_CORE_INFO("TimeDependantAnimation Can't have a duration of 0 seconds or less");
		animationComplete = true;
		return animationComplete;
	}

	perFrameSpeedGain = (1 / FPS_)*SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_currentAnimationSquence.numTiles), static_cast<float>(m_currentAnimationSquence.duration));
	//getting the UvCoords
	m_animeTime += perFrameSpeedGain;
	if (m_animeTime > m_currentAnimationSquence.numTiles) {
		m_animeTime = 0.0f;//when the animation is done we should just reset , which will loop stuff
	}
	auto retrieved_texture = AssetManager::TextureManager::getTextureById(clusterObject_.texture_info.texture_id);
	//Apply Animation
	m_currentTileIndex = m_currentAnimationSquence.startTile + (int)m_animeTime;
	//get the uvCoords
	SBX_CORE_ERROR("We need to update the the TimeDependantAnimation System");
	glm::vec4 uvRect = retrieved_texture.getUVReactAtIndex(m_currentTileIndex);

	//change spriteDirection
	if (!m_is_forward_direction)
	{
		//flip the uv
		uvRect.x += 1.0f / retrieved_texture.tiling.x;
		uvRect.z *= -1;
	}

	clusterObject_.texture_info.uv_rect = uvRect;
	return animationComplete;
}

void SquareBox::AnimationSystem::TimeDependantAnimation::setAnimationSquence(SquareBox::AnimationSystem::AnimationSquence animationSquence_)
{
	if (animationComplete)//can only set an animation when the current One is compete
	{
		m_currentAnimationSquence = animationSquence_;
		m_currentTileIndex = m_currentAnimationSquence.startTile;
		m_animeTime = 0.0f;
		animationComplete = false;
	}
}