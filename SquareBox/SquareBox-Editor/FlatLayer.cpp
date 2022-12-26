#include "FlatLayer.h"

FlatLayer::FlatLayer()
{
}

FlatLayer::~FlatLayer()
{
}

void FlatLayer::updateStates()
{
	//updates all states to show the values of the currently active/selected object so that it doesn't loss its values
	//by taking up the ones of its predessors
	//this function should be called each time we change objects if we want to use the expected new state

	/*
		this Should only be called when m_current_cluster_object changes value other wise it can lead to logic bugs
	*/

	if (m_current_cluster_object_ptr != nullptr) {


		//the shape
		m_selected_shape = static_cast<int>(m_current_cluster_object_ptr->shape);

		//the body Type
		m_selected_body_type = static_cast<int>(m_current_cluster_object_ptr->type);

		if (m_current_cluster_object_ptr->physics_properties == nullptr) {
			m_enable_physics = false;
		}
		else
		{
			m_enable_physics = true;
		}

		if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::SINGLE) {
			//Singles
			m_selected_single_texture_index = m_current_cluster_object_ptr->texture_info.texture_index;
		}
		else {
			//sheets
			m_selected_tile_sheet_texture_index = m_current_cluster_object_ptr->texture_info.texture_index;
		}

		//the family
		m_selected_family = static_cast<int>(m_current_cluster_object_ptr->family);
	}
}

void FlatLayer::checkPhysicsStatus(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	if (m_current_cluster_object_ptr != nullptr) {
		if (m_enable_physics) {
			if (m_current_cluster_object_ptr->physics_properties == nullptr) {
				if (editor_mode_ == EditorModeEnum::PLACE) {
					//the object had not yet been created just give it physics back
					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
				}
				else if (editor_mode_ == EditorModeEnum::SELECT) {
					//the object had already been created !!!
					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
					//recreate it into the physics world
					if (m_current_shape_ptr->is_plotted)
					{
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else
					{
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
				}
			}
		}
		else if (!m_enable_physics) {
			//if this object has a physics object attacted to it delete it
			if (m_current_cluster_object_ptr->physics_properties != nullptr) {
				m_utilities.destroryClusterObjectJoints(layers_, *m_current_cluster_object_ptr, &m_physics_world);
				m_current_cluster_object_ptr->physics_properties->dispose();
				//free up the memory
				delete m_current_cluster_object_ptr->physics_properties;
				m_current_cluster_object_ptr->physics_properties = nullptr;
			}
		}
	}
}

void FlatLayer::onInit()
{
	//animations
	m_animation_creator.init();
	m_animation_creator.loadAnimationScript(m_animation_script);

	//The shapes we can create and edit in the world
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::BoxShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CircleShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PolygonShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::EdgeShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::ChainShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CapsuleShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::TopCapsuleShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PointLightShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CalculatedLightShape());
	shapes_labels_ptr = new const char* [m_vec_shapes_pointer.size()];

	m_vec_animations_pointer.push_back(new SquareBox::AnimationSystem::ForceDependantAnimation);
	m_vec_animations_pointer.push_back(new SquareBox::AnimationSystem::TimeDependantAnimation);
	m_vec_animations_pointer.push_back(new SquareBox::AnimationSystem::PropertiesDependantAnimation);
	animations_labels_ptr = new const char* [m_vec_animations_pointer.size()];

	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::stationary);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::forward);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::backward);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::up);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::down);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::forwardup);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::forwarddown);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::backwardup);
	m_vec_of_animation_motion_states.push_back(SquareBox::AnimationMotionStateEnum::backwarddown);

	animation_motion_states_labels_ptr = new const char* [m_vec_of_animation_motion_states.size()];

	layer_type = SquareBox::LayerTypeEnum::FlatLayer;
	m_physics_world.init();
	m_utilities.init();

	m_temp_limbo_animation.animation_specifications.emplace_back(); 
/* 
																		we need to have atleast one animation specification
																		because we are using it in our adding motion states logic
																	*/
}

void FlatLayer::onLayerInit(SquareBox::GWOM::Layer& layer_)
{
	layer_.alive_cluster_objects.clear();
	layer_.alive_cluster_objects.reserve(60);//some cache friendlness

	//create the layers first world cluster and cluster object
	//set up the first world cluster in this layer
	layer_.world_clusters.emplace_back();
	auto& world_cluster = layer_.world_clusters.back();
	world_cluster.index = 0;
	m_utilities.nameWorldClusterByIndex(world_cluster);

	//set up the first cluster object in the world cluster
	world_cluster.cluster_objects.emplace_back();
	auto& cluster_object = world_cluster.cluster_objects.back();
	cluster_object.cluster_index = world_cluster.index;
	cluster_object.index = 0;
	cluster_object.layer_index = layer_.index; //  give the cluster object its layer index
	m_utilities.nameClusterObjectByIndex(cluster_object);
	layer_.layer_type = SquareBox::LayerTypeEnum::FlatLayer;

}

int FlatLayer::onLayerInitGui(SquareBox::GWOM::Layer& layer_)
{

	ImGui::OpenPopup("Change Layer Type");
	// Always center this window when appearing
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	int choice = -1;
	if (ImGui::BeginPopupModal("Change Layer Type", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("All contents of this layer will be lost");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			choice = 1;
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			choice = 0;
		}
		ImGui::EndPopup();
	}
	return choice;
}

void FlatLayer::onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{

}

void FlatLayer::onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];
	for (unsigned int j = 0; j < active_layer.world_clusters.size(); j++)
	{
		auto& focus_world_cluster = active_layer.world_clusters[j];

		for (unsigned int k = 0; k < focus_world_cluster.cluster_objects.size(); k++)
		{
			/*
				As we are recreating a the cluster objects  , i am worried of invalidating the alive_cluster_objects
				vector, but it being a list of no dups is what is giving me confidence
			*/
			bool is_last_cluster_object_in_last_world_cluster = (j == active_layer.world_clusters.size() - 1) && (k == focus_world_cluster.cluster_objects.size() - 1) ? true : false;
			m_current_cluster_object_ptr = &focus_world_cluster.cluster_objects[k];

			/* this is meant to be a fix incase we are not upating the texture info anywhere else
			 thou i thinking the whole parent texture grid does something
			*/
			//if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::SINGLE) {
			//	if (active_layer.single_textures.size()> m_current_cluster_object_ptr->texture_info.texture_index)
			//	{
			// 		m_current_cluster_object_ptr->texture_info.texture_id = active_layer.single_textures[m_current_cluster_object_ptr->texture_info.texture_index].texture.id;
			//	}
			//}
			//else {
			//	//tile sheet
			//	if (active_layer.tiled_textures.size() > m_current_cluster_object_ptr->texture_info.texture_index)
			//	{
			//		m_current_cluster_object_ptr->texture_info.texture_id = active_layer.tiled_textures[m_current_cluster_object_ptr->texture_info.texture_index].texture.id;
			//	}
			//}
			m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
			SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(m_current_shape_ptr->is_plotted, false, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, is_last_cluster_object_in_last_world_cluster, false);
			if (is_last_cluster_object_in_last_world_cluster) {
				if (details.shellCoordinates.first > -1 && details.shellCoordinates.second > -1) {
					m_current_cluster_object_ptr = &active_layer.world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
				}
				else {
					/*
					  because we are not yet cleaning up the layers indicies . We are currenlty also saving the shell
					  and once it gets loaded, it fails to be created and in return we get invalid settlement and shell coordinates
					  so when this happens for now i am going to reference that shell as out current_cluster object
					  The best approach would be to properly implement the layes indicies clean up so that only cluster objects with
					  valid body dimensions are saved
					*/
					m_current_cluster_object_ptr = &active_layer.world_clusters.back().cluster_objects.back();
					if (m_current_cluster_object_ptr->is_alive) {
						// we have the wrong guy which is an impossibility
						__debugbreak();
					}
				}
			}
		}
	}
	SBX_INFO("Creating Joints.");
	m_utilities.createLayerJointsOnLoadFromStorage(layers_[active_layer_index_], &m_physics_world);
	SBX_INFO("Finished Creating Joints.");
	SBX_INFO("Finised Level Loading.");
	updateStates();
}

void FlatLayer::onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_)
{
	for (unsigned int i = 0; i < active_layer_.alive_cluster_objects.size(); i++)
	{
		auto& cluster_object_coordinates = active_layer_.alive_cluster_objects[i];
		SquareBox::GWOM::ClusterObject& focus_cluser_object = active_layer_.world_clusters[cluster_object_coordinates.first].cluster_objects[cluster_object_coordinates.second];
		m_utilities.updateClusterObjectLayerIndex(focus_cluser_object, new_layer_index_);
	}
	if (m_current_cluster_object_ptr != nullptr) {
		m_current_cluster_object_ptr->layer_index = new_layer_index_;
	}
	active_layer_.index = new_layer_index_;
}

void FlatLayer::onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_)
{

	m_mannually_choosen_world_cluster_index = 0;
	m_auto_assign_world_cluster = true;
	m_selected_cluster_objects.clear();
	m_respect_ancestor = false;
	//if this errors then this layer did not have a good intial setup where 
	// a dummy worldcluster and cluster object were never created
	if (editor_mode_ == EditorModeEnum::PLACE) {

		m_current_cluster_object_ptr = &layer_.world_clusters.back().cluster_objects.back();

		/*

		update the cluster_objects_layer index just incase a layer has been deleted and the whole layer heirecy has changed

		*/
		if (m_current_cluster_object_ptr->is_alive) {
			__debugbreak();
			/*

			   the shell is never ment to be alive while it is still a shell

			*/
		}
	}
	else if (editor_mode_ == EditorModeEnum::SELECT) {
		m_current_cluster_object_ptr = nullptr;
	}
	updateStates();
}

void FlatLayer::onOutOfFocus(SquareBox::GWOM::Layer& layer_)
{
	m_current_cluster_object_ptr = nullptr;
}

