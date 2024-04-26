#include "GameLogic.h"

namespace Platformer {




	void GameLogic::init(SquareBox::IMainGame* game_ptr_,SquareBox::Camera::ParallelCamera & game_play_camera_)
	{

		controlContinue = false;
		controlCrouch = false;
		controlGetUp = false;
		controlJump = false;
		controlReload = false;
		controlSelectGun = false;
		controlNextGun = false;
		controlPreviousGun = false;
		toggleFocus = false;
		controlMoveX = 0;

		glm::vec2 mouse_in_world = game_play_camera_.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

		controlAimX = mouse_in_world.x + halfCursorSide;
		controlAimY = mouse_in_world.y + halfCursorSide;
	}

	void GameLogic::update(SquareBox::Camera::ParallelCamera& game_play_camera_)
	{
		controlContinue = false;
		controlCrouch = false;
		controlGetUp = false;
		controlJump = false;
		controlReload = false;
		controlSelectGun = false;
		controlNextGun = false;
		controlPreviousGun = false;
		toggleFocus = false;


		

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))) {
			controlShoot = true;
		}

		auto mouse_wheel_y = m_game_ptr->getInputDevice()->getPivotMotion().y;

		if (mouse_wheel_y < 0) controlNextGun = true;
		if (mouse_wheel_y > 0) controlPreviousGun = true;

	

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_d))) {
			controlMoveX = 1;
		}


		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_a))) {
			controlMoveX = - 1;
		}

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_w))) {
			controlJump = true;
		}

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_s))) {
			controlCrouch = true;
		}

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_r))) {
			controlReload = true;
		}

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::SPACE))) {
			controlContinue = true;
		}

		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
			toggleFocus = !toggleFocus;
		}

		glm::vec2 mouse_in_world = game_play_camera_.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

		controlAimX = mouse_in_world.x + halfCursorSide;
		controlAimY = mouse_in_world.y + halfCursorSide;


		//if (code > SDLK_0 && code < SDLK_9) { // Cambio de arma
		//	controlSelectGun = true;
		//	selectedGun = code - 49;
		//}
		//

		//if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::NUM_0))) {
		//		controlSelectGun = true;
		//		selectedGun = code - 49;
		//}

}