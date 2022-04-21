#pragma once
#include <glm/glm.hpp>
#include <GWOM/Entities.h>
#include <sstream>
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>

namespace SquareBox {
	namespace Shapes {
		class BridgeWrapper
		{
		public:
			void createBridge(glm::vec2 & startingPoint_, glm::vec2 & endingPoint_, std::vector<SquareBox::GWOM::WorldCluster> &worldClusters_, SquareBox::PhysicsCollisionEngine::PhysicsWorld & physicsWorld_, int & activeWorldClusterIndex_, std::vector<std::pair<int, int >>& vecOfAliveClusterobjects, SquareBox::GWOM::ClusterObject& tempBridgeJointsObject_, float weightLoss);
		};
	};
};
