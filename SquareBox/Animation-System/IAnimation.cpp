#include "IAnimation.h"

void SquareBox::AnimationSystem::IAnimation::defineSequence(AnimationSquence animationSequence_)
{
	{
		bool absent = true;
		for (unsigned int i = 0; i < vec_of_animation_Squence.size(); i++)
		{
			if (vec_of_animation_Squence[i].motionState == animationSequence_.motionState)
			{
				//replace in case we are re defining
				vec_of_animation_Squence[i] = animationSequence_;
				absent = false;
			}
		}

		if (absent) {
			vec_of_animation_Squence.push_back(animationSequence_);
		}
	}
}

void SquareBox::AnimationSystem::IAnimation::resetAnimation()
{
	m_animeTime = 0.0f;
	vec_of_animation_Squence.clear();
}