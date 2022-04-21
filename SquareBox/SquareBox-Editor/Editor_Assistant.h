#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
namespace SquareBoxEditor {
	class Editor_Assistant
	{
	public:
		std::pair<int, int> currentClusterObjectDupper(SquareBox::PhysicsCollisionEngine::PhysicsWorld & m_physicsWorld, std::vector<SquareBox::GWOM::WorldCluster>&  m_worldClusters, std::vector<std::pair<int, int>> & m_aliveClusterObjects, std::vector<std::pair<int, int>> & m_selectedClusterObjects, SquareBox::GWOM::ClusterObject * m_currentClusterObject_ptr, SquareBox::IShape* m_currentShape_ptr, SquareBox::IMainGame * m_game, SquareBox::Utilities & m_utilities);
		void cameraControls(SquareBox::Camera::ParallelCamera & camera_, SquareBox::IMainGame * gamePtr_);

	};
}