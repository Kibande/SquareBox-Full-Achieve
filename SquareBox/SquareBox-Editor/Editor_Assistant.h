#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
namespace SquareBoxEditor {
	class Editor_Assistant
	{
	public:
		void currentTileDuplicator(SquareBox::GWOM::Layer & active_layer_ref_, std::vector<std::pair<int,int>> & selected_tiles_vec_, SquareBox::IMainGame * m_game_ptr_, SquareBox::Utilities & m_utilities_ref_);
		std::pair<int, int> currentClusterObjectDuplicator(SquareBox::PhysicsCollisionEngine::PhysicsWorld & m_physicsWorld, std::vector<SquareBox::GWOM::Layer>&  layers_, std::vector<std::pair<int, int>> & m_selectedClusterObjects, SquareBox::GWOM::ClusterObject * m_currentClusterObject_ptr, SquareBox::IShape* m_currentShape_ptr, SquareBox::IMainGame * m_game, SquareBox::Utilities & m_utilities);
		void cameraControls(SquareBox::Camera::ParallelCamera & camera_, SquareBox::IMainGame * gamePtr_);
	private:

		//Controls
		//cluster object selection duplicating combo
		int cluster_object_selection_duplication_input_key_1 = static_cast<int>(SquareBox::KeyBoardEnum::TAB);
		//duplication direction keys
		int cluster_object_selection_north_duplication_input_key = static_cast<int>(SquareBox::KeyBoardEnum::ARROW_UP);
		int cluster_object_selection_south_duplication_input_key = static_cast<int>(SquareBox::KeyBoardEnum::ARROW_DOWN);
		int cluster_object_selection_left_duplication_input_key = static_cast<int>(SquareBox::KeyBoardEnum::ARROW_LEFT);
		int cluster_object_selection_right_duplication_input_key = static_cast<int>(SquareBox::KeyBoardEnum::ARROW_RIGHT);


		//camera controls
		int camera_panning_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_ALT);
		int camera_panning_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_ALT);
		int camera_panning_master_input_key= static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);


		//camera zoom
		int camera_zooming_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_ALT);
		int camera_zooming_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_ALT);
		//and the mouse wheel


		//camera motion
		int camera_motion_1_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL);
		int camera_motion_2_of_or_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_CTRL);
		//and the mouse wheel
	
	};
}