void FlatLayer::onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];
	const std::vector<std::pair<int, int>>& active_layer_alive_cluster_objects = active_layer.alive_cluster_objects;

	glm::vec2& mouse_in_world = active_layer.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

	if (editor_mode_ == EditorModeEnum::SELECT) {
		bool hoveringOverCluster = false;
		bool hoveringOverClusterObject = false;
		for (unsigned int i = 0; i < active_layer_alive_cluster_objects.size(); i++)
		{
			SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[active_layer_alive_cluster_objects[i].first].cluster_objects[active_layer_alive_cluster_objects[i].second];
			m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
			// is being hovered and doesn't belong to a camera which is closed
			if (m_current_shape_ptr->containtsPoint(ccobj, mouse_in_world)) {
				hoveringOverCluster = true;
				hoveringOverClusterObject = true;
				m_current_hovered_world_cluster_index = active_layer_alive_cluster_objects[i].first;
				m_current_hovered_cluster_object_index = active_layer_alive_cluster_objects[i].second;
			}
		}

		//selection implementation
		if (m_selection_mode_index == SelectModeEnum::WORLDCLUSTER) {
			//if we are hovering over a cluster object and we Left click let that objects' whole cluster become selected in our world scene
			if (hoveringOverClusterObject && game_ptr_->getInputDevice()->isInputIdReceived(world_cluster_selection_input_key)) {
				m_selected_cluster_objects.clear();
				//loop through our alive Objects and select this HoveredWorld Cluster
				for (size_t i = 0; i < active_layer_alive_cluster_objects.size(); i++)
				{
					if (active_layer_alive_cluster_objects[i].first == m_current_hovered_world_cluster_index) {
						m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(active_layer_alive_cluster_objects[i].first, active_layer_alive_cluster_objects[i].second));
					}
				}
				/*
					keep track of where the mouse was in the world cluster when we selected this world
					cluster , we shall use this while dragging the world cluster later
				*/
				clustermousepos = active_layer.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);
			}
		}
		else if (m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) {
			//Cluster object Selection
			//this is the only place where we actually set the active worldClusters and Objects
			//if we are hovering over a cluster object and we Left click let that object become the current active object in our world scene
			if (hoveringOverClusterObject && game_ptr_->getInputDevice()->isInputIdReceived(cluster_object_selection_input_key)) {
				m_current_cluster_object_ptr = &active_layer.world_clusters[m_current_hovered_world_cluster_index].cluster_objects[m_current_hovered_cluster_object_index];
				updateStates();//this updates the imGui variables
				m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
				m_selected_cluster_objects.clear();
				m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index));

				//check if we just clicked on one of chains or edge ends , if so record this progress
				if (m_current_shape_ptr->is_selected_through_ends) {
					if (m_current_cluster_object_ptr->is_first_hovered) {
						m_current_cluster_object_ptr->is_first_hovered = false;
						m_current_cluster_object_ptr->is_last_hovered = false;

						m_current_cluster_object_ptr->is_first_selected = true;
						m_current_cluster_object_ptr->is_last_selected = false;
					}
					else if (m_current_cluster_object_ptr->is_last_hovered) {
						m_current_cluster_object_ptr->is_first_hovered = false;
						m_current_cluster_object_ptr->is_last_hovered = false;

						m_current_cluster_object_ptr->is_first_selected = false;
						m_current_cluster_object_ptr->is_last_selected = true;
					}
				}
			}

			/* Duplicate  the current Cluster Object if its being requested by the user */
			std::pair<int, int> m_new_cluster_object_coordinates = m_editor_assitant.currentClusterObjectDuplicator(m_physics_world, layers_, m_selected_cluster_objects, m_current_cluster_object_ptr, m_current_shape_ptr, game_ptr_, m_utilities);
			if (m_new_cluster_object_coordinates.first != -1 && m_new_cluster_object_coordinates.second != -1) { //Why would this be (-1,-1) in the first case !!!! i need to better comment this 
				m_current_cluster_object_ptr = &active_layer.world_clusters[m_new_cluster_object_coordinates.first].cluster_objects[m_new_cluster_object_coordinates.second];
			}
		}
		else if (m_selection_mode_index == SelectModeEnum::FREESELECT) {
			//Free selection
			if (hoveringOverClusterObject && game_ptr_->getInputDevice()->isInputIdReceived(free_selection_input_key)) {
				m_utilities.addPairToVector(m_selected_cluster_objects, std::pair<int, int>(m_current_hovered_world_cluster_index, m_current_hovered_cluster_object_index));
				/*
					keep track of where the mouse was in the world cluster when we selected this
					cluster object , we shall use this while dragging the world cluster later
				*/
				clustermousepos = active_layer.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);
			}

			//drag select
			if (game_ptr_->getInputDevice()->isInputIdBeingReceived(free_selection_drag_input_key_1) && game_ptr_->getInputDevice()->isInputIdBeingReceived(free_selection_drag_input_key_2)) {
				//if we are currently not drag selection
				if (!m_is_drag_selecting) {
					m_drag_select_origin = mouse_in_world;
					m_is_drag_selecting = true;
				}
				else {
					glm::vec4 drag_select_destRect(m_drag_select_origin, (mouse_in_world - m_drag_select_origin));
					//add the objects that are currently in our dest to the selected object
					for (size_t i = 0; i < active_layer_alive_cluster_objects.size(); i++)
					{
						SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[active_layer_alive_cluster_objects[i].first].cluster_objects[active_layer_alive_cluster_objects[i].second];
						if (!ccobj.is_hidden && m_utilities.isInBox(ccobj.position, drag_select_destRect)) {
							m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(active_layer_alive_cluster_objects[i].first, active_layer_alive_cluster_objects[i].second));
						}
					}
				}
				/*
					keep track of where the mouse was in the world cluster when we selected this
					cluster object , we shall use this while dragging the world cluster later
				*/
				clustermousepos = active_layer.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);
			}
			else {
				m_is_drag_selecting = false;
			}
		}
		else if (m_selection_mode_index == SelectModeEnum::JOINTSSELECT) {
			//Joint Creation
			//only two objects can be selected at once
			//and only one joint can exit between two bodies
			if (hoveringOverClusterObject && game_ptr_->getInputDevice()->isInputIdReceived(joint_selection_input_key)) {
				//clicked on object
				SquareBox::GWOM::ClusterObject& clicked_on_object = active_layer.world_clusters[m_current_hovered_world_cluster_index].cluster_objects[m_current_hovered_cluster_object_index];
				if (clicked_on_object.physics_properties != nullptr) {
					//only physics Objects can be part of joints
					if (m_selected_cluster_objects.size() == 2) {
						//remove the top most cluster Object Coordinates so that we maintain only two selected cluster Objects
						m_selected_cluster_objects.erase(m_selected_cluster_objects.begin());
					}
					if (m_selected_cluster_objects.size() > 2) {
						SBX_ERROR("m_selectedClusterObjects size is greater than 2, for SelectionMode joint ");
					}
					m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_current_hovered_world_cluster_index, m_current_hovered_cluster_object_index), false);
				}
			}

			//plotting joint points
			if (m_current_joint_ptr != nullptr) {
				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
					if (game_ptr_->getInputDevice()->isInputIdReceived(pulley_plotting_input_key)) {
						glm::vec2 mouse_in_world = layers_[active_layer_index_].camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);
						if (m_current_joint_ptr->vec_of_points.size() < 2) {
							m_current_joint_ptr->vec_of_points.push_back(mouse_in_world);
						}
					}
				}
			}

		}
		if (m_selection_mode_index != SelectModeEnum::FREESELECT) {
			m_is_drag_selecting = false;
		}

		//dragging and copying implementations
		if (m_selection_mode_index == SelectModeEnum::WORLDCLUSTER || m_selection_mode_index == SelectModeEnum::FREESELECT && !(game_ptr_->getInputDevice()->isInputIdBeingReceived(world_cluster_and_free_selection_dragging_activation_input_key))) {
			//dont drag and drag select at the same time
			if (game_ptr_->getInputDevice()->isInputIdBeingReceived(world_cluster_and_free_selection_dragging_input_key))
			{
				float diplacementinX = mouse_in_world.x - clustermousepos.x;
				float diplacementinY = mouse_in_world.y - clustermousepos.y;
				for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
					m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);

					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						for (unsigned int j = 0; j < ccobj.vertices.size(); j++)
						{
							ccobj.vertices[j].x += diplacementinX;
							ccobj.vertices[j].y += diplacementinY;
						}
						//need to do the logic for displacing edges,chains and ploygons and bridges since
						//we want to displace the vertices based on the mouses motion
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else
					{
						ccobj.position.x += diplacementinX;
						ccobj.position.y += diplacementinY;
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
				}
				clustermousepos = mouse_in_world;
				m_current_shape_ptr = nullptr;
			}

			//copying with Ctrl-C implementation ,
			//Ctrl-V is not yet implemented thou, nno need for that
			if (
				m_selected_cluster_objects.size() > 0
				&&
				(
					game_ptr_->getInputDevice()->isInputIdBeingReceived(left_ctrl_input_key) ||
					game_ptr_->getInputDevice()->isInputIdBeingReceived(right_ctrl_input_key)
					)
				&&
				game_ptr_->getInputDevice()->isInputIdReceived(c_input_key)
				)
			{
				m_is_all_work_saved = false;
				//make a copy of all the seleceted objects
				std::vector<std::pair<int, int>> copy_of_selected_objects = m_selected_cluster_objects;
				//clear out the original selected obejcts
				m_selected_cluster_objects.clear();
				//create copies of the previously selected ojects but in new locations in our worldClusters vector

				SquareBox::GWOM::ClusterObject* new_ccobj = nullptr;
				std::pair<int, int> got_shell_coordinates = std::pair<int, int>(-1, -1);

				for (unsigned int i = 0; i < copy_of_selected_objects.size(); i++)
				{
					if (i == 0) {
						//for the first one we will have to create the new objects template manually
						//create the first object mannually 
						const int world_cluster_index = copy_of_selected_objects[0].first;

						//a copy not reference
						SquareBox::GWOM::ClusterObject copy_of_original_ccobj = active_layer.world_clusters[copy_of_selected_objects[0].first].cluster_objects[copy_of_selected_objects[0].second];

						copy_of_original_ccobj.is_alive = false;

						//TODO :
						//joints are not copied , i will implement this later
						copy_of_original_ccobj.joints.clear();
						//change its index 
						copy_of_original_ccobj.index = active_layer.world_clusters[world_cluster_index].cluster_objects.size();
						// rename it
						m_utilities.nameClusterObjectByIndex(copy_of_original_ccobj);

						//add the copy to the worldClusters
						active_layer.world_clusters[world_cluster_index].cluster_objects.push_back(copy_of_original_ccobj);

						// get a referenece to it 
						new_ccobj = &active_layer.world_clusters[world_cluster_index].cluster_objects.back();

					}
					else {
						//depend on the shell we got 
						if (got_shell_coordinates.first >= 0 && got_shell_coordinates.second >= 0) {
							//copy properties
							active_layer.world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second] = active_layer.world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second];
							//point to our object
							new_ccobj = &active_layer.world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second];
							//restore the cluster_index && index
							new_ccobj->cluster_index = got_shell_coordinates.first;
							new_ccobj->index = got_shell_coordinates.second;
							//restore the default name
							m_utilities.nameClusterObjectByIndex(*new_ccobj);
							new_ccobj->is_alive = false;
							//joints are not copied , i will implement this later
							new_ccobj->joints.clear();
						}
						else {

							SBX_ERROR("Invalid shell coordinates");
							__debugbreak();
						}
					}

					//The actual creation into the world starts here
					m_utilities.setCurrentShapePointer(new_ccobj->shape, &m_current_shape_ptr);
					//the world placement

					//the core should not create a shell after we create the last  cluster object
					bool create_shell = i < (copy_of_selected_objects.size() - 1);
					int desired_shell_world_cluster_index = -1;//only remain true if we are not creating a shell 
					if (create_shell) {
						desired_shell_world_cluster_index = copy_of_selected_objects[i + 1].first;
						/*
						* world cluster index
						* of next cluster objects in the copy_of_selected_objects
						*/
					}
					SquareBox::Utilities::creationDetails  details;
					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), new_ccobj, layers_, &m_physics_world, create_shell, false);
					}
					else {
						details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), new_ccobj, layers_, &m_physics_world, create_shell, false);
					}
					if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
						m_utilities.addPairToVector(active_layer.alive_cluster_objects, details.settlementCoordinates);
						m_utilities.addPairToVector(m_selected_cluster_objects, details.settlementCoordinates);
					}
					else {
						SBX_ERROR("We failed to get back settlement coordinates when copying a world cluster ");
					}
					if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
						got_shell_coordinates = details.shellCoordinates;
					}
					else {
						got_shell_coordinates = std::pair<int, int>(-1, -1);
					}
				}
			}
		}
		else if (m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT && m_current_cluster_object_ptr != nullptr) {
			//ClusterObject Mode
			if (m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
				if (game_ptr_->getInputDevice()->isInputIdBeingReceived(cluster_object_drag_selection_input_key))
				{
					m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
					//we are trusting our program to always make the selected object the current object ,which it does perfectly
					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						//need to do the logic for displacing edges,chains and ploygons and bridges since
						//we want to displace the vertices basinf on the mouses motion
						m_utilities.createClusterObjectIntoWorld(true, true, true, mouse_in_world, m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else
					{
						m_utilities.createClusterObjectIntoWorld(false, true, true, mouse_in_world, m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
				}
			}
		}
		else if (m_selection_mode_index == SelectModeEnum::JOINTSSELECT) {
			/*
				Joints dragging is handled when the individual cluster Objects are dragged
			*/
		}
	}
	else if (editor_mode_ == EditorModeEnum::PLACE) {
		if (game_ptr_->getInputDevice()->isInputIdReceived(non_tiled_layer_placement_input_ley)) {
			//place plotting points for the ploted shapes , and draw the other objects
			SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, true, mouse_in_world, m_current_cluster_object_ptr, layers_, &m_physics_world, true, m_respect_ancestor);
			//add to m_alive_cluster_objects Objects if its alive
			if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
				if (active_layer.world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
					m_utilities.addPairToVector(active_layer.alive_cluster_objects, details.settlementCoordinates);
					m_is_all_work_saved = false;
				}
				else {
					SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
					__debugbreak();
				}
			}
			//get a pointer to the shell
			if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
				m_current_cluster_object_ptr = &active_layer.world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
				checkPhysicsStatus(layers_, active_layer_index_, editor_mode_);
				updateStates();
			}

		}
	}
}

