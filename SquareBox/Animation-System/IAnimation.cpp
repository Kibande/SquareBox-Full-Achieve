#include "IAnimation.h"

void SquareBox::AnimationSystem::IAnimation::defineSpecification(AnimationSpecifications animationSpecification_)
{
	{
		bool absent = true;
		for (unsigned int i = 0; i < vec_of_animation_specification.size(); i++)
		{
			if (vec_of_animation_specification[i].specification_id == animationSpecification_.specification_id)
			{
				//replace in case we are re defining
				vec_of_animation_specification[i] = animationSpecification_;
				absent = false;
			}
		}

		if (absent) {
			vec_of_animation_specification.push_back(animationSpecification_);
		}
	}
}

void SquareBox::AnimationSystem::IAnimation::setCurrentAnimationSpecification(int specification_id_)
{
	if (specification_id_ != vec_of_animation_specification[m_current_animation_specification_index].specification_id)
	{
		bool animationDefined = false;
		for (unsigned int i = 0; i < vec_of_animation_specification.size(); i++)
		{
			//get the animation squence for this movestate
			if (vec_of_animation_specification[i].specification_id == specification_id_)
			{
				//make it our current, we we shall get all reference values
				m_current_animation_specification_index = i;
				m_animeTime = 0.0f;
				m_currentTileIndex = vec_of_animation_specification[m_current_animation_specification_index].startTile; // this helps with time dependants
				animationDefined = true;
			}
		}
		if (!animationDefined) {
			SBX_CORE_INFO("Index:{} animation Specification not Defined ", specification_id_);
		}
	}
}

void SquareBox::AnimationSystem::IAnimation::deleteAnimationSpecificationById(int specification_id_)
{

	bool animationDefined = false;
	for (unsigned int i = 0; i < vec_of_animation_specification.size(); i++)
	{
		//get the animation squence for this movestate
		if (vec_of_animation_specification[i].specification_id == specification_id_)
		{
			animationDefined = true;
			break;
		}
	}
	if (!animationDefined) {
		SBX_CORE_INFO("Index:{} animation Specification not Defined ", specification_id_);
	}
	else {
		std::vector<SquareBox::AnimationSystem::AnimationSpecifications> temp_vec_of_animation_specification = vec_of_animation_specification;
		vec_of_animation_specification.clear();
		for (unsigned int i = 0; i < temp_vec_of_animation_specification.size(); i++)
		{
			//get the animation squence for this movestate
			if (temp_vec_of_animation_specification[i].specification_id != specification_id_)
			{
				vec_of_animation_specification.push_back(temp_vec_of_animation_specification[i]);
			}
		}
	}
}

void SquareBox::AnimationSystem::IAnimation::resetAnimation()
{
	m_animeTime = 0.0f;
	vec_of_animation_specification.clear();
}