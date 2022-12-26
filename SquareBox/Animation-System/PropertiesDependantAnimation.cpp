#include "PropertiesDependantAnimation.h"

SquareBox::AnimationSystem::PropertiesDependantAnimation::PropertiesDependantAnimation(std::string ClusterObjectName_, std::pair<int, int> target_cluster_object_coordinates_, std::vector<SquareBox::AnimationSystem::AnimationSpecifications> animation_specifications_)
{
	animationObjectName = ClusterObjectName_;
	target_cluster_object_coordinates = target_cluster_object_coordinates_;
	animation_type = SquareBox::AnimationTypeEnum::propertiesDependant;
	for (unsigned int i = 0; i < animation_specifications_.size(); i++)
	{
		defineSequence(animation_specifications_[i]);
	}
	animation_type = AnimationTypeEnum::propertiesDependant;
}

bool SquareBox::AnimationSystem::PropertiesDependantAnimation::Update(const float & deltatime_, float currentGameLoopElapsedTime_, SquareBox::GWOM::ClusterObject & clusterObject_, float FPS_, float fElapsedTime, SquareBox::InputManager::IInputDevice * input_)
{
	/*
		The properties we are going to edit should not depend on the physics engine
		we want a cross clusterObject Animation implementation
	*/
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
			//time to play
			/*
			so fare we have only implemented position Editing
			We Shall add others when need arises
			*/
			//teleportation
			if (vec_of_animation_Squence[i].isPositionEditing) {
				clusterObject_.position.x = clusterObject_.position.x + (vec_of_animation_Squence[i].gainInX* deltatime_);
				clusterObject_.position.y = clusterObject_.position.y + (vec_of_animation_Squence[i].gainInY* deltatime_);
			}
			clusterObject_.requesting_recreation = true;
		}
	}

	return false;//we want this animation to never expire
}