void FlatLayer::onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
	checkPhysicsStatus(layers_, active_layer_index_, editor_mode_);//to keep track when we toggle an objects Physics abilities

	/* setting the record straight for current cluster object pointer*/
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];
	if (editor_mode_ == EditorModeEnum::PLACE) {
		if (m_auto_assign_world_cluster) {
			m_current_cluster_object_ptr = &active_layer.world_clusters.back().cluster_objects.back();
		}
		else {
			m_current_cluster_object_ptr = &active_layer.world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.back();
		}
		updateStates();
	}


	SquareBox::IShape* orignalShapePointer = m_current_shape_ptr;

	for each (const auto & ref_layer in layers_) {
		auto& layer = layers_[ref_layer.index];
		/*
		 doing this for all none tiled layers because updating the physics engine does not only update the active layers
		 objects
		*/
		if (m_run_physics) {
			for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject& ccobj = layer.world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
				m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
				m_current_shape_ptr->keepTrackOfPosition(ccobj);//Must be run before our automation.update

				//recreate objects that requested recreation, mainly through the SquareBox::AI
				if (ccobj.requesting_recreation) {
					/* Requested recreation does on take into account the current mouse position */
					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else {
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					ccobj.requesting_recreation = false;
				}
			}
		}
	}

	m_current_shape_ptr = orignalShapePointer;

	//placing draw shapes  into the world
	if (editor_mode_ == EditorModeEnum::PLACE) {
		if (m_draw_plotted_shape) {
			SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, true, m_respect_ancestor);
			//add to m_alive_cluster_objects Objects if its alive
			if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
				if (active_layer.world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
					m_utilities.addPairToVector(active_layer.alive_cluster_objects, details.settlementCoordinates);
					m_is_all_work_saved = false;
				}
				else {
					SBX_CRITICAL("Tried Adding a Dead Object to m_aliveClusterObjects");
				}
			}
			//get a pointer to the shell
			if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
				m_current_cluster_object_ptr = &active_layer.world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
				checkPhysicsStatus(layers_, active_layer_index_, editor_mode_);
				updateStates();
			}
			else {
				SBX_CRITICAL("Missing shell!!");
			}

			m_draw_plotted_shape = false;
		}
	}


	bool deleted_a_cluster_object = false;
	// deleting objects
	for (unsigned int i = 0; i < m_cluster_objects_to_delete.size(); i++)
	{
		auto& target_layer = layers_[m_cluster_objects_to_delete[i].first];
		if (target_layer.index != active_layer_index_) {
			SBX_CRITICAL("Unexpected Occurance {} {} ", __FILE__, __LINE__);
			/*
			  As per current editor setting , we only delete cluster objects in the active layer
			  hence the deletion layer is expected to be the same as the active layer
			*/
		}

		auto& target_cluster_object_coordinates = m_cluster_objects_to_delete[i].second;
		auto& target_cluster_object = target_layer.world_clusters[target_cluster_object_coordinates.first].cluster_objects[target_cluster_object_coordinates.second];

		m_utilities.deleteClusterObjectFromWorld(layers_, target_cluster_object, &m_physics_world);
		deleted_a_cluster_object = true;
	}
	m_cluster_objects_to_delete.clear();
	if (deleted_a_cluster_object) {
		m_selected_cluster_objects.clear();
	}


	//run update loop for our world objects
	if (m_run_physics) {
		m_physics_world.update();//update our physics
	}

	for each (auto & layer in layers_) {
		m_animation_creator.update(deltaTime_, layers_[layer.index].world_clusters, layer.alive_cluster_objects, game_ptr_->getFps(), game_ptr_->getGameLoopElapsedSeconds(), game_ptr_->getInputDevice(), m_play_animations);
	}

	//usefull key board short cuts
	if (game_ptr_->getInputDevice()->isInputIdBeingReceived(clear_selection_input_key_1) && game_ptr_->getInputDevice()->isInputIdBeingReceived(clear_selection_input_key_2)) {
		//Clear selected
		if (editor_mode_ == EditorModeEnum::SELECT && m_selection_mode_index != SelectModeEnum::CLUSTEROBJECT) {
			m_selected_cluster_objects.clear();
			m_current_joint_ptr = nullptr;//Safety precaution in joint select mode
			//would be nice to also be able to do this for cluster object mode.
		}
	}

	if (game_ptr_->getInputDevice()->isInputIdBeingReceived(delete_selection_input_key)) {
		//Delete selected
		if (editor_mode_ == EditorModeEnum::SELECT) {
			for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
			{

				auto& target_cluster_object = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				std::pair<int, int> location_pair = std::pair<int, int>(target_cluster_object.cluster_index, target_cluster_object.index);
				std::pair<int, std::pair<int, int>> master_pair = std::pair<int, std::pair<int, int>>(target_cluster_object.layer_index, location_pair);
				m_cluster_objects_to_delete.push_back(master_pair);
			}
			m_selected_cluster_objects.clear();
			m_current_joint_ptr = nullptr;//Safety precaution in joint select mode
		}
	}

	if (m_physics_world.isLocked()) {
		m_show_world_locked_dialog = true;
	}
	else {
		m_show_world_locked_dialog = false;
	}
}

void FlatLayer::onContentDraw(SquareBox::GWOM::Layer& active_layer, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
	std::vector<std::pair<int, int>> vector_of_alive_cluster_objects = active_layer.alive_cluster_objects;

	for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& cwobj = active_layer.world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
		//lights are drawn using the light render later
		if (m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::PointLightShape && m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::CalculatedLightShape) {
			/*
				we should only be drawing the clusterObjects that are in the cameras view
				where we can, these will safe resources
			*/

			if (m_current_shape_ptr->is_plotted && cwobj.shape == SquareBox::BodyShapeEnum::Edge) {
				SBX_ERROR("Drawing Edges has not yet beeen sorted out {} {}", __FILE__, __LINE__);
				/*if (m_always_draw_edges) {
					if (cwobj.vertices.size() > 1) {
						debug_renderer_.begin();
						m_current_shape_ptr->debugDraw(cwobj, debug_renderer_, SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.color.x), (255 * cwobj.color.y), (255 * cwobj.color.z), (255 * cwobj.color.w) * (0.01 * layer.opacity)));
						debug_renderer_.end();
						m_debug_program.use();
						uploadCameraInfo(&m_debug_program, &layer.camera, "P");
						debug_renderer_.render();
						m_debug_program.unuse();
					}
				}*/
			}
			else if (m_current_shape_ptr->needs_height_width) {
				if (active_layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.width, cwobj.height))) {
					m_current_shape_ptr->draw(cwobj, sprite_batch_, active_layer.opacity);
				}
			}
			else if (m_current_shape_ptr->needs_radius) {
				if (active_layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
					m_current_shape_ptr->draw(cwobj, sprite_batch_, active_layer.opacity);
				}
			}
			else
			{
				SBX_ERROR("Unidentified shape found for {} ", cwobj.name);
			}
		}
	}
}

void FlatLayer::onDebugDraw(SquareBox::GWOM::Layer& active_layer, SquareBox::RenderEngine::DebugRenderer& debug_renderer_)
{
	/*

		the flatlayer has me in a tight spot becasue,
		there objects like the pulleys and edges that should ideally be drawn while drawing the layer contents , but they require the debug renderer not the sprite batch
		currently they are only being drawn in debug mode . which is not right

	*/
	//PULLEY JOINTS
	std::vector<std::pair<int, int>> vector_of_alive_cluster_objects = active_layer.alive_cluster_objects;
	for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& cwobj = active_layer.world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];
		if (cwobj.joints.size() > 0) {
			for (unsigned int j = 0; j < cwobj.joints.size(); j++)
			{
				const SquareBox::GWOM::Joint& focus_joint = cwobj.joints[j];
				if (focus_joint.joint_type == SquareBox::JointTypeEnum::pulley_joint) {
					//draw the pulley ropes

					SquareBox::GWOM::ClusterObject& jointObjectA = active_layer.world_clusters[focus_joint.body_a_coordinates.first].cluster_objects[focus_joint.body_a_coordinates.second];
					SquareBox::GWOM::ClusterObject& jointObjectB = active_layer.world_clusters[focus_joint.body_b_coordinates.first].cluster_objects[focus_joint.body_b_coordinates.second];

					//check if we are dealing with physics bodies , which should always be the case for joints 
					if (jointObjectA.physics_properties != nullptr && jointObjectB.physics_properties != nullptr && focus_joint.vec_of_points.size() == 2) {
						m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
						debug_renderer_.drawLine(focus_joint.vec_of_points[0], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectA, focus_joint.local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * focus_joint.color.x), (255 * focus_joint.color.y), (255 * focus_joint.color.z), (255 * focus_joint.color.w))));
						debug_renderer_.drawLine(focus_joint.vec_of_points[1], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectB, focus_joint.local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * focus_joint.color.x), (255 * focus_joint.color.y), (255 * focus_joint.color.z), (255 * focus_joint.color.w))));
					}
					else {
						SBX_ERROR("Error while trying to draw pulley Joint ropes");
					}
				}
			}
		}
	}

	// +X axis
	glm::vec2 debug_center = active_layer.camera.getPosition();
	float camera_width = active_layer.camera.getCameraDimensions().x;
	float camera_height = active_layer.camera.getCameraDimensions().y;

	//+X
	debug_renderer_.drawLine(debug_center, glm::vec2(debug_center.x + (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 200));
	// -X axis
	debug_renderer_.drawLine(debug_center, glm::vec2(debug_center.x - (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 100));

	// +Y axis
	debug_renderer_.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y + (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 200));
	// -Y axis
	debug_renderer_.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y - (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 100));

	for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& cwobj = active_layer.world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

		if (cwobj.physics_properties == nullptr || !cwobj.physics_properties->isIntialised()) {
			m_border_color = m_not_physics_body_color;
		}
		else if (cwobj.physics_properties->isDynamic()) {
			m_border_color = m_debug_dynamic_body_color;
		}
		else if (cwobj.physics_properties->isStatic()) {
			m_border_color = m_debug_static_body_color;
		}
		else if (cwobj.physics_properties->isKinematic()) {
			m_border_color = m_debug_kinetic_body_color;
		}
		else {
			m_border_color = m_not_physics_body_color;
		}



		//show debug edges
		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
		m_current_shape_ptr->debugDraw(cwobj, debug_renderer_, m_border_color);
		//show the first and last vertices
		if (m_current_shape_ptr->is_plotted) {
			m_utilities.showPlottedVertices(true, debug_renderer_, cwobj, 1.0f / active_layer.camera.getScale());
		}

	}


}

void FlatLayer::onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_, EditorModeEnum editor_mode_)
{
	if (editor_mode_ == EditorModeEnum::PLACE && m_current_cluster_object_ptr != nullptr) {
		/*
			m_current_cluster_object_ptr my be null if we are just coming from select mode
		*/
		glm::vec2 mouse_in_world = layer_.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

		m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
		m_current_shape_ptr->traceDraw(*m_current_cluster_object_ptr, debug_renderer_, mouse_in_world, m_place_mode_sketch_color, layer_.camera.getScale());
	}
	else if (editor_mode_ == EditorModeEnum::SELECT) {
		//drawing the selected objects debug lines
		//are we among the selected Objects
		for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
		{
			SquareBox::GWOM::ClusterObject& cwobj = layer_.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
			m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
			m_current_shape_ptr->debugDraw(cwobj, debug_renderer_, m_selected_body_color);
		}
		glm::vec2 mouse_in_world = layer_.camera.convertScreenToWorld(game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

		//are we drag selecting
		if (m_is_drag_selecting) {
			//construct the destRect 
			glm::vec4 drag_select_destRect(m_drag_select_origin, (mouse_in_world - m_drag_select_origin));
			debug_renderer_.drawBox(drag_select_destRect, m_drag_selection_box_color, 0.0f);
		}
		//drawing the scaling lines
		if ((m_selected_cluster_objects.size() == 1) && m_show_scaling_in_debug_mode && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) {//dont show in freelance mode
			SquareBox::GWOM::ClusterObject& cwobj = layer_.world_clusters[m_selected_cluster_objects[0].first].cluster_objects[m_selected_cluster_objects[0].second];
			m_current_shape_ptr->scalingDraw(cwobj, debug_renderer_, mouse_in_world, game_ptr_->getInputDevice());
		}
		//for Joint Creation selection Mode
		//ploting the plotted points
		for (unsigned int i = 0; i < m_vector_of_joint_ploting_points.size(); i++)
		{
			debug_renderer_.drawCircle(m_vector_of_joint_ploting_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / layer_.camera.getScale());
		}

		if (m_current_joint_ptr != nullptr) {
			SquareBox::GWOM::ClusterObject& jointBodyA = layer_.world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second];
			SquareBox::GWOM::ClusterObject& jointBodyB = layer_.world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second];

			m_utilities.setCurrentShapePointer(jointBodyA.shape, &m_current_shape_ptr);
			debug_renderer_.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyA, m_current_joint_ptr->local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / layer_.camera.getScale());

			m_utilities.setCurrentShapePointer(jointBodyB.shape, &m_current_shape_ptr);
			debug_renderer_.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyB, m_current_joint_ptr->local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / layer_.camera.getScale());

			if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
				for (unsigned int i = 0; i < m_current_joint_ptr->vec_of_points.size(); i++)
				{
					debug_renderer_.drawCircle(m_current_joint_ptr->vec_of_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / layer_.camera.getScale());
				}
			}
		}
	}
}

void FlatLayer::onLightDraw(SquareBox::GWOM::Layer& active_layer, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
	std::vector<std::pair<int, int>> vector_of_alive_cluster_objects = active_layer.alive_cluster_objects;
	for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& cwobj = active_layer.world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);

		if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::PointLightShape) {
			if (m_current_shape_ptr->needs_radius) {
				if (active_layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
					m_current_shape_ptr->draw(cwobj, sprite_batch_);
				}
			}
		}
		else if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::CalculatedLightShape) {
			m_current_shape_ptr->draw(cwobj, sprite_batch_);
		}
	}
}

