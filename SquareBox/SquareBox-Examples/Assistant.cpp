#include "Assistant.h"

void SquareBoxExample::Assistant::cameraControls(SquareBox::Camera::ParallelCamera& camera_, SquareBox::IMainGame* gamePtr_)
{

	float calculated_movement_speed = 1 / camera_.getScale();

	//camera panning
	if (
		(
			gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_1_of_or_input_key)
			||
			gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_2_of_or_input_key)
			)
		&&
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_panning_master_input_key)
		) {

		auto relation_motion = gamePtr_->getInputDevice()->getScreenLocations()[1].coordinates;
		camera_.offsetPosition(glm::vec2(-relation_motion.x, relation_motion.y * camera_.getAspectRatio()) * calculated_movement_speed);
		return;
	}
	//zoom
	if (
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_zooming_1_of_or_input_key)
		||
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_zooming_2_of_or_input_key)
		) {
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float scale_offset = mouse_wheel.y;
		if (std::abs(scale_offset)) {
			glm::vec2 mouse_in_world_before = camera_.convertScreenToWorld(gamePtr_->getInputDevice()->getScreenLocations()[0].coordinates);
			camera_.offsetScale(scale_offset);
			glm::vec2 mouse_in_world_after = camera_.convertScreenToWorld(gamePtr_->getInputDevice()->getScreenLocations()[0].coordinates);

			glm::vec2 dist_vec = mouse_in_world_before - mouse_in_world_after;
			camera_.offsetPosition(dist_vec);
			return;
		}
	}

	if (
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_motion_1_of_or_input_key)
		||
		gamePtr_->getInputDevice()->isInputIdBeingReceived(camera_motion_2_of_or_input_key)
		) {
		//check for movement in x axis first
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float offset_in_x = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(offset_in_x, 0));
	}
	else {
		//all movement is in the y axis
		auto mouse_wheel = gamePtr_->getInputDevice()->getPivotMotion();
		float offset_in_y = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(0, offset_in_y));
	}
}
