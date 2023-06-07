#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
namespace SquareBoxExample {

	class Assistant
	{
	public:
		void init() {};
		void cameraControls(SquareBox::Camera::ParallelCamera& camera_, SquareBox::IMainGame* gamePtr_);
		void dispose() {};

	private:
		/* 
			These were copied from the Editors Assistant and should always be kept in line with them in order to maintain consistence 
		*/

		//camera controls
		int camera_panning_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_ALT);
		int camera_panning_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_ALT);
		int camera_panning_master_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);


		//camera zoom
		int camera_zooming_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT);
		int camera_zooming_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_SHIFT);
		//and the mouse wheel


		//camera motion
		int camera_motion_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL);
		int camera_motion_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_CTRL);
	};
}