void FlatLayer::onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];

	if (m_show_world_locked_dialog) {
		ImGui::OpenPopup("WorldLocked");
	}

	if (ImGui::BeginPopupModal("WorldLocked", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(" The Physics Engine started at a bad memory Address .\nPlease reload the Editor");
		ImGui::Separator();

		static bool dont_ask_me_next_time_about_world_locked = false;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time_about_world_locked);
		ImGui::PopStyleVar();

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			//Close  LevelEditor
			//m_allow_exit = true;
			//onExit();
			/* this would require us communicating with the editor*/
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (editor_mode_ == EditorModeEnum::PLACE) {
			if (m_auto_assign_world_cluster) {
				if (ImGui::Button("New Cluster")) {
					m_auto_assign_world_cluster = true;//we are lossing control
					active_layer.world_clusters.emplace_back();
					auto new_world_cluster = active_layer.world_clusters.back();
					new_world_cluster.index = active_layer.world_clusters.size() - 1;
					m_utilities.nameWorldClusterByIndex(new_world_cluster);
					m_current_cluster_object_ptr = &new_world_cluster.cluster_objects.back();
					m_current_cluster_object_ptr->cluster_index = new_world_cluster.index;
					m_current_cluster_object_ptr->index = 0;
					m_current_cluster_object_ptr->layer_index = active_layer_index_;
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
					updateStates();
				}
			}
			else {
				ImGui::Text("X New Cluster");
				/*
				we can not create a new cluster object when we are mannually fixing the world cluster
				*/
			}
		}
		else if (editor_mode_ == EditorModeEnum::SELECT) {
			ImGui::Text("Selection");
			int before = static_cast<int>(m_selection_mode_index);
			int subject = before;
			ImGui::RadioButton("Cluster", &subject, 0); ImGui::SameLine();
			ImGui::RadioButton("Object ", &subject, 1); ImGui::SameLine();
			ImGui::RadioButton("Free ", &subject, 2); ImGui::SameLine();
			ImGui::RadioButton("Joints", &subject, 3);
			m_selection_mode_index = static_cast<SelectModeEnum>(subject);
			if (before != subject) {//we changed our selection critria
				m_selected_cluster_objects.clear();
				m_current_cluster_object_ptr = nullptr;
				m_current_shape_ptr = nullptr;
				m_current_joint_ptr = nullptr;
			}
		}

		if (m_current_cluster_object_ptr != nullptr) {
			ImGui::Separator();
			ImGui::Text("Cluster");

			if (editor_mode_ == EditorModeEnum::PLACE) {
				if (m_auto_assign_world_cluster) {
					ImGui::InputText("###CurrentWorldClustersName", active_layer.world_clusters.back().name, 20);
				}
				else {
					ImGui::InputText("###CurrentWorldClustersName", active_layer.world_clusters[m_mannually_choosen_world_cluster_index].name, 20);
				}
			}
			else if (editor_mode_ == EditorModeEnum::SELECT) {
				ImGui::InputText("###CurrentWorldClustersName", active_layer.world_clusters[m_current_cluster_object_ptr->cluster_index].name, 20);
			}

			ImGui::Text("Object name");
			ImGui::InputText("###CurrentClusterObjectsName", m_current_cluster_object_ptr->name, 20);
			ImGui::Separator();
		}
		ImGui::EndPopup();
	}

	if (m_current_cluster_object_ptr != nullptr) {
		ImGui::Separator();
		ImGui::Text("Cluster");
		ImGui::InputText("###CurrentWorldClustersName", active_layer.world_clusters[m_current_cluster_object_ptr->cluster_index].name, 20);
		ImGui::Text("Object name");
		ImGui::InputText("###CurrentClusterObjectsName", m_current_cluster_object_ptr->name, 20);
		ImGui::Separator();
	}

	if (editor_mode_ == EditorModeEnum::PLACE) {
		if (m_auto_assign_world_cluster) {
			if (ImGui::Button("New Cluster")) {
				m_auto_assign_world_cluster = true;//we are lossing control
				active_layer.world_clusters.emplace_back();
				auto& new_world_cluster = active_layer.world_clusters.back();
				new_world_cluster.index = active_layer.world_clusters.size() - 1;
				m_utilities.nameWorldClusterByIndex(new_world_cluster);
				m_current_cluster_object_ptr = &new_world_cluster.cluster_objects.back();
				m_current_cluster_object_ptr->cluster_index = new_world_cluster.index;
				m_current_cluster_object_ptr->index = 0;
				m_current_cluster_object_ptr->layer_index = active_layer.index;
				m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				updateStates();
			}
		}
	}
	else if (editor_mode_ == EditorModeEnum::SELECT) {
		ImGui::Text("Selection");
		int before = static_cast<int>(m_selection_mode_index);
		int subject = before;
		ImGui::RadioButton("Cluster", &subject, 0); ImGui::SameLine();
		ImGui::RadioButton("Object ", &subject, 1); ImGui::SameLine();
		ImGui::RadioButton("Free ", &subject, 2); ImGui::SameLine();
		ImGui::RadioButton("Joints", &subject, 3);
		m_selection_mode_index = static_cast<SelectModeEnum>(subject);
		if (before != subject) {//we changed our selection critria
			m_selected_cluster_objects.clear();
			m_current_cluster_object_ptr = nullptr;
			m_current_shape_ptr = nullptr;
			m_current_joint_ptr = nullptr;
		}
	}


	if (((editor_mode_ == EditorModeEnum::SELECT && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) || editor_mode_ == EditorModeEnum::PLACE) && m_current_cluster_object_ptr != nullptr) {
		ImGui::Text("Color");
		static float col2[4];

		col2[0] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->texture_info.color.x), static_cast<float>(255));
		col2[1] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->texture_info.color.y), static_cast<float>(255));
		col2[2] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->texture_info.color.z), static_cast<float>(255));
		col2[3] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->texture_info.color.w), static_cast<float>(255));

		ImGui::ColorEdit4("Clear color", col2);
		m_current_cluster_object_ptr->texture_info.color.x = static_cast<int>(col2[0] * 255);
		m_current_cluster_object_ptr->texture_info.color.y = static_cast<int>(col2[1] * 255);
		m_current_cluster_object_ptr->texture_info.color.z = static_cast<int>(col2[2] * 255);
		m_current_cluster_object_ptr->texture_info.color.w = static_cast<int>(col2[3] * 255);

		int shape_type_enum = static_cast<int>(m_place_mode_shape_type);
		ImGui::RadioButton("Single Shape", &shape_type_enum, 0); ImGui::SameLine();
		ImGui::RadioButton("Compound Shape", &shape_type_enum, 1);
		m_place_mode_shape_type = static_cast<ShapeTypeEnum>(shape_type_enum);
		if (m_place_mode_shape_type == ShapeTypeEnum::SINGLESHAPE) {
			//Object Types
			ImGui::Spacing();
			//display our shapes names
			ImGui::Text("Shape     : "); ImGui::SameLine();

			for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
			{
				shapes_labels_ptr[i] = ToString(m_vec_shapes_pointer[i]->body_shape);
			}
			if (editor_mode_ == EditorModeEnum::SELECT) {
				ImGui::BeginDisabled();
			}
			ImGui::Combo("###Shape", &m_selected_shape, shapes_labels_ptr, m_vec_shapes_pointer.size());
			if (editor_mode_ == EditorModeEnum::SELECT) {
				ImGui::EndDisabled();
			}
			//get our shapes dimensions

			for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
			{
				if (m_selected_shape == i) {
					ImGui::Spacing();
					ImGui::Text("%s Dimensions", ToString(m_vec_shapes_pointer[i]->body_shape));
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					m_current_cluster_object_ptr->shape = m_vec_shapes_pointer[i]->body_shape;
					m_current_shape_ptr = m_vec_shapes_pointer[i];

					if (m_current_shape_ptr->needs_height_width) {
						ImGui::Text("Height    : "); ImGui::SameLine();
						ImGui::InputFloat("###b_h", &m_current_cluster_object_ptr->height, 0.05f, 0, "%.3f");
						ImGui::Text("Width     : "); ImGui::SameLine();
						ImGui::InputFloat("###b_w", &m_current_cluster_object_ptr->width, 0.05f, 0, "%.3f");
					}
					if (m_current_shape_ptr->needs_radius || m_vec_shapes_pointer[i]->is_calculated_light) {
						ImGui::Text("Radius     : "); ImGui::SameLine();
						ImGui::InputFloat("###c_r", &m_current_cluster_object_ptr->radius, 0.025f, 0, "%.3f");
					}

					if (m_current_shape_ptr->is_plotted || m_vec_shapes_pointer[i]->is_calculated_light) {
						ImGui::Checkbox("Show Cords", &m_show_polygon_plots);
						if (m_show_polygon_plots) {
							if (m_current_cluster_object_ptr->vertices.size() >= 2) {
								if (m_current_cluster_object_ptr->vertices.size() % 2 == 0) {
									for (unsigned int i = 0; i < m_current_cluster_object_ptr->vertices.size() - 1; i++)
									{
										ImGui::Text("(%.2f,%.2f) (%.2f,%.2f)", m_current_cluster_object_ptr->vertices[i].x, m_current_cluster_object_ptr->vertices[i].y, m_current_cluster_object_ptr->vertices[i + 1], m_current_cluster_object_ptr->vertices[i + 1].y);
									}
								}
								else {
									for (int i = 0; i < static_cast<int>(m_current_cluster_object_ptr->vertices.size() - 2); i++)
									{
										ImGui::Text("(%.2f,%.2f) (%.2f,%.2f)", m_current_cluster_object_ptr->vertices[i].x, m_current_cluster_object_ptr->vertices[i].y, m_current_cluster_object_ptr->vertices[i + 1].x, m_current_cluster_object_ptr->vertices[i + 1].y);
									}
									ImGui::Text("(%.2f,%.2f) (?,?)", m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].x, m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].y);
								}
							}
						}

						if (m_current_shape_ptr->is_calculated_light) {
							if (ImGui::Button("Add Edge")) {
								if (static_cast<int>(m_current_cluster_object_ptr->vertices.size()) >= m_vec_shapes_pointer[i]->min_plotting_vertices)
								{
									const glm::vec2& start = m_current_cluster_object_ptr->vertices[0];
									const glm::vec2& end = m_current_cluster_object_ptr->vertices[m_vec_shapes_pointer[i]->min_plotting_vertices - 1];
									m_current_cluster_object_ptr->vec_of_edges.push_back(std::pair(start, end));
									m_current_cluster_object_ptr->vertices.clear();
								}
							}

							if (ImGui::Button("Draw Light")) {
								if (m_current_cluster_object_ptr->vec_of_edges.size() >= 4)
								{
									m_draw_plotted_shape = true;
								}
								else {
									ImGui::OpenPopup("CantDrawPolt");
								}
							}
						}
						else {
							if (ImGui::Button("Draw")) {
								if (static_cast<int>(m_current_cluster_object_ptr->vertices.size()) >= m_vec_shapes_pointer[i]->min_plotting_vertices)
								{
									m_draw_plotted_shape = true;
								}
								else {
									ImGui::OpenPopup("CantDrawPolt");
								}
							}
						}

						if (ImGui::Button("Clear plots")) {
							m_current_cluster_object_ptr->vec_of_edges.clear();
							m_current_cluster_object_ptr->vertices.clear();
						}

						// Always center this window when appearing
						ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

						if (ImGui::BeginPopupModal("CantDrawPolt", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							int min_points = m_current_cluster_object_ptr->shape == SquareBox::BodyShapeEnum::CalculatedLightShape ? 4 : m_vec_shapes_pointer[i]->min_plotting_vertices;
							ImGui::Text("A minimum of %d  points is need", min_points);
							ImGui::Separator();

							if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::SetItemDefaultFocus();
							ImGui::EndPopup();
						}
					}
				}
			}

			ImGui::Text("Rotation  : "); ImGui::SameLine();
			ImGui::InputFloat("###rtv", &m_current_cluster_object_ptr->angle, 0.05f, 0, "%.3f");
			ImGui::Separator();


			ImGui::Text("Texture");
			ImGui::Separator();
			static int current_texture_type = 0;
			current_texture_type = static_cast<int>(m_current_cluster_object_ptr->texture_info.texture_type);
			ImGui::RadioButton("Single Sprite", &current_texture_type, 0); ImGui::SameLine();
			ImGui::RadioButton("Tile Sheet", &current_texture_type, 1);
			m_current_cluster_object_ptr->texture_info.texture_type = static_cast<SquareBox::TextureEnum>(current_texture_type);

			if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::SINGLE) {
				ImGui::Text("uvRect ");
				ImGui::InputFloat("uv_x", &m_current_cluster_object_ptr->texture_info.uv_rect.x, 0.01f, 0, "%.2f");
				ImGui::InputFloat("uv_y", &m_current_cluster_object_ptr->texture_info.uv_rect.y, 0.01f, 0, "%.2f");
				ImGui::InputFloat("uv_z", &m_current_cluster_object_ptr->texture_info.uv_rect.z, 0.01f, 0, "%.2f");
				ImGui::InputFloat("uv_w", &m_current_cluster_object_ptr->texture_info.uv_rect.w, 0.01f, 0, "%.2f");
				std::vector<char*>  vc;
				vc.reserve(active_layer.single_textures.size());
				std::transform(active_layer.single_textures.begin(), active_layer.single_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
				{
					//this new here is never deleted, and is called alot, that is a problem to us 
					char* pc = new char[parentTexture_.texture.display_name.size() + 1];
					std::strcpy(pc, parentTexture_.texture.display_name.c_str());
					return pc;
				});
				//compute how many single textures we want to display
				//we are only displaying those that are not empty strings
				if (active_layer.single_textures.size() > 0) {
					ImGui::Combo("Single", &m_selected_single_texture_index, &vc[0], active_layer.single_textures.size());
					//update the current objects texture info
					m_current_cluster_object_ptr->texture_info.texture_index = m_selected_single_texture_index;
					m_current_cluster_object_ptr->texture_info.texture_id = active_layer.single_textures[m_selected_single_texture_index].texture.id;
					//current active texture
					ImTextureID my_tex_id;
					my_tex_id = (ImTextureID)active_layer.single_textures[m_selected_single_texture_index].texture.id;

					float my_tex_w = 50;
					float my_tex_h = 50;
					float zoom = 4.0f;
					float texture_width = active_layer.single_textures[m_selected_single_texture_index].texture.width;
					float texture_height = active_layer.single_textures[m_selected_single_texture_index].texture.height;

					float region_width = m_current_cluster_object_ptr->texture_info.uv_rect.z * texture_width;
					float region_height = m_current_cluster_object_ptr->texture_info.uv_rect.w * texture_height;

					float region_x_orign = (m_current_cluster_object_ptr->texture_info.uv_rect.x) * texture_width;
					float region_y_orign = (1 - m_current_cluster_object_ptr->texture_info.uv_rect.w - m_current_cluster_object_ptr->texture_info.uv_rect.y) * texture_height;
					/*dont under stand why we have to subtract the height. this is a classic it works so dont touch it scenario*/
					ImVec2 uv0 = ImVec2((region_x_orign) / texture_width, (region_y_orign) / texture_height);
					ImVec2 uv1 = ImVec2((region_x_orign + region_width) / texture_width, (region_y_orign + region_height) / texture_height);

					ImGui::Text("Preview");
					//ImVec2 uv_min = ImVec2(m_current_cluster_object_ptr->texture_info.uv_rect.x, m_current_cluster_object_ptr->texture_info.uv_rect.y);                 // Top-left
					//ImVec2 uv_max = ImVec2(uv_min.x + m_current_cluster_object_ptr->texture_info.uv_rect.z, uv_min.y+ m_current_cluster_object_ptr->texture_info.uv_rect.w );                 // Lower-right
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
					ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv0, uv1, tint_col, border_col);

				}
				else {
					ImGui::Combo("Single", &m_selected_single_texture_index, "Empty", active_layer.single_textures.size());
				}


			}
			else if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::TILESHEET) {
				if (active_layer.tiled_textures.size() > 0) {
					ImGui::Text("Tiling:");
					ImGui::Text("cols : "); ImGui::SameLine();
					ImGui::Text("%d", active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.x);
					ImGui::Text("rows : "); ImGui::SameLine();
					ImGui::Text("%d", active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.y);

					std::vector<char*>  vc;
					vc.reserve(active_layer.tiled_textures.size());
					std::transform(active_layer.tiled_textures.begin(), active_layer.tiled_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
					{
						//this new here is never deleted, and is called alot, that is a problem to us 
						char* pc = new char[parentTexture_.texture.display_name.size() + 1];
						std::strcpy(pc, parentTexture_.texture.display_name.c_str());
						return pc;
					});

					ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], active_layer.tiled_textures.size());
					//update the current objects texture info
					auto& selected_tile_sheet_texture = active_layer.tiled_textures[m_selected_tile_sheet_texture_index].texture;
					m_current_cluster_object_ptr->texture_info.texture_index = m_selected_tile_sheet_texture_index;
					m_current_cluster_object_ptr->texture_info.texture_id = selected_tile_sheet_texture.id;
					//current active texture
					ImTextureID my_tex_id = (ImTextureID)selected_tile_sheet_texture.id;
					float my_tex_w = 50;
					float my_tex_h = 50;
					float zoom = 4.0f;
					ImGui::Text("Preview");

					/*
					we update the cluster objects uv coords here .if it is a freelance cluster Object
					*/
					glm::vec4 uvRect = selected_tile_sheet_texture.getUVReactAtIndex(m_current_cluster_object_ptr->texture_info.tile_sheet_index);

					if (m_current_cluster_object_ptr->controller_type == SquareBox::ControlledTypeEnum::freelance) {
						m_current_cluster_object_ptr->texture_info.uv_rect = uvRect;
					}

					float texture_width = selected_tile_sheet_texture.width;
					float texture_height = selected_tile_sheet_texture.height;

					float region_width = m_current_cluster_object_ptr->texture_info.uv_rect.z * texture_width;
					float region_height = m_current_cluster_object_ptr->texture_info.uv_rect.w * texture_height;

					float region_x_orign = (m_current_cluster_object_ptr->texture_info.uv_rect.x) * texture_width;
					float region_y_orign = (1 - m_current_cluster_object_ptr->texture_info.uv_rect.w - m_current_cluster_object_ptr->texture_info.uv_rect.y) * texture_height;
					/*dont under stand why we have to subtract the height. this is a classic it works so dont touch it scenario*/
					ImVec2 uv0 = ImVec2((region_x_orign) / texture_width, (region_y_orign) / texture_height);
					ImVec2 uv1 = ImVec2((region_x_orign + region_width) / texture_width, (region_y_orign + region_height) / texture_height);

					ImVec2 texture_dimensions(selected_tile_sheet_texture.width, selected_tile_sheet_texture.height);
					//ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);   // Top-left
					//ImVec2 uv_max = ImVec2(uv_min.x +uvRect.z , uv_min.y + uvRect.w);  // Lower-right
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white


					ImGui::Image(my_tex_id, ImVec2(my_tex_w * zoom, my_tex_h * zoom), uv0, uv1, tint_col, border_col);
					ImGui::Text("Index: "); ImGui::SameLine();
					//ImGui::InputInt("", &m_current_cluster_object_ptr->texture_info.tile_sheet_index);
					ImGui::DragInt("###m_current_tile_sheet_index", &m_current_cluster_object_ptr->texture_info.tile_sheet_index, 1, 0, (selected_tile_sheet_texture.tiling.x * selected_tile_sheet_texture.tiling.y) - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
					if (m_current_cluster_object_ptr->texture_info.tile_sheet_index < 0) {
						m_current_cluster_object_ptr->texture_info.tile_sheet_index = 0;
					}

				}
				else {
					ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, "Empty", active_layer.tiled_textures.size());
				}


			}
		}
		else if (m_place_mode_shape_type == ShapeTypeEnum::COMPOUNDSHAPE) {
		}
		ImGui::Separator();
	}
	else if (editor_mode_ == EditorModeEnum::SELECT && m_selection_mode_index == SelectModeEnum::FREESELECT && m_selected_cluster_objects.size() > 0) {

		ImGui::InputFloat("Universal Angle : ", &m_free_select_rotation_universal_angle, 0.01f, 0.1f, "%.3f");
		if (ImGui::Button("Fix Angles")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle = m_free_select_rotation_universal_angle;
				ccobj.requesting_recreation = true;
			}
		}
		ImGui::InputFloat("Angle Padding : ", &m_free_select_rotation_padding, 0.01f, 0.1f, "%.3f");
		ImGui::Text("Angle Rotations");
		if (ImGui::SmallButton("Angle +")) {
			//loop through all the selected objects and apply the roation to them 
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle -= m_free_select_rotation_padding;
				ccobj.requesting_recreation = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Angle -")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle += m_free_select_rotation_padding;
				ccobj.requesting_recreation = true;
			}
		}

		ImGui::Text("Position Rotations");
		if (ImGui::SmallButton("Position +")) {
			//loop through all the selected objects and apply the roation to them 
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				SBX_INFO("Before {} {}", ccobj.position.x, ccobj.position.y);
				m_utilities.rotatePoint(ccobj.position, -m_free_select_rotation_padding);
				SBX_INFO("After {} {}", ccobj.position.x, ccobj.position.y);

				ccobj.requesting_recreation = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Position -")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				m_utilities.rotatePoint(ccobj.position, m_free_select_rotation_padding);
				ccobj.requesting_recreation = true;
			}
		}
	}
}

