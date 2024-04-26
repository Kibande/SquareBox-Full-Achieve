#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
namespace Platformer {
	class GameLogic
	{

	public:
		void init(SquareBox::IMainGame* game_ptr_, SquareBox::Camera::ParallelCamera& game_play_camera_);

		void update(SquareBox::Camera::ParallelCamera& game_play_camera_);

		void update(float delta_time_, bool fire_, std::vector<SquareBox::GWOM::Layer>& layers_);
		bool controlShoot = false;
		bool controlReload = false;
		bool controlContinue = false;
		bool controlCrouch = false;
		bool controlGetUp = false;
		bool controlJump = false;
		bool controlSelectGun = false;
		bool controlNextGun = false;
		bool controlPreviousGun = false;
		bool toggleFocus = false;

		int selectedGun = 1;
		int controlAimX = 0;
		int controlAimY = 0;
		int controlMoveX = 0;

		static const int halfCursorSide = 10;

		SquareBox::IMainGame* m_game_ptr = nullptr;

	};

}

