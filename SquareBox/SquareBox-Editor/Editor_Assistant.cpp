#include "Editor_Assistant.h"
/*
	the code in this class is designed to be able to practically be liftable from here and thrown into the LevelEditor
	in the places where the function is called
*/
std::pair<int, int> SquareBoxEditor::Editor_Assistant::currentClusterObjectDupper(SquareBox::PhysicsCollisionEngine::PhysicsWorld & m_physicsWorld, std::vector<SquareBox::GWOM::WorldCluster>&  m_worldClusters, std::vector<std::pair<int, int>> & m_aliveClusterObjects, std::vector<std::pair<int, int>> & m_selectedClusterObjects, SquareBox::GWOM::ClusterObject * m_currentClusterObject_ptr, SquareBox::IShape* m_currentShape_ptr, SquareBox::IMainGame * m_game, SquareBox::Utilities & m_utilities)
{
	std::pair<int, int> m_currentClusterObject_coordinates = std::pair<int, int>(-1, -1);//since our pointer is going to get fucked!
	//Duplicating ,for needs_width
	if (m_currentClusterObject_ptr != nullptr) {
		if (m_game->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::TAB) && m_game->getInputManager()->isInputIdReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_4)) {
			if (m_currentShape_ptr->needs_height_width) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.x -= orignals_properties_copy.width;

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else if (m_currentShape_ptr->needs_radius) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.x -= (orignals_properties_copy.radius + orignals_properties_copy.radius);

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else {
				SBX_INFO("Shape dupping not yet implemented ");
			}
		}
		else if (m_game->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::TAB) && m_game->getInputManager()->isInputIdReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_6)) {
			if (m_currentShape_ptr->needs_height_width) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.x += orignals_properties_copy.width;

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else if (m_currentShape_ptr->needs_radius) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.x += (orignals_properties_copy.radius + orignals_properties_copy.radius);

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else {
				SBX_INFO("Shape dupping not yet implemented ");
			}
		}
		else 	if (m_game->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::TAB) && m_game->getInputManager()->isInputIdReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_8)) {
			if (m_currentShape_ptr->needs_height_width) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.y += orignals_properties_copy.height;

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else if (m_currentShape_ptr->needs_radius) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.y += (orignals_properties_copy.radius + orignals_properties_copy.radius);

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else {
				SBX_INFO("Shape dupping not yet implemented ");
			}
		}
		else if (m_game->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::TAB) && m_game->getInputManager()->isInputIdReceived((int)SquareBox::KeyBoardEnum::KEYPAD_NUM_2)) {
			if (m_currentShape_ptr->needs_height_width) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.y -= orignals_properties_copy.height;

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else if (m_currentShape_ptr->needs_radius) {
				const int orignals_cluster_index = m_currentClusterObject_ptr->cluster_index;
				const SquareBox::GWOM::ClusterObject orignals_properties_copy = *m_currentClusterObject_ptr;//because we are dealing with dynamic memory
				//create push back a new Cluster Objects
				m_worldClusters[orignals_cluster_index].cluster_objects.push_back(orignals_properties_copy);
				const int dupsIndex = m_worldClusters[orignals_cluster_index].cluster_objects.size() - 1;
				SquareBox::GWOM::ClusterObject & dup = m_worldClusters[orignals_cluster_index].cluster_objects[dupsIndex];
				dup.cluster_index = orignals_cluster_index;
				dup.index = dupsIndex;
				m_utilities.nameClusterObjectByIndex(dup);
				dup.position.y -= (orignals_properties_copy.radius + orignals_properties_copy.radius);

				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &dup, m_worldClusters, dup.cluster_index, &m_physicsWorld, false, false);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_aliveClusterObjects, details.settlementCoordinates);
						//clear out the m_selected_cluster_objects
						m_selectedClusterObjects.clear();
						//add this new one
						m_utilities.addPairToVector(m_selectedClusterObjects, details.settlementCoordinates);

						//make this one the m_currentClusterObject
						m_currentClusterObject_ptr = &m_worldClusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second];
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					}
				}
			}
			else {
				SBX_INFO("Shape dupping not yet implemented ");
			}
		}
		m_currentClusterObject_coordinates.first = m_currentClusterObject_ptr->cluster_index;
		m_currentClusterObject_coordinates.second = m_currentClusterObject_ptr->index;
	}
	return m_currentClusterObject_coordinates;
}

void SquareBoxEditor::Editor_Assistant::cameraControls(SquareBox::Camera::ParallelCamera & camera_, SquareBox::IMainGame * gamePtr_)
{

	float calculated_movement_speed =	1/ camera_.getScale();
	
	//camera panning
	if (gamePtr_->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_ALT) && gamePtr_->getInputManager()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
		
		auto relation_motion= gamePtr_->getInputManager()->getScreenLocations()[1].coordinates;
		camera_.offsetPosition(glm::vec2(-relation_motion.x, relation_motion.y * camera_.getAspectRatio()) * calculated_movement_speed);
	}
	//zoom
	if (gamePtr_->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_ALT)) {
		auto mouse_wheel = gamePtr_->getInputManager()->getPivotMotion();
		float scale_offset = mouse_wheel.y;
		camera_.offsetScale(scale_offset);
	}else if (gamePtr_->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_CTRL)) {
	//check for movement in x axis first
		auto mouse_wheel = gamePtr_->getInputManager()->getPivotMotion();
		float offset_in_x = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(offset_in_x, 0));
	}
	else {
		//all movement is in the y axis
		auto mouse_wheel = gamePtr_->getInputManager()->getPivotMotion();
		float offset_in_y = mouse_wheel.y * calculated_movement_speed;
		camera_.offsetPosition(glm::vec2(0, offset_in_y));
	}
}