void FlatLayer::onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];

	ImGui::NewLine();
	ImGui::Checkbox("Run Physics             ", &m_run_physics);
	ImGui::Checkbox("Play Animations         ", &m_play_animations);
	ImGui::Checkbox("Respect Ancesor         ", &m_respect_ancestor);
	ImGui::Checkbox("Draw Edges              ", &m_always_draw_edges);
	ImGui::Checkbox("Show scaling lines      ", &m_show_scaling_in_debug_mode);
	ImGui::Checkbox("Auto Assign WorldCluster", &m_auto_assign_world_cluster);
	ImGui::NewLine();
	/*

  map{
	key : WorldClusterIndex

	 value:  pair{

			first{

						WorlClusterName
				}

			 second{
					vector{
							pair{
								ClusterObjectName, ClusterObjectIndex
							}
					}
			 }

		  }

	}

	*/

	std::map<int, std::pair<std::string, std::vector<std::pair<std::string, int>>>> layer_world_clusters_layout;
	for (unsigned int i = 0; i < active_layer.alive_cluster_objects.size(); i++)
	{
		const auto& focus_cluster_object_coordinates = active_layer.alive_cluster_objects[i];
		SquareBox::GWOM::WorldCluster& focus_world_cluster = layers_[active_layer_index_].world_clusters[focus_cluster_object_coordinates.first];
		auto checker = layer_world_clusters_layout.find(focus_world_cluster.index);

		if (checker != layer_world_clusters_layout.end()) {
			//it was found
		}
		else {
			std::string world_cluster_name = focus_world_cluster.name;

			//add it
			layer_world_clusters_layout[focus_world_cluster.index] = std::pair<std::string, std::vector<std::pair<std::string, int>>>(
				world_cluster_name, std::vector<std::pair<std::string, int>>(0));
		}

		auto clerk = layer_world_clusters_layout.find(focus_world_cluster.index);

		SquareBox::GWOM::ClusterObject& focus_cluster_object = focus_world_cluster.cluster_objects[focus_cluster_object_coordinates.second];
		std::pair<std::string, int> raw_data_pair = std::pair<std::string, int>(focus_cluster_object.name, focus_cluster_object.index);
		clerk->second.second.push_back(raw_data_pair);
	}

	//layer layerout node
	if (ImGui::TreeNode("Layer layout")) {
		for (unsigned int i = 0; i < layer_world_clusters_layout.size(); i++)
		{
			if (ImGui::TreeNode(layer_world_clusters_layout[i].first.c_str())) {
				for (unsigned int j = 0; j < layer_world_clusters_layout[i].second.size(); j++)
				{
					if (ImGui::TreeNode((void*)(intptr_t)j, "%s %d", layer_world_clusters_layout[i].second[j].first.c_str(), layer_world_clusters_layout[i].second[j].second))
					{
						std::pair<int, int> target_cluster_object_coordinates = std::pair<int, int>(active_layer_index_, layer_world_clusters_layout[i].second[j].second);

						SquareBox::GWOM::ClusterObject& target_cluster_object = active_layer.world_clusters[target_cluster_object_coordinates.first].cluster_objects[target_cluster_object_coordinates.second];

						if (ImGui::SmallButton("up")) {
							m_utilities.movePairInVector(active_layer.alive_cluster_objects, target_cluster_object_coordinates, -1);
						}
						ImGui::SameLine();
						if (ImGui::SmallButton("down")) {
							m_utilities.movePairInVector(active_layer.alive_cluster_objects, target_cluster_object_coordinates, 1);
						}
						ImGui::SameLine();
						if (target_cluster_object.is_hidden) {
							if (ImGui::SmallButton("Show")) {
								target_cluster_object.is_hidden = false;
							}
						}

						else {
							if (ImGui::SmallButton("Hide")) {
								target_cluster_object.is_hidden = true;
							}
						}
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::Button("Reload Animations Script"))
	{
		m_animation_creator.loadAnimationScript(m_animation_script);
	}

	ImGui::NewLine();
	if (!m_auto_assign_world_cluster) {
		m_show_set_mannual_wc_index_dialog = true;
	}
	if (m_show_set_mannual_wc_index_dialog) {
		ImGui::OpenPopup("AutoAssignWorldCluster?");
	}

	if (ImGui::BeginPopupModal("AutoAssignWorldCluster?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Must be in the range of the WorldCluster vector size()\n");
		ImGui::Separator();
		ImGui::DragInt("WorldClusterIndex", &m_mannually_choosen_world_cluster_index, 1, 0, active_layer.world_clusters.size() - 1, "%d%", ImGuiSliderFlags_AlwaysClamp);
		if (m_mannually_choosen_world_cluster_index < 0) {
			m_mannually_choosen_world_cluster_index = 0;
		}
		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_auto_assign_world_cluster = false;
			m_show_set_mannual_wc_index_dialog = false;

			//check if we have a template at the end of the mannually selected world cluster to work with
			if (active_layer.world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.back().is_alive) {
				//push back a shell to use
				active_layer.world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.emplace_back();
			}

			m_current_cluster_object_ptr = &active_layer.world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.back();

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			m_auto_assign_world_cluster = true;
			m_show_set_mannual_wc_index_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void FlatLayer::onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
	SquareBox::GWOM::Layer& active_layer = layers_[active_layer_index_];

	if (m_selected_cluster_objects.size() > 0) {
		if (ImGui::Button("Shift Selected Objects")) {
			//we active a pop up that lets us shift all selected Objects to another layer
			m_show_shift_cobjs_to_another_layer_dialog = true;
		}
		ImGui::SameLine();
	}
	if (ImGui::Button("Unhide All Objects")) {
		for (unsigned int i = 0; i < active_layer.alive_cluster_objects.size(); i++)
		{
			SquareBox::GWOM::ClusterObject* cwobj = &active_layer.world_clusters[active_layer.alive_cluster_objects[i].first].cluster_objects[active_layer.alive_cluster_objects[i].second];
			//actually delete these cluster Objects
			cwobj->is_hidden = false;
		}
	}

	if (m_show_shift_cobjs_to_another_layer_dialog) {
		ImGui::OpenPopup("ShiftCobjsToAnotherLayer?");
	}
	if (ImGui::BeginPopupModal("ShiftCobjsToAnotherLayer?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Shifting cluster Objects\nFrom: %s \n", active_layer.name);
		ImGui::Separator();

		ImGui::DragInt("New Layer index", &m_target_shift_to_layer_index, 1, 0, layers_.size() - 1, "%d%", ImGuiSliderFlags_AlwaysClamp);
		if (m_target_shift_to_layer_index < 0) {
			m_target_shift_to_layer_index = 0;
		}
		ImGui::Text("To: %s", layers_[m_target_shift_to_layer_index].name);

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			m_show_shift_cobjs_to_another_layer_dialog = false;
			if (m_target_shift_to_layer_index == active_layer_index_) {
				/*
				we are still going with the flow here because it gives the user a way to move a bunch of
				objects to the back of the layers draw calls
				*/
			}
			//remove all the selected cluster objects from the active layer
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				//kill the objects
				active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second].is_alive = false;
				m_utilities.removePairFromVector(active_layer.alive_cluster_objects, m_selected_cluster_objects[i]);
				m_utilities.destroryClusterObjectJoints(layers_, active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second], &m_physics_world);
				//destorying all the joints because joints should only be between objects the same , best option would be to do checks
				// but maybe willl implement this when am dealing with joints
			}
			//add to the target layer
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {

				//create a shell for it
				int m_target_world_cluster_in_target_layer = layers_[m_target_shift_to_layer_index].world_clusters.size() - 1;
				layers_[m_target_shift_to_layer_index].world_clusters[m_target_world_cluster_in_target_layer].cluster_objects.push_back(active_layer.world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second]);
				auto& newly_shifted_object = layers_[m_target_shift_to_layer_index].world_clusters[m_target_world_cluster_in_target_layer].cluster_objects.back();
				newly_shifted_object.layer_index = m_target_shift_to_layer_index;
				newly_shifted_object.cluster_index = m_target_world_cluster_in_target_layer;
				newly_shifted_object.index = layers_[m_target_shift_to_layer_index].world_clusters[m_target_world_cluster_in_target_layer].cluster_objects.size();
				m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(), &newly_shifted_object, layers_, &m_physics_world, false, false);
				//its auto added to the alive_cluster_objects by the createWorldClusterObject
			}

			m_selected_cluster_objects.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			m_show_shift_cobjs_to_another_layer_dialog = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if ((editor_mode_ == EditorModeEnum::SELECT && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT && m_current_cluster_object_ptr != nullptr) || editor_mode_ == EditorModeEnum::PLACE) {
		ImGui::NewLine();
		ImGui::Text("%s Objects' Properties ", m_current_cluster_object_ptr->name);
	}
	ImGui::NewLine();

	int object_properties_subject = static_cast<int>(m_object_properties_enum);
	ImGui::RadioButton("Physics Properties", &object_properties_subject, 0); ImGui::SameLine();
	ImGui::RadioButton("Animation", &object_properties_subject, 1); ImGui::Spacing();
	m_object_properties_enum = static_cast<ObjectPropertiesEnum>(object_properties_subject);

	if (m_object_properties_enum == ObjectPropertiesEnum::PHYSICS) {

		if ((editor_mode_ == EditorModeEnum::SELECT && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT || editor_mode_ == EditorModeEnum::PLACE) && m_current_cluster_object_ptr != nullptr) {
			ImGui::Checkbox("Enable Physics", &m_enable_physics);
			if (!m_enable_physics) {
				ImGui::BeginDisabled();

			}

			ImGui::Spacing();
			ImGui::Text("Body Type");
			ImGui::Spacing();
			ImGui::RadioButton("Dynamic ", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::DynamicBody)); ImGui::SameLine();
			ImGui::RadioButton("Static", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::StaticBody)); ImGui::SameLine();
			ImGui::RadioButton("Kinematic", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::KinematicBody));
			m_current_cluster_object_ptr->type = static_cast<SquareBox::BodyTypeEnum>(m_selected_body_type);
			ImGui::Spacing();
			ImGui::Text("World Cluster  Index %d", m_current_cluster_object_ptr->cluster_index);
			ImGui::Spacing();
			ImGui::Text("Cluster Object Index %d", m_current_cluster_object_ptr->index);
			ImGui::Spacing();
			bool enable_cluster_object_rotation = !m_current_cluster_object_ptr->is_fixed_rotation;
			ImGui::Text("Enable Rotation   : "); ImGui::SameLine();
			ImGui::Checkbox("###rot", &enable_cluster_object_rotation);
			m_current_cluster_object_ptr->is_fixed_rotation = !enable_cluster_object_rotation;

			ImGui::Text("Bullet Collision  : "); ImGui::SameLine();
			ImGui::Checkbox("###bulet", &m_current_cluster_object_ptr->is_bullet);

			ImGui::Text("Density       : "); ImGui::SameLine();
			ImGui::InputFloat("###d_t", &m_current_cluster_object_ptr->density, 0.05f, 0, "%.3f");

			ImGui::Text("Fricition     : "); ImGui::SameLine();
			ImGui::InputFloat("###f_r", &m_current_cluster_object_ptr->friction, 0.05f, 0, "%.3f");

			ImGui::Text("Restitution   : "); ImGui::SameLine();
			ImGui::InputFloat("###r_s", &m_current_cluster_object_ptr->restitution, 0.05f, 0, "%.3f");

			ImGui::Text("gravityScale  : "); ImGui::SameLine();
			ImGui::InputFloat("###g_s", &m_current_cluster_object_ptr->gravity_scale, 0.01f, 0, "%.3f");

			ImGui::Text("linearDamping : "); ImGui::SameLine();
			ImGui::InputFloat("###l_d", &m_current_cluster_object_ptr->linear_damping, 0.01f, 0, "%.3f");

			ImGui::Text("angularDamping: "); ImGui::SameLine();
			ImGui::InputFloat("###a_d", &m_current_cluster_object_ptr->angular_damping, 0.01f, 0, "%.3f");

			ImGui::Text("IAV	       : "); ImGui::SameLine();
			ImGui::InputFloat("###i_v", &m_current_cluster_object_ptr->init_angular_velocity, 0.05f, 0, "%.3f");

			if (ImGui::TreeNode("Collision Bounding"))
			{
				ImGui::RadioButton("Family a", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_a));
				ImGui::RadioButton("Family b", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_b));
				ImGui::RadioButton("Family c", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_c));
				ImGui::RadioButton("Family d", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_d));
				ImGui::RadioButton("Family e", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_e));
				ImGui::RadioButton("Family f", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_f));
				ImGui::RadioButton("Family g", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_g));
				ImGui::RadioButton("Family h", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_h));
				ImGui::RadioButton("Family i", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_i));
				ImGui::RadioButton("Family j", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_j));
				ImGui::RadioButton("Family k", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_k));
				ImGui::RadioButton("Family l", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_l));
				ImGui::RadioButton("Family m", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_m));
				ImGui::RadioButton("Family n", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_n));
				ImGui::RadioButton("Family o", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_o));
				ImGui::RadioButton("Family p", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_p));

				m_current_cluster_object_ptr->family = static_cast<SquareBox::FamilyBitsEnum>(m_selected_family);

				ImGui::Text("Mask Bits");
				{
					static bool has_mask_a;
					//getting current state
					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a))) {
						has_mask_a = true;
					}
					else {
						has_mask_a = false;
					}
					ImGui::Checkbox("Mask a", &has_mask_a);
					//acknowledging the changes
					if (has_mask_a) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a));
					}

					static bool has_mask_b = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b))) {
						has_mask_b = true;
					}
					else {
						has_mask_b = false;
					}
					ImGui::Checkbox("Mask b", &has_mask_b);
					//acknowledging the changes
					if (has_mask_b) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b));
					}

					static bool has_mask_c = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c))) {
						has_mask_c = true;
					}
					else {
						has_mask_c = false;
					}
					ImGui::Checkbox("Mask c", &has_mask_c);
					//acknowledging the changes
					if (has_mask_c) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c));
					}

					static bool has_mask_d = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d))) {
						has_mask_d = true;
					}
					else {
						has_mask_d = false;
					}
					ImGui::Checkbox("Mask d", &has_mask_d);
					//acknowledging the changes
					if (has_mask_d) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d));
					}

					static bool has_mask_e = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e))) {
						has_mask_e = true;
					}
					else {
						has_mask_e = false;
					}
					ImGui::Checkbox("Mask e", &has_mask_e);
					//acknowledging the changes
					if (has_mask_e) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e));
					}

					static bool has_mask_f = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f))) {
						has_mask_f = true;
					}
					else {
						has_mask_f = false;
					}
					ImGui::Checkbox("Mask f", &has_mask_f);
					//acknowledging the changes
					if (has_mask_f) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f));
					}

					static bool has_mask_g = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g))) {
						has_mask_g = true;
					}
					else {
						has_mask_g = false;
					}
					ImGui::Checkbox("Mask g", &has_mask_g);
					//acknowledging the changes
					if (has_mask_g) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g));
					}

					static bool has_mask_h = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h))) {
						has_mask_h = true;
					}
					else {
						has_mask_h = false;
					}
					ImGui::Checkbox("Mask h", &has_mask_h);
					//acknowledging the changes
					if (has_mask_h) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h));
					}

					static bool has_mask_i = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i))) {
						has_mask_i = true;
					}
					else {
						has_mask_i = false;
					}
					ImGui::Checkbox("Mask i", &has_mask_i);
					//acknowledging the changes
					if (has_mask_i) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i));
					}

					static bool has_mask_j = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j))) {
						has_mask_j = true;
					}
					else {
						has_mask_j = false;
					}
					ImGui::Checkbox("Mask j", &has_mask_j);
					//acknowledging the changes
					if (has_mask_j) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j));
					}

					static bool has_mask_k = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k))) {
						has_mask_k = true;
					}
					else {
						has_mask_k = false;
					}
					ImGui::Checkbox("Mask k", &has_mask_k);
					//acknowledging the changes
					if (has_mask_k) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k));
					}

					static bool has_mask_l = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l))) {
						has_mask_l = true;
					}
					else {
						has_mask_l = false;
					}
					ImGui::Checkbox("Mask l", &has_mask_l);
					//acknowledging the changes
					if (has_mask_l) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l));
					}

					static bool has_mask_m = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m))) {
						has_mask_m = true;
					}
					else {
						has_mask_m = false;
					}
					ImGui::Checkbox("Mask m", &has_mask_m);
					//acknowledging the changes
					if (has_mask_m) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m));
					}

					static bool has_mask_n = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n))) {
						has_mask_n = true;
					}
					else {
						has_mask_n = false;
					}
					ImGui::Checkbox("Mask n", &has_mask_n);
					//acknowledging the changes
					if (has_mask_n) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n));
					}

					static bool has_mask_o = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o))) {
						has_mask_o = true;
					}
					else {
						has_mask_o = false;
					}
					ImGui::Checkbox("Mask o", &has_mask_o);
					//acknowledging the changes
					if (has_mask_o) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o));
					}

					static bool has_mask_p = false;
					//getting current state

					if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p))) {
						has_mask_p = true;
					}
					else {
						has_mask_p = false;
					}
					ImGui::Checkbox("Mask p", &has_mask_p);
					//acknowledging the changes
					if (has_mask_p) {
						//if not part add it
						m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p));
					}
					else {
						//if part remove it
						m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p));
					}
				}

				ImGui::Text("Grouping Index");
				ImGui::SameLine();
				ImGui::InputInt("###grouping_index", &m_current_cluster_object_ptr->group_index);
				ImGui::TreePop();
			}

			if (!m_enable_physics) {
				ImGui::EndDisabled();

			}
		}
		else if (m_selection_mode_index == SelectModeEnum::JOINTSSELECT && editor_mode_ == EditorModeEnum::SELECT) {
			ImGui::Checkbox("Show Joints", &m_debug_show_joints);


			//for the pulley Joint
			if (m_current_joint_ptr != nullptr) {

				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {

					if (ImGui::Button("Clear Plots")) {
						m_current_joint_ptr->vec_of_points.clear();
					}
				}
			}
			if (m_selected_cluster_objects.size() == 2) {
				//loop through these bodies joints and get the details of the joint between them
				//clusterObjects can have onlyOne joint between them
				SquareBox::GWOM::ClusterObject& ccobjA = active_layer.world_clusters[m_selected_cluster_objects[0].first].cluster_objects[m_selected_cluster_objects[0].second];
				SquareBox::GWOM::ClusterObject& ccobjB = active_layer.world_clusters[m_selected_cluster_objects[1].first].cluster_objects[m_selected_cluster_objects[1].second];
				for (unsigned int i = 0; i < ccobjA.joints.size(); i++)
				{
					SquareBox::GWOM::Joint& focus_joint = ccobjA.joints[i];
					const std::pair<int, int>& bodyAACoordinates = std::pair(focus_joint.body_a_coordinates.first, focus_joint.body_a_coordinates.second);
					const std::pair<int, int>& bodyABCoordinates = std::pair(focus_joint.body_b_coordinates.first, focus_joint.body_b_coordinates.second);
					if (
						(
							bodyAACoordinates == std::pair(ccobjA.cluster_index, ccobjA.index)
							&&
							bodyABCoordinates == std::pair(ccobjB.cluster_index, ccobjB.index)
							)
						||
						(
							bodyAACoordinates == std::pair(ccobjB.cluster_index, ccobjB.index)
							&&
							bodyABCoordinates == std::pair(ccobjA.cluster_index, ccobjA.index)
							)
						) {
						m_current_joint_ptr = &ccobjA.joints[i];
						break;//break the loop
					}
				}
				if (m_current_joint_ptr == nullptr) {
					//we don't have a joint so create one
					m_current_joint_ptr = &m_joint_template;
					m_current_joint_ptr->is_joint_alive = false;

					m_current_joint_ptr->body_a_coordinates = std::pair(ccobjA.cluster_index, ccobjA.index);
					m_current_joint_ptr->body_b_coordinates = std::pair(ccobjB.cluster_index, ccobjB.index);
				}
				std::pair<int, int> currentjointbodyACoords = std::pair(m_current_joint_ptr->body_a_coordinates.first, m_current_joint_ptr->body_a_coordinates.second);
				std::pair<int, int> currentjointbodyBCoords = std::pair(m_current_joint_ptr->body_b_coordinates.first, m_current_joint_ptr->body_b_coordinates.second);
				//incase of any discripases this is to be considered a logical error
				if (
					!(
						(currentjointbodyACoords == std::pair(ccobjA.cluster_index, ccobjA.index) &&
							currentjointbodyBCoords == std::pair(ccobjB.cluster_index, ccobjB.index))
						||
						(currentjointbodyACoords == std::pair(ccobjB.cluster_index, ccobjB.index) &&
							currentjointbodyBCoords == std::pair(ccobjA.cluster_index, ccobjA.index)))
					) {
					__debugbreak();
				}
				ImGui::NewLine();
				const char* JointTypes[]{ ToString(SquareBox::JointTypeEnum::distance_joint),ToString(SquareBox::JointTypeEnum::revolute_joint),ToString(SquareBox::JointTypeEnum::prismatic_joint),ToString(SquareBox::JointTypeEnum::pulley_joint) };
				int selectedjointtype = static_cast<int>(m_current_joint_ptr->joint_type);
				ImGui::Combo("Joint Type ", &selectedjointtype, JointTypes, IM_ARRAYSIZE(JointTypes));
				m_current_joint_ptr->joint_type = static_cast<SquareBox::JointTypeEnum>(selectedjointtype);
				ImGui::NewLine();

				ImGui::Text("Body A : "); ImGui::SameLine();
				ImGui::Text(layers_[active_layer_index_].world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second].name);
				ImGui::Text("Body B : "); ImGui::SameLine();
				ImGui::Text(layers_[active_layer_index_].world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second].name);

				ImGui::Spacing();
				ImGui::Checkbox("Collision  ", &m_current_joint_ptr->collide_connected_bodies);
				ImGui::Spacing();

				ImGui::Text("Body A anchor: ");
				ImGui::InputFloat("###j_body_a_anchor_x", &m_current_joint_ptr->local_anchor_a.x, 0.01f, 0, "%.3f");
				ImGui::InputFloat("###j_body_a_anchor_y", &m_current_joint_ptr->local_anchor_a.y, 0.01f, 0, "%.3f");
				ImGui::Spacing();

				ImGui::Text("Body B anchor: ");
				ImGui::InputFloat("###j_body_b_anchor_x", &m_current_joint_ptr->local_anchor_b.x, 0.01f, 0, "%.3f");
				ImGui::InputFloat("###j_body_b_anchor_y", &m_current_joint_ptr->local_anchor_b.y, 0.01f, 0, "%.3f");
				ImGui::Spacing();

				//unique Attributes
				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint) {
					ImGui::Text("Lower Angle      :"); ImGui::SameLine();
					ImGui::InputFloat("###j_lower_angle", &m_current_joint_ptr->lower_angle, 0.01f, 0, "%.3f");
					ImGui::Text("Upper Angle      :"); ImGui::SameLine();
					ImGui::InputFloat("###j_upper_angle", &m_current_joint_ptr->upper_angle, 0.01f, 0, "%.3f");
					ImGui::Text("Max Motor Torque :"); ImGui::SameLine();
					ImGui::InputFloat("###j_maxMotorTorque", &m_current_joint_ptr->max_motor_torque, 0.01f, 0, "%.3f");
				}
				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
					ImGui::Text("Lower Translation:"); ImGui::SameLine();
					ImGui::InputFloat("###j_lower_translation", &m_current_joint_ptr->lower_translation, 0.01f, 0, "%.3f");
					ImGui::Text("Upper Translation:"); ImGui::SameLine();
					ImGui::InputFloat("###j_upper_translation", &m_current_joint_ptr->upper_translation, 0.01f, 0, "%.3f");

					ImGui::Text("Max Motor Force  :"); ImGui::SameLine();
					ImGui::InputFloat("###j_maxMotorForce", &m_current_joint_ptr->max_motor_force, 0.01f, 0, "%.3f");

					ImGui::Text("Local Axis A     :");
					ImGui::InputFloat("###j_localAxisA_x", &m_current_joint_ptr->local_axis_a.x, 0.01f, 0, "%.3f");
					ImGui::InputFloat("###j_localAxisA_y", &m_current_joint_ptr->local_axis_a.y, 0.01f, 0, "%.3f");
				}
				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::distance_joint) {
					//distance
					ImGui::Text("DampingRatio     :"); ImGui::SameLine();
					ImGui::InputFloat("###j_dr", &m_current_joint_ptr->damping_ratio, 0.01f, 0, "%.3f");

					ImGui::Text("frequencyHz      :"); ImGui::SameLine();
					ImGui::InputFloat("###j_f", &m_current_joint_ptr->frequency_hz, 0.01f, 0, "%.3f");

					ImGui::Text("Stiffness        :"); ImGui::SameLine();
					ImGui::InputFloat("###j_stiffness", &m_current_joint_ptr->stiffness, 0.01f, 0, "%.3f");

					ImGui::Text("Length           :"); ImGui::SameLine();
					ImGui::InputFloat("###j_length", &m_current_joint_ptr->length, 0.01f, 0, "%.3f");

					ImGui::Text("Max Length       :"); ImGui::SameLine();
					ImGui::InputFloat("###j_max_length", &m_current_joint_ptr->max_length, 0.01f, 0, "%.3f");

					ImGui::Text("MinLength        :"); ImGui::SameLine();
					ImGui::InputFloat("###j_min_length", &m_current_joint_ptr->min_length, 0.01f, 0, "%.3f");
				}
				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
					if (m_current_joint_ptr->vec_of_points.size() == 2) {
						m_current_joint_ptr->ground_anchor_a = m_current_joint_ptr->vec_of_points[0];
						m_current_joint_ptr->ground_anchor_b = m_current_joint_ptr->vec_of_points[1];
						m_current_joint_ptr->length_a = fabs(glm::length(m_current_joint_ptr->vec_of_points[0] - ccobjA.physics_properties->getXYPosition()));
						m_current_joint_ptr->length_b = fabs(glm::length(m_current_joint_ptr->vec_of_points[1] - ccobjB.physics_properties->getXYPosition()));
					}
					ImGui::Text("Pulley Ratio     :"); ImGui::SameLine();
					ImGui::InputFloat("###j_pulley_ratio", &m_current_joint_ptr->pulley_ratio, 0.01f, 0, "%.3f");
					static float pulley_joint_color[4];
					pulley_joint_color[0] = m_current_joint_ptr->color.x;
					pulley_joint_color[1] = m_current_joint_ptr->color.y;
					pulley_joint_color[2] = m_current_joint_ptr->color.z;
					pulley_joint_color[3] = m_current_joint_ptr->color.w;

					ImGui::ColorEdit4("Pulley color:", pulley_joint_color);

					m_current_joint_ptr->color.x = pulley_joint_color[0];
					m_current_joint_ptr->color.y = pulley_joint_color[1];
					m_current_joint_ptr->color.z = pulley_joint_color[2];
					m_current_joint_ptr->color.w = pulley_joint_color[3];

					ImGui::InputFloat("###j_pulley_rope_thickness", &m_current_joint_ptr->thickness, 0.01f, 0, "%.3f");
				}

				//shared attributes
				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint || m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
					ImGui::Text("Reference Angle  :"); ImGui::SameLine();
					ImGui::InputFloat("###j_reference_angle", &m_current_joint_ptr->reference_angle, 0.01f, 0, "%.3f");
					ImGui::Text("Motor Speed      :"); ImGui::SameLine();
					ImGui::InputFloat("###j_motor_speed", &m_current_joint_ptr->motor_speed, 0.01f, 0, "%.3f");

					bool enable_joint_limit = m_current_joint_ptr->enable_limit;
					ImGui::Checkbox("Enable Limit", &enable_joint_limit);
					m_current_joint_ptr->enable_limit = enable_joint_limit;

					bool enable_joint_motor = m_current_joint_ptr->enable_motor;
					ImGui::Checkbox("Enable Motor", &enable_joint_motor);
					m_current_joint_ptr->enable_motor = enable_joint_motor;
				}

				if (m_current_joint_ptr->is_joint_alive) {
					if (ImGui::Button("Update Joint")) {
						if (ccobjA.physics_properties != nullptr && ccobjB.physics_properties != nullptr) {
							m_utilities.destroyClusterObjectJoint(active_layer, *m_current_joint_ptr, &m_physics_world);
							/* when the joint is destoyed our copy here with the updated attributes remains intact*/
							m_utilities.createClusterObjectJoint(active_layer, *m_current_joint_ptr, &m_physics_world);
							m_current_joint_ptr = nullptr;
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Destory Joint")) {
						m_utilities.destroyClusterObjectJoint(active_layer, *m_current_joint_ptr, &m_physics_world);
						m_current_joint_ptr = nullptr;
					}
				}
				else {
					if (ImGui::Button("Create Joint")) {
						if (ccobjA.physics_properties != nullptr && ccobjB.physics_properties != nullptr) {
							m_utilities.setCurrentShapePointer(ccobjA.shape, &m_current_shape_ptr);
							if (m_current_shape_ptr->can_be_joint_memeber) {
								m_utilities.setCurrentShapePointer(ccobjB.shape, &m_current_shape_ptr);
								if (m_current_shape_ptr->can_be_joint_memeber) {
									m_utilities.createClusterObjectJoint(active_layer, *m_current_joint_ptr, &m_physics_world);
									m_vector_of_joint_ploting_points.clear();
									m_current_joint_ptr = nullptr;
								}
							}
						}
					}
				}
			}
			else {
				m_current_joint_ptr = nullptr;
			}
		}

		if (m_selection_mode_index == SelectModeEnum::WORLDCLUSTER) {
			//WorldCluster
			if (m_selected_cluster_objects.size() > 0) {
				ImGui::Text("World Cluster  Index {} ", layers_[active_layer_index_].world_clusters[m_selected_cluster_objects[0].first].index);


				ImGui::Text("World Cluster Name");
				ImGui::InputText("###SelectedClusterObjectsName", layers_[active_layer_index_].world_clusters[m_selected_cluster_objects[0].first].name, 20);

			}
		}
		else if (m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT && m_current_cluster_object_ptr != nullptr) {
			//ClusterObject
			if (editor_mode_ == EditorModeEnum::SELECT && m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
				if (ImGui::Button("Update"))
				{
					m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
					//we are trusting our program to always make the selected object the current object
					if (m_current_shape_ptr->is_plotted) {
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else
					{
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, layers_, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
				}

				if (ImGui::Button("Delete")) {
					auto& target_cluster_object = *m_current_cluster_object_ptr;
					std::pair<int, int> location_pair = std::pair<int, int>(target_cluster_object.cluster_index, target_cluster_object.index);
					std::pair<int, std::pair<int, int>> master_pair = std::pair<int, std::pair<int, int>>(target_cluster_object.layer_index, location_pair);
					m_cluster_objects_to_delete.push_back(master_pair);
				}
			}
		}
		if ((m_selection_mode_index == SelectModeEnum::WORLDCLUSTER || m_selection_mode_index == SelectModeEnum::FREESELECT || m_selection_mode_index == SelectModeEnum::JOINTSSELECT) && m_selected_cluster_objects.size() > 0) {
			ImGui::Text("%d Objects Selected ", m_selected_cluster_objects.size());

			if (ImGui::Button("Clear Selection")) {
				m_selected_cluster_objects.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Selected")) {
				for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
				{
					auto& target_cluster_object = layers_[active_layer_index_].world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
					std::pair<int, int> location_pair = std::pair<int, int>(target_cluster_object.cluster_index, target_cluster_object.index);
					std::pair<int, std::pair<int, int>> master_pair = std::pair<int, std::pair<int, int>>(target_cluster_object.layer_index, location_pair);
					m_cluster_objects_to_delete.push_back(master_pair);
				}
				if (m_selected_cluster_objects.size() == 2) {
					//joint Selection, safe guards us if a joint had already been created
					m_current_joint_ptr = nullptr;//nullfy the current joints data since we just deleted it
				}
				m_selected_cluster_objects.clear();
			}
			ImGui::SameLine();

			if (ImGui::Button("Hide Selected")) {
				for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& ccobj = layers_[active_layer_index_].world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
					ccobj.is_hidden = true;
				}
				m_selected_cluster_objects.clear();
			}

			//shifting selected to a different world cluster
			static int target_world_cluster_index = 0;
			ImGui::Separator();
			ImGui::DragInt("Target Index :", &target_world_cluster_index, 1, 0, layers_[active_layer_index_].world_clusters.size() - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (target_world_cluster_index < 0) {
				target_world_cluster_index = 0;
			}
			ImGui::Text("Target World Cluster:");
			ImGui::SameLine();
			ImGui::Text(layers_[active_layer_index_].world_clusters[target_world_cluster_index].name);
			if (ImGui::Button("Shift selected")) {
				m_is_all_work_saved = false;
				//make a copy of all the seleceted objects
				std::vector<std::pair<int, int>> copy_of_selected_objects = m_selected_cluster_objects;
				//clear out the original selected obejcts
				m_selected_cluster_objects.clear();
				//create copies of the previously selected objects but in new locations
				// in the target world cluters vector

				SquareBox::GWOM::ClusterObject* new_ccobj_ptr = nullptr;
				std::pair<int, int> got_shell_coordinates = std::pair<int, int>(-1, -1);

				for (unsigned int i = 0; i < copy_of_selected_objects.size(); i++)
				{
					if (i == 0) {
						//for the first one we will have to create the new objects template manually
						//create the first object mannually 

						//a copy not reference
						SquareBox::GWOM::ClusterObject copy_of_original_ccobj = layers_[active_layer_index_].world_clusters[copy_of_selected_objects[0].first].cluster_objects[copy_of_selected_objects[0].second];
						copy_of_original_ccobj.cluster_index = target_world_cluster_index;
						copy_of_original_ccobj.is_alive = false;

						//TODO :
						//joints are not copied , i will implement this later
						copy_of_original_ccobj.joints.clear();
						//change its index 
						copy_of_original_ccobj.index = layers_[active_layer_index_].world_clusters[target_world_cluster_index].cluster_objects.size();
						// rename it
						m_utilities.nameClusterObjectByIndex(copy_of_original_ccobj);

						//add the copy to the worldCLusters
						layers_[active_layer_index_].world_clusters[target_world_cluster_index].cluster_objects.push_back(copy_of_original_ccobj);

						// get a referenece to it 
						new_ccobj_ptr = &layers_[active_layer_index_].world_clusters[target_world_cluster_index].cluster_objects.back();

					}
					else {
						//depend on the shell we got 
						if (got_shell_coordinates.first >= 0 && got_shell_coordinates.second >= 0) {

							//copy properties
							layers_[active_layer_index_].world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second] = layers_[active_layer_index_].world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second];
							//point to our object
							new_ccobj_ptr = &layers_[active_layer_index_].world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second];
							//restore the cluster_index && index
							new_ccobj_ptr->cluster_index = got_shell_coordinates.first;
							new_ccobj_ptr->index = got_shell_coordinates.second;
							//restore the default name
							m_utilities.nameClusterObjectByIndex(*new_ccobj_ptr);
							new_ccobj_ptr->is_alive = false;
							//joints are not copied , i will implement this later
							new_ccobj_ptr->joints.clear();
						}
						else {

							SBX_ERROR("Invalid shell coordinates");
							__debugbreak();
						}
					}

					//The actual creatioin into the world starts here
					m_utilities.setCurrentShapePointer(new_ccobj_ptr->shape, &m_current_shape_ptr);
					//the world placement

					//the core should not create a shell after we create the last  cluster object
					bool create_shell = i < (copy_of_selected_objects.size() - 1);

					SquareBox::Utilities::creationDetails  details;
					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), new_ccobj_ptr, layers_, &m_physics_world, create_shell, false);
					}
					else {
						details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), new_ccobj_ptr, layers_, &m_physics_world, create_shell, false);
					}
					if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
						m_utilities.addPairToVector(m_selected_cluster_objects, details.settlementCoordinates);
					}
					else {
						SBX_CRITICAL("We failed to get back settelment coordinates when copying a world cluster ");
					}
					if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
						got_shell_coordinates = details.shellCoordinates;
					}
					else {
						got_shell_coordinates = std::pair<int, int>(-1, -1);
					}
				}
				//delete them from where ever they were before
				for (unsigned i = 0; i < copy_of_selected_objects.size(); i++)
				{
					auto& target_cluster_object = layers_[active_layer_index_].world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second];
					std::pair<int, int> location_pair = std::pair<int, int>(target_cluster_object.cluster_index, target_cluster_object.index);
					std::pair<int, std::pair<int, int>> master_pair = std::pair<int, std::pair<int, int>>(target_cluster_object.layer_index, location_pair);
					m_cluster_objects_to_delete.push_back(master_pair);
				}
			}
		}

	}
	else if (m_object_properties_enum == ObjectPropertiesEnum::ANIMATION) {
		if (m_current_cluster_object_ptr != nullptr) {
			bool atleast_one_animation_found = false;
			
			for each (auto & focus_animation in m_animation_creator.m_animations)
			{
				if (focus_animation->target_cluster_object_coordinates == std::pair<int, int>(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index)) {
					m_selected_animation = static_cast<int>(focus_animation->animation_type);
					ImGui::Spacing();
					//display our shapes names
					ImGui::Text("Animation     : "); ImGui::SameLine();
					ImGui::Text(ToString(focus_animation->animation_type));

					atleast_one_animation_found = true;
					/*
					
					  this animation part still needs alot of editing

					  i have to add appropriate buttons for editing, deleting and adding new animations

					  ability to save the animations currently in the animation vec to the animation  
					  script so that they can be loaded in next time
					
					*/

					for (unsigned int i = 0; i < m_vec_animations_pointer.size(); i++)
					{
						if (m_selected_animation == i) {

							if (m_vec_animations_pointer[m_selected_animation]->animation_type == SquareBox::AnimationTypeEnum::forceDependant) {
								for (unsigned int j = 0; j < focus_animation->vec_of_animation_Squence.size(); j++)
								{
									SquareBox::AnimationSystem::AnimationSpecifications& focus_animation_squence = focus_animation->vec_of_animation_Squence[j];
									if (ImGui::TreeNode(ToString(focus_animation_squence.motionState))) {

										ImGui::InputFloat("Animation Speed			   :  ", &focus_animation_squence.animationSpeed, 0.1, 1.0f);
										ImGui::Checkbox("Is Forward Movement Oriented  :  ", &focus_animation_squence.isForwardMovementOrientied);
										ImGui::Checkbox("Is Reverse Movement Oriented  :  ", &focus_animation_squence.isReversedMovementOrientied);
										ImGui::Checkbox("Is Ground Dependant           :  ", &focus_animation_squence.isGroundDependant);
										ImGui::Checkbox("Linked to Vertical Velocity   :  ", &focus_animation_squence.isAnimationSpeedLinkedToVerticalVelocity);
										ImGui::Checkbox("Linked to Horizontal Velocity :  ", &focus_animation_squence.isAnimationSpeedLinkedToHorizontalVelocity);
										ImGui::Checkbox("Force by Mass                 :  ", &focus_animation_squence.applyForceAccordingToMass);
										ImGui::Checkbox("is Position Editing           :  ", &focus_animation_squence.isPositionEditing);
										ImGui::InputFloat("Gain in X        		   :  ", &focus_animation_squence.gainInX, 0.1, 1.0f);
										ImGui::InputFloat("Gain in Y        		   :  ", &focus_animation_squence.gainInY, 0.1, 1.0f);
										ImGui::InputInt("Number of Tiles               :  ", &focus_animation_squence.numTiles);
										ImGui::InputInt("Start Tiles	               :  ", &focus_animation_squence.startTile);
										/*
										0 - onTap
										1 - onLongPress
										2 - isBeingPressed
										i will give this an enum class when i get time
										*/
										ImGui::InputInt("Key Activation Squence        :  ", &focus_animation_squence.keyActivationSquence);
										ImGui::InputInt("Key Code                      :  ", &focus_animation_squence.keycode);
										ImGui::InputInt("Duration                      :  ", &focus_animation_squence.duration);
										ImGui::InputFloat("Animation Force  X 		   :  ", &focus_animation_squence.animationForce.x, 0.1, 1.0f);
										ImGui::InputFloat("Animation Force  Y 		   :  ", &focus_animation_squence.animationForce.y, 0.1, 1.0f);
										
										ImGui::TreePop();
									}
								}
							}
							else if (m_vec_animations_pointer[m_selected_animation]->animation_type == SquareBox::AnimationTypeEnum::timeDependant) {
								for (unsigned int j = 0; j < focus_animation->vec_of_animation_Squence.size(); j++)
								{
									SquareBox::AnimationSystem::AnimationSpecifications& focus_animation_squence = focus_animation->vec_of_animation_Squence[j];
									if (ImGui::TreeNode(ToString(focus_animation_squence.motionState))) {
										ImGui::InputFloat("Animation Speed			   :  ", &focus_animation_squence.animationSpeed, 0.1, 1.0f);
										ImGui::InputInt("Number of Tiles               :  ", &focus_animation_squence.numTiles);
										ImGui::InputInt("Start Tiles	               :  ", &focus_animation_squence.startTile);
										/*
										0 - onTap
										1 - onLongPress
										2 - isBeingPressed
										i will give this an enum class when i get time
										*/
										ImGui::InputInt("Key Activation Squence        :  ", &focus_animation_squence.keyActivationSquence);
										ImGui::InputInt("Key Code                      :  ", &focus_animation_squence.keycode);
										ImGui::InputInt("Duration                      :  ", &focus_animation_squence.duration);
										
										// the animations duration can not be less than one
										if (focus_animation_squence.duration<1) {
											focus_animation_squence.duration = 1;
										}

										ImGui::TreePop();
									}
								}
							}
							else if (m_vec_animations_pointer[m_selected_animation]->animation_type == SquareBox::AnimationTypeEnum::propertiesDependant) {
								for (unsigned int j = 0; j < focus_animation->vec_of_animation_Squence.size(); j++)
								{
									SquareBox::AnimationSystem::AnimationSpecifications& focus_animation_squence = focus_animation->vec_of_animation_Squence[j];
									if (ImGui::TreeNode(ToString(focus_animation_squence.motionState))) {

										ImGui::InputFloat("Animation Speed			   :  ", &focus_animation_squence.animationSpeed, 0.1, 1.0f);
										ImGui::Checkbox("Is Forward Movement Oriented  :  ", &focus_animation_squence.isForwardMovementOrientied);
										ImGui::Checkbox("Is Reverse Movement Oriented  :  ", &focus_animation_squence.isReversedMovementOrientied);
										ImGui::Checkbox("Is Ground Dependant           :  ", &focus_animation_squence.isGroundDependant);
										ImGui::Checkbox("Linked to Vertical Velocity   :  ", &focus_animation_squence.isAnimationSpeedLinkedToVerticalVelocity);
										ImGui::Checkbox("Linked to Horizontal Velocity :  ", &focus_animation_squence.isAnimationSpeedLinkedToHorizontalVelocity);
										ImGui::Checkbox("Force by Mass                 :  ", &focus_animation_squence.applyForceAccordingToMass);
										ImGui::Checkbox("is Position Editing           :  ", &focus_animation_squence.isPositionEditing);
										ImGui::InputFloat("Gain in X        		   :  ", &focus_animation_squence.gainInX, 0.1, 1.0f);
										ImGui::InputFloat("Gain in Y        		   :  ", &focus_animation_squence.gainInY, 0.1, 1.0f);
										ImGui::InputInt("Number of Tiles               :  ", &focus_animation_squence.numTiles);
										ImGui::InputInt("Start Tiles	               :  ", &focus_animation_squence.startTile);
										/*
										0 - onTap
										1 - onLongPress
										2 - isBeingPressed
										i will give this an enum class when i get time
										*/
										ImGui::InputInt("Key Activation Squence        :  ", &focus_animation_squence.keyActivationSquence);
										ImGui::InputInt("Key Code                      :  ", &focus_animation_squence.keycode);
										ImGui::InputInt("Duration                      :  ", &focus_animation_squence.duration);
										ImGui::InputFloat("Animation Force  X 		   :  ", &focus_animation_squence.animationForce.x, 0.1, 1.0f);
										ImGui::InputFloat("Animation Force  Y 		   :  ", &focus_animation_squence.animationForce.y, 0.1, 1.0f);
										ImGui::TreePop();
									}
								}
							}
						}
					}


				}
			}
		    
			if (editor_mode_==EditorModeEnum::SELECT) {
				//can only add animations to objects that are already inside alive_cluster_objects_array
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("NEW ANIMATION");
				ImGui::Spacing();
				ImGui::Spacing();

				m_temp_limbo_animation.target_cluster_object = std::string(m_current_cluster_object_ptr->name);


				ImGui::Text("Animation     : "); ImGui::SameLine();

				for (unsigned int i = 0; i < m_vec_animations_pointer.size(); i++)
				{
					animations_labels_ptr[i] = ToString(m_vec_animations_pointer[i]->animation_type);
				}


				ImGui::Combo("###TempAnimation", &m_temp_limbo_selected_animation, animations_labels_ptr, m_vec_animations_pointer.size());
				m_temp_limbo_animation.animation_type = m_vec_animations_pointer[m_temp_limbo_selected_animation]->animation_type;

			
				ImGui::Text("Motion State  : "); ImGui::SameLine();
				/* motion states are added during the editing phase*/


				for (unsigned int i = 0; i < m_vec_of_animation_motion_states.size(); i++)
				{
					animation_motion_states_labels_ptr[i] = ToString(m_vec_of_animation_motion_states[i]);
				}

				m_temp_limbo_selected_animation_motion_state = static_cast<int>(m_temp_limbo_animation.animation_specifications.back().motionState);
				ImGui::Combo("###AnimationMotionState", &m_temp_limbo_selected_animation_motion_state, animation_motion_states_labels_ptr, m_vec_of_animation_motion_states.size());
				m_temp_limbo_animation.animation_specifications.back().motionState = m_vec_of_animation_motion_states[m_temp_limbo_selected_animation_motion_state];
				
				if (ImGui::Button("Add")) {
					m_animation_creator.addNewLimboAnimation(m_temp_limbo_animation);
				}
			}
		}
	}
}

void FlatLayer::onDispose()
{
	m_physics_world.destroy();
	m_utilities.dispose();
}

void FlatLayer::onLayerDispose(SquareBox::GWOM::Layer& layer_)
{
	for (unsigned int i = 0; i < layer_.alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject* cwobj = &layer_.world_clusters[layer_.alive_cluster_objects[i].first].cluster_objects[layer_.alive_cluster_objects[i].second];
		if (cwobj->physics_properties != nullptr) {
			cwobj->physics_properties->dispose();
			delete cwobj->physics_properties; //we should always free up this memory with delete
			cwobj->physics_properties = nullptr;
		}
	}
	layer_.alive_cluster_objects.clear();
	layer_.world_clusters.clear();
	layer_.active_joints_body_a_map.clear();
	m_selected_cluster_objects.clear();

	onLayerInit(layer_);
	/*
		need to call init layer again so that we have atleast one world cluster and object
	*/
}
