#include "BridgeWrapper.h"

namespace SquareBox {
	namespace Shapes {
		void BridgeWrapper::createBridge(glm::vec2& startingPoint_, glm::vec2& endingPoint_, std::vector<SquareBox::GWOM::WorldCluster>& worldClusters_, SquareBox::PhysicsCollisionEngine::PhysicsWorld& physicsWorld_, int& activeWorldClusterIndex_, std::vector<std::pair<int, int >>& vecOfAliveClusterobjects, SquareBox::GWOM::ClusterObject& tempBridgeJointsObject_, float weightLoss)
		{
			SquareBox::GWOM::ClusterObject* currentClusterObject_ptr = nullptr;
			currentClusterObject_ptr = &worldClusters_[activeWorldClusterIndex_].cluster_objects[worldClusters_[activeWorldClusterIndex_].cluster_objects.size() - 1];
			currentClusterObject_ptr->height = tempBridgeJointsObject_.height;
			currentClusterObject_ptr->width = tempBridgeJointsObject_.width;
			currentClusterObject_ptr->radius = tempBridgeJointsObject_.radius;
			currentClusterObject_ptr->is_fixed_rotation = tempBridgeJointsObject_.is_fixed_rotation;
			currentClusterObject_ptr->shape = tempBridgeJointsObject_.shape;
			std::vector<std::pair<int, int>> vectorOfCreatedClusterObjects;
			//using pythongrogous theory

			//To Do
			// we have to implement a drawing mechanisim that takes into account the trail we want to follow
			glm::vec2 pointA = startingPoint_;
			glm::vec2 pointB = endingPoint_;
			float distance = fabs(glm::length(pointA - pointB));

			float slope = ((pointB.y - pointA.y) / (pointB.x - pointA.x));

			float c_intercept = pointA.y - (slope * pointA.x);

			float pacer = 0.0f;
			std::cout << "point a ( " << pointA.x << " , " << pointA.y << " ) " << std::endl;
			std::cout << "point b ( " << pointB.x << " , " << pointB.y << " ) " << std::endl;
			std::cout << " Distance " << distance << std::endl;
			int maxPossibleNumObjects;//how many boxes , circles will make up our bridge
			float totalJointsLength;
			float separationDistance;//how far aparts are our shapes
			//the weightloss are the number of circles that we shall shade off, and it should be greater thn one
			//otherwise if jsut drops the bridge in some cases, especially if we are counting from the 0 x cord of the screen
			//weightloss should neva be 100%

			if (weightLoss < 0.1f || weightLoss>0.9f) {
				weightLoss = 0.1f;
			}

			if (currentClusterObject_ptr->shape == SquareBox::BodyShapeEnum::Circle)
			{
				maxPossibleNumObjects = static_cast<int>(distance / (currentClusterObject_ptr->radius * 2.0f));
				if (currentClusterObject_ptr->radius <= 0) {
					SBX_CORE_INFO("Inorder to draw a circle Radius Must be greater than 0 ");
					return;
				}
				pacer = currentClusterObject_ptr->radius * 2.0f;
				if (distance > 0) {
					maxPossibleNumObjects = static_cast<int>(maxPossibleNumObjects - (maxPossibleNumObjects * weightLoss));//Shade off some circles
					totalJointsLength = distance - (maxPossibleNumObjects * (currentClusterObject_ptr->radius * 2.0f));
					separationDistance = totalJointsLength / (maxPossibleNumObjects * weightLoss);
				}
				else {
					maxPossibleNumObjects = 0;
					totalJointsLength = 0.0f;
					separationDistance = 0.0f;
				}

				std::cout << "Num Circles " << maxPossibleNumObjects << std::endl;
				std::cout << "Separation Distance " << separationDistance << std::endl;
			}
			else if (currentClusterObject_ptr->shape == SquareBox::BodyShapeEnum::Box)
			{
				maxPossibleNumObjects = distance / currentClusterObject_ptr->width;

				pacer = currentClusterObject_ptr->width;
				if (currentClusterObject_ptr->width <= 0 || currentClusterObject_ptr->height <= 0) {
					SBX_CORE_INFO("Inorder to draw a box height and width Must be greater than 0 ");
					return;
				}
				if (distance > 0) {
					maxPossibleNumObjects = maxPossibleNumObjects - (maxPossibleNumObjects * weightLoss);

					totalJointsLength = distance - (maxPossibleNumObjects * (currentClusterObject_ptr->width));
					separationDistance = totalJointsLength / (maxPossibleNumObjects * weightLoss);
				}
				else {
					maxPossibleNumObjects = 0.0f;
					totalJointsLength = 0.0f;
					separationDistance = 0.0f;
				}

				std::cout << "Num Boxes " << maxPossibleNumObjects << std::endl;
				std::cout << "Separation Distance " << separationDistance << std::endl;
			}
			else {
				SBX_CORE_INFO("Shape type Not yet supported by Bridge Wrapper");
			}
			//creating the cluster Objects
			float x_range = std::fabs(pointA.x - pointB.x);
			float y_range = std::fabs(pointA.y - pointB.y);

			float currentX = 0.0f;
			float currentY = 0.0f;

			if (x_range > y_range) {
				currentX = pointA.x;
			}
			else {
				currentY = pointA.y;
			}

			for (unsigned i = 0; i < maxPossibleNumObjects; i++)
			{
				if (i == 0)
				{
					//first
					currentClusterObject_ptr->type = SquareBox::BodyTypeEnum::StaticBody;
					currentClusterObject_ptr->is_fixed_rotation = true;
					currentClusterObject_ptr->position = glm::vec2(pointA.x, pointA.y);
					std::cout << "first -> ( " << pointA.x << " , " << pointA.y << " ) " << std::endl;
				}
				else if (i == maxPossibleNumObjects - 1) {
					//last
					currentClusterObject_ptr->type = SquareBox::BodyTypeEnum::StaticBody;
					currentClusterObject_ptr->is_fixed_rotation = true;
					currentClusterObject_ptr->position = glm::vec2(pointB.x, pointB.y);
					std::cout << "last -> ( " << pointB.x << " , " << pointB.y << " ) " << std::endl;
				}
				else
				{
					//we have to account for the first spot already being handled
					if (x_range > y_range) {
						currentX = currentX + separationDistance;
						currentY = ((slope * currentX) + c_intercept);
					}
					else {
						currentY = currentY + separationDistance;
						if ((currentY - c_intercept) > 0) {
							currentX = ((currentY - c_intercept) / slope);
						}
						else {
							currentX = 0;
						}
					}
					currentClusterObject_ptr->type = SquareBox::BodyTypeEnum::DynamicBody;
					currentClusterObject_ptr->is_fixed_rotation = tempBridgeJointsObject_.is_fixed_rotation;
					currentClusterObject_ptr->position = glm::vec2(currentX, currentY);
					std::cout << "point -> ( " << currentX << " , " << currentY << " ) " << std::endl;
				}

				if (currentClusterObject_ptr->physics_properties == nullptr) {
					currentClusterObject_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
				}
				physicsWorld_.createObject(*currentClusterObject_ptr);
				vecOfAliveClusterobjects.push_back(std::pair<int, int>(currentClusterObject_ptr->cluster_index, currentClusterObject_ptr->index));
				currentClusterObject_ptr->is_alive = true;
				vectorOfCreatedClusterObjects.push_back(std::pair<int, int>(currentClusterObject_ptr->cluster_index, currentClusterObject_ptr->index));
				worldClusters_[activeWorldClusterIndex_].cluster_objects.emplace_back();
				int	last_index = worldClusters_[activeWorldClusterIndex_].cluster_objects.size() - 1;
				//we shall maintain the same cluster al through
				currentClusterObject_ptr = &worldClusters_[activeWorldClusterIndex_].cluster_objects[last_index];
				std::stringstream ss;
				if (last_index < 10) {
					ss << "Cluster Object 00";
					ss << last_index;
				}
				else if (last_index < 100) {
					ss << "Cluster Object 0";
					ss << last_index;
				}
				else {
					ss << "Cluster Object ";
					ss << last_index;
				}
				std::string nameString = ss.str();

				for (unsigned int si = 0; si < nameString.size(); si++)
				{
					currentClusterObject_ptr->name[si] = nameString[si];
				}
				currentClusterObject_ptr->name[nameString.size()] = '\0';

				ss.clear();
				currentClusterObject_ptr->index = last_index;
				currentClusterObject_ptr->cluster_index = worldClusters_[activeWorldClusterIndex_].index;
				//reaffrim all the defaults
				currentClusterObject_ptr->height = tempBridgeJointsObject_.height;
				currentClusterObject_ptr->width = tempBridgeJointsObject_.width;
				currentClusterObject_ptr->radius = tempBridgeJointsObject_.radius;
				currentClusterObject_ptr->is_fixed_rotation = tempBridgeJointsObject_.is_fixed_rotation;
				currentClusterObject_ptr->shape = tempBridgeJointsObject_.shape;
			}

			//now creating the joints
			if (separationDistance > 0) {
				for (unsigned int i = 0; i < vectorOfCreatedClusterObjects.size() - 1; i++)
				{
					SquareBox::GWOM::ClusterObject& bodyA = worldClusters_[vectorOfCreatedClusterObjects[i].first].cluster_objects[vectorOfCreatedClusterObjects[i].second];
					SquareBox::GWOM::ClusterObject& bodyB = worldClusters_[vectorOfCreatedClusterObjects[i + 1].first].cluster_objects[vectorOfCreatedClusterObjects[i + 1].second];

					SquareBox::GWOM::Joint* joint = new SquareBox::GWOM::Joint;
					joint->joint_type = SquareBox::JointTypeEnum::distance_joint;
					joint->body_a_coordinates = std::pair<int, int>(bodyA.cluster_index, bodyA.index);
					joint->body_b_coordinates = std::pair<int, int>(bodyB.cluster_index, bodyB.index);
					joint->local_anchor_a = glm::vec2(0.0f);
					joint->local_anchor_b = glm::vec2(0.0f);
					joint->collide_connected_bodies = true;
					joint->min_length = separationDistance;
					joint->length = separationDistance;
					joint->max_length = separationDistance;
					physicsWorld_.createJoint(bodyA, bodyB, *joint);
				}
			}
		}
	}
};