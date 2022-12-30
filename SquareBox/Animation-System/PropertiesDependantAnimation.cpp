#include "PropertiesDependantAnimation.h"

SquareBox::AnimationSystem::PropertiesDependantAnimation::PropertiesDependantAnimation(std::string ClusterObjectName_, std::pair<int, int> target_cluster_object_coordinates_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	target_cluster_object_coordinates = target_cluster_object_coordinates_;
	animation_type = SquareBox::AnimationTypeEnum::propertiesDependant;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSpecification(animation_specifications_[i]);
	}
	animation_type = AnimationTypeEnum::propertiesDependant;
}

void SquareBox::AnimationSystem::PropertiesDependantAnimation::Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	if (m_current_animation_specification_index>= vec_of_animation_specification.size()) {
		return;
	}
	/*
		The properties we are going to edit should not depend on the physics engine
		we want a cross clusterObject Animation implementation
	*/

	SquareBox::AnimationSystem::AnimationSpecifications& focus_specification = vec_of_animation_specification[m_current_animation_specification_index];
	if (focus_specification.loops < 1) {
		return;
	}
	
	if (focus_specification.isPositionEditing) {
		clusterObject_.position.x = clusterObject_.position.x + (focus_specification.gainInX * deltatime_ * focus_specification.animationSpeed);
		clusterObject_.position.y = clusterObject_.position.y + (focus_specification.gainInY * deltatime_ * focus_specification.animationSpeed);
	}

	clusterObject_.requesting_recreation = true;

	focus_specification.loops += -1;
}