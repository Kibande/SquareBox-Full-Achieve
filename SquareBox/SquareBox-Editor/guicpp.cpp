//#include "LevelEditor_Screen.h"
//#include "ScreenIndices.h"
//#include<utility>
//
//LevelEditor_Screen::LevelEditor_Screen(SquareBox::RenderEngine::Window* window)
//	:m_window(window)
//{
//}
//
//void LevelEditor_Screen::build()
//{
//	SBX_INFO("LevelEditor Screen Loaded into memory");
//}
//
//void LevelEditor_Screen::onEntry() {
//	m_sprite_font.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);
//	m_utilities.init();
//	m_sprite_batch.init();
//	m_debug_renderer.init();
//	m_physics_world.init();
//	m_automation.init();
//	m_animation_creator.init();
//	if (m_animation_creator.loadAnimationScript(m_animation_script)) {
//		SBX_INFO("Managed To load Animation Scipts");
//	}
//	else {
//		SBX_WARN("Failed To load Animation Scipts");
//	}
//	if (m_automation.loadAutomationScript(m_automation_script)) {
//		SBX_INFO("Managed To load Automation Scipts");
//	}
//	else {
//		SBX_WARN("Failed To load Automation Scipts");
//	}
//
//	//Texture Program
//	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
//	m_texture_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
//	m_texture_program.addAttribute("vertexColor");
//	m_texture_program.addAttribute("vertexUV");
//	m_texture_program.linkShaders();
//
//	//Complie our light shader
//	m_light_program.compileShaders("Assets/Shaders/lightShading.vert", "Assets/Shaders/lightShading.frag");
//	m_light_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
//	m_light_program.addAttribute("vertexColor");
//	m_light_program.addAttribute("vertexUV");
//	m_light_program.linkShaders();
//
//	//the debugProgram
//	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
//	m_debug_program.addAttribute("vertexPosition");
//	m_debug_program.addAttribute("vertexColor");
//	m_debug_program.linkShaders();
//
//	m_k_contstant_for_scale_and_zoom = m_camera_paning_speed * m_camera_zooming_speed;
//
//	//Clear out the world
//	m_world_clusters.clear();
//
//	//Wil reserve a few world clusters here to make stuff faster
//	m_world_clusters.reserve(60);
//
//	//first WorldCluster
//	m_world_clusters.emplace_back();
//	m_current_world_cluster_ptr = &m_world_clusters.back();
//	m_current_world_cluster_ptr->index = m_world_clusters.size() - 1;
//	m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);
//
//	//First Cluster object
//	m_current_world_cluster_ptr->cluster_objects.emplace_back();
//	m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects.back();
//	m_current_cluster_object_ptr->index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
//	m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
//	m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//
//	//The shapes we can create and edit in the world
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::BoxShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CircleShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PolygonShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::EdgeShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::ChainShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CapsuleShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::TopCapsuleShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PointLightShape());
//	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CalculatedLightShape());
//
//	//intiliase camera
//	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
//	m_camera.setScale(m_initial_camera_scale);
//	m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2));//Center the camera
//
//	updateStates();
//
//	initGUI();
//}
//
//void LevelEditor_Screen::update(const float & deltaTime_)
//{
//	//update all camera
//	m_camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
//
//	ImGuiIO& io = ImGui::GetIO();
//	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
//	{
//		SDL_Event evnt;
//		//stop our game engine from processing input
//		m_game_ptr->setProcessingInput(false);
//		while (SDL_PollEvent(&evnt))
//		{
//			//hand over all the events to imGUI
//			ImGui_ImplSDL2_ProcessEvent(&evnt);
//		}
//	}
//	else {
//		m_game_ptr->setProcessingInput(true);
//	}
//
//	/*
//	 these operations are only carried out when we are processing input ,(The mouse is in our world space)
//	 please take note of that
//	 If an operation does not depend on us processing input then is should join its similar friends below this if
//	 //statement
//	*/
//	if (m_game_ptr->isProcessingInput()) {
//		//Calculated Movement Speed
//		float cMS = (m_k_contstant_for_scale_and_zoom / m_camera.getScale());
//
//		//zooming and moving the camera in our world
//		m_editor_assitant.cameraMovement(m_camera, cMS, m_game_ptr);
//
//		//where the mouse currently is at in our world
//		glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
//
//		if (m_select_mode) {
//			bool hoveringOverCluster = false;
//			bool hoveringOverClusterObject = false;
//			for (size_t i = 0; i < m_alive_cluster_objects.size(); i++)
//			{
//				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//				m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
//				if (m_current_shape_ptr->containtsPoint(ccobj, mouse_in_world)) {
//					hoveringOverCluster = true;
//					hoveringOverClusterObject = true;
//					m_current_hovered_world_cluster_index = m_alive_cluster_objects[i].first;
//					m_current_hovered_cluster_object_index = m_alive_cluster_objects[i].second;
//				}
//			}
//
//			//the actual cluster object selection
//			/*
//				0 - Cluster Select, used to delete many related objects at once(a fullCluster)
//				1 - Object Select, used to edit an indivdual cluster objects properties
//				2 - Joints Select,used to create joints
//			*/
//
//			if (m_selection_mode_index == 0) {
//				//World cluster selection
//				//if we are hovering over a cluster object and we Left click let that objects' whole cluster become selected in our world scene
//				if (hoveringOverClusterObject && m_game_ptr->getInputDevice()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
//					m_selected_cluster_objects.clear();
//					//loop through our alive Objects and select this HoveredWorld Cluster
//					for (size_t i = 0; i < m_alive_cluster_objects.size(); i++)
//					{
//						if (m_alive_cluster_objects[i].first == m_current_hovered_world_cluster_index) {
//							m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_alive_cluster_objects[i].first, m_alive_cluster_objects[i].second));
//						}
//					}
//					/*
//						keep track of where the mouse was in the world cluster when we selected this world
//						cluster , we shall use this while dragging the world cluster later
//					*/
//					clustermousepos = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
//				}
//			}
//			else if (m_selection_mode_index == 1) {
//				//Cluster object Selection
//				//this is the only place where we actually set the active worldClusters and Objects
//				//if we are hovering over a cluster object and we Left click let that object become the current active object in our world scene
//				if (hoveringOverClusterObject && m_game_ptr->getInputDevice()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
//					m_current_world_cluster_ptr = &m_world_clusters[m_current_hovered_world_cluster_index];
//					m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects[m_current_hovered_cluster_object_index];
//					updateStates();//this updates the imGui variables
//					m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
//					m_selected_cluster_objects.clear();
//					m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index));
//
//					//check if we just clicked on one of chains or edge ends , if so record this progress
//					if (m_current_shape_ptr->is_selected_through_ends) {
//						if (m_current_cluster_object_ptr->is_first_hovered) {
//							m_current_cluster_object_ptr->is_first_hovered = false;
//							m_current_cluster_object_ptr->is_last_hovered = false;
//
//							m_current_cluster_object_ptr->is_first_selected = true;
//							m_current_cluster_object_ptr->is_last_selected = false;
//						}
//						else if (m_current_cluster_object_ptr->is_last_hovered) {
//							m_current_cluster_object_ptr->is_first_hovered = false;
//							m_current_cluster_object_ptr->is_last_hovered = false;
//
//							m_current_cluster_object_ptr->is_first_selected = false;
//							m_current_cluster_object_ptr->is_last_selected = true;
//						}
//					}
//				}
//
//				/* Duplicate  the current Cluster Object if needed */
//				std::pair<int, int> m_currentClusterObject_coordinates = m_editor_assitant.currentClusterObjectDupper(m_physics_world, m_world_clusters, m_alive_cluster_objects, m_selected_cluster_objects, m_current_cluster_object_ptr, m_current_shape_ptr, m_game_ptr, m_utilities);
//				if (m_currentClusterObject_coordinates.first != -1 && m_currentClusterObject_coordinates.second != -1) {
//					m_current_cluster_object_ptr = &m_world_clusters[m_currentClusterObject_coordinates.first].cluster_objects[m_currentClusterObject_coordinates.second];
//				}
//			}
//			else if (m_selection_mode_index == 2) {
//				//Joint Creation
//				//only two objects can be selected at once
//				//and only one joint can exit between two bodies
//				if (hoveringOverClusterObject && m_game_ptr->getInputDevice()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
//					//clicked on object
//					SquareBox::GWOM::ClusterObject & clicked_on_object = m_world_clusters[m_current_hovered_world_cluster_index].cluster_objects[m_current_hovered_cluster_object_index];
//					if (clicked_on_object.physics_properties != nullptr) {
//						//only physics Objects can be part of joints
//						if (m_selected_cluster_objects.size() == 2) {
//							//remove the top most cluster Object Coordinates so that we maintain only two selected cluster Objects
//							m_selected_cluster_objects.erase(m_selected_cluster_objects.begin());
//						}
//						if (m_selected_cluster_objects.size() > 2) {
//							SBX_ERROR("m_selectedClusterObjects size is greater than 2, for SelectionMode joint ");
//						}
//						m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_current_hovered_world_cluster_index, m_current_hovered_cluster_object_index), false);
//					}
//				}
//			}
//
//			//dragging implemetation
//			if (m_selection_mode_index == 0) {
//				//World Cluster Mode
//				if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::RIGHT_CLICK))
//				{
//					float diplacementinX = mouse_in_world.x - clustermousepos.x;
//					float diplacementinY = mouse_in_world.y - clustermousepos.y;
//					for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
//					{
//						SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
//						m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
//						updateStates();/*having this here is irrelvant because we dont really call to the varibales in this time
//									   frim at all ,maybe after when we have finished this loop */
//
//									   //we should do the proper displacement at this point
//
//						if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
//							for (unsigned int j = 0; j < ccobj.vertices.size(); j++)
//							{
//								ccobj.vertices[j].x += diplacementinX;
//								ccobj.vertices[j].y += diplacementinY;
//							}
//							//need to do the logic for displacing edges,chains and ploygons and bridges since
//							//we want to displace the vertices basinf on the mouses motion
//							m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
//						}
//						else
//						{
//							ccobj.position.x += diplacementinX;
//							ccobj.position.y += diplacementinY;
//							m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
//						}
//					}
//					clustermousepos = mouse_in_world;
//					m_current_shape_ptr = nullptr;
//				}
//			}
//			else if (m_selection_mode_index == 1 && m_current_cluster_object_ptr != nullptr) {
//				//ClusterObject Mode
//				if (m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
//					if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::RIGHT_CLICK))
//					{
//						//this should be okay to remove
//						m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
//
//						//we are trusting our program to always make the selected object the current object ,which it does perfectly
//						if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
//							//need to do the logic for displacing edges,chains and ploygons and bridges since
//							//we want to displace the vertices basinf on the mouses motion
//							m_utilities.createClusterObjectIntoWorld(true, true, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//						}
//						else
//						{
//							m_utilities.createClusterObjectIntoWorld(false, true, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//						}
//					}
//				}
//			}
//			else if (m_selection_mode_index == 2) {
//				/*
//					Joints dragging is handled when the individual cluster Objects are dragged
//				*/
//			}
//		}
//		else if (m_place_mode) {
//			if (m_game_ptr->getInputDevice()->isInputIdReceived((int)SquareBox::MouseEnum::RIGHT_CLICK)) {
//				//place plotting points for the ploted shapes , and draw the other objects
//				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_auto_assign_world_cluster ? m_current_active_world_cluster_index : m_mannually_choosen_world_cluster_index, &m_physics_world, true, m_respect_ancestor);
//				//add to m_alive_cluster_objects Objects if its alive
//				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//					if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//						m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//					}
//					else {
//						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//					}
//				}
//				//get a pointer to the shell
//				if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
//					m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
//					m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
//					m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
//					/*
//						i think that this check here is totally irrelevant , nothing is uncheckinh the Physics Attribute
//					*/
//					if (m_respect_ancestor) {
//						checkPhysicsStatus();
//					}
//				}
//			}
//		}
//	}
//	else {
//		//when imgui is the one doing the processing
//		//keeping up with the physics
//
//		checkPhysicsStatus();//to keep track when we toggle an objects Physics abilities
//
//		if (m_place_mode) {
//			if (m_draw_plotted_shape) {
//				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_auto_assign_world_cluster ? m_current_active_world_cluster_index : m_mannually_choosen_world_cluster_index, &m_physics_world, true, m_respect_ancestor);
//				//add to m_alive_cluster_objects Objects if its alive
//				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//					if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//						m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//					}
//					else {
//						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//					}
//				}
//				//get a pointer to the shell
//				if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
//					m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
//					m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
//					m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
//					if (m_respect_ancestor) {
//						checkPhysicsStatus();
//					}
//				}
//				else {
//					SBX_ERROR("Missing shell!!");
//					__debugbreak();
//				}
//
//				m_draw_plotted_shape = false;
//			}
//		}
//	}
//
//	/*these operations are carried out regardless of what is doing the processing , game or imgui*/
//
//	//keeping track of the clusterObjects positions in the world
//	//important because we have physics engine that edits poistions and angles
//	SquareBox::IShape * orignalShapePointer = m_current_shape_ptr;
//	for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//	{
//		SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//		m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
//		m_current_shape_ptr->keepTrackOfPosition(ccobj);//Must be run before our automation.update
//
//		//recreate objects that requested recreation, mainly through the SquareBox::AI
//		if (ccobj.requesting_recreation) {
//			/* Requested recreation does on take into account the current mouse position */
//			if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
//				m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
//			}
//			else {
//				m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
//			}
//			ccobj.requesting_recreation = false;
//		}
//	}
//	m_current_shape_ptr = orignalShapePointer;
//
//	/*
//		When the delete Button is clicked on , all the selected Objects get deleted
//	*/
//	bool deleted_a_cluster_object = false;
//	// deleting objects
//	for (std::vector<std::pair<int, int>>::iterator it = m_alive_cluster_objects.begin(); it != m_alive_cluster_objects.end();)
//	{
//		bool erased = false;
//		SquareBox::GWOM::ClusterObject & ccobj = m_world_clusters[(*it).first].cluster_objects[(*it).second];
//		//Destory those that have expired
//		if (ccobj.life_span < 1) {
//			SBX_INFO("Deleting Object");
//
//			for (unsigned int j = 0; j < ccobj.joints.size(); j++)
//			{
//				SquareBox::GWOM::Joint* joint = ccobj.joints[j];
//				SquareBox::GWOM::ClusterObject& jointBodyA = m_world_clusters[joint->body_a_coordinates.first].cluster_objects[joint->body_a_coordinates.second];
//				SquareBox::GWOM::ClusterObject& jointBodyB = m_world_clusters[joint->body_b_coordinates.first].cluster_objects[joint->body_b_coordinates.second];
//				m_utilities.destroyClusterObjectJoint(jointBodyA, jointBodyB);
//			}
//
//			it = m_alive_cluster_objects.erase(it);
//			erased = true;
//			deleted_a_cluster_object = true;
//			ccobj.is_alive = false;
//			if (ccobj.physics_properties != nullptr) {
//				ccobj.physics_properties->dispose();
//				ccobj.physics_properties = nullptr;
//			}
//
//			//if (m_current_cluster_object_ptr != nullptr) {
//			//	// this is not a usecase i think that i shall be seeing in the editor, because
//			//	// we , atleast at this current point in time , neva delete the object referenced ,but wait , when does our
//			//	//m_current_cluster_object_ptr be null as we are deleting ?, this is something i have to explain later
//			//	if (ccobj.cluster_index == m_current_cluster_object_ptr->cluster_index && ccobj.index == m_current_cluster_object_ptr->index) {
//			//	}
//			//}
//			m_current_cluster_object_ptr = nullptr;
//			SBX_INFO("Finshed Deleting ");
//		}
//		if (!erased) {
//			++it;
//		}
//	}
//
//	if (deleted_a_cluster_object) {
//		m_selected_cluster_objects.clear();
//	}
//
//	//run update loop for our world objects
//	if (m_play_simulation) {
//		m_physics_world.update();//update our physics
//		m_automation.update(m_world_clusters, m_alive_cluster_objects, m_game_ptr->getFps(), m_game_ptr->getGameLoopElapsedSeconds());
//		m_animation_creator.update(1.0f, m_world_clusters, m_alive_cluster_objects, m_game_ptr->getFps(), m_game_ptr->getGameLoopElapsedSeconds(), m_game_ptr->getInputDevice());
//	}
//
//	if (m_physics_world.isLocked()) {
//		m_show_world_locked_dialog = true;
//	}
//	else {
//		m_show_world_locked_dialog = false;
//	}
//}
//
//void LevelEditor_Screen::draw() {
//	m_debug_renderer.begin();
//
//	//drawing pulley joints ropes
//	for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//	{
//		SquareBox::GWOM::ClusterObject & cwobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//		if (cwobj.joints.size() > 0) {
//			for (unsigned int j = 0; j < cwobj.joints.size(); j++)
//			{
//				if (cwobj.joints[j]->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
//					//draw the pulley ropes
//
//					SquareBox::GWOM::ClusterObject & jointObjectA = m_world_clusters[cwobj.joints[j]->body_a_coordinates.first].cluster_objects[cwobj.joints[j]->body_a_coordinates.second];
//					SquareBox::GWOM::ClusterObject & jointObjectB = m_world_clusters[cwobj.joints[j]->body_b_coordinates.first].cluster_objects[cwobj.joints[j]->body_b_coordinates.second];
//
//					//check if we are dealing with physics bodies , which should always be the case for joints
//					if (jointObjectA.physics_properties != nullptr && jointObjectB.physics_properties != nullptr && cwobj.joints[j]->vec_of_points.size() == 2) {
//						m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
//						m_debug_renderer.drawLine(cwobj.joints[j]->vec_of_points[0], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectA, cwobj.joints[j]->local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.joints[j]->color.x), (255 * cwobj.joints[j]->color.y), (255 * cwobj.joints[j]->color.z), (255 * cwobj.joints[j]->color.w))));
//						m_debug_renderer.drawLine(cwobj.joints[j]->vec_of_points[1], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectB, cwobj.joints[j]->local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.joints[j]->color.x), (255 * cwobj.joints[j]->color.y), (255 * cwobj.joints[j]->color.z), (255 * cwobj.joints[j]->color.w))));
//					}
//					else {
//						SBX_ERROR("Error while trying to draw pulley Joint ropes");
//					}
//				}
//			}
//		}
//	}
//	m_debug_renderer.end();
//
//	m_debug_program.use();
//	preUpdateCamera(&m_debug_program, &m_camera, "P");
//	m_debug_renderer.render();
//	m_debug_program.unuse();
//
//	m_sprite_batch.begin(SquareBox::RenderEngine::NONE);//helps us respect the order we already have predifined in the m_alive_cluster_objects class
//
//	for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//	{
//		SquareBox::GWOM::ClusterObject & cwobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
//		//lights are drawn using the light later
//		if (m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::PointLightShape && m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::CalculatedLightShape) {
//			/*
//				we should only be drawing the clusterObjects that are in the cameras view
//				where we can, these will safe resources
//			*/
//
//			if (m_current_shape_ptr->is_plotted) {
//				//we be drawing all plotted shapes always
//				m_current_shape_ptr->draw(cwobj, m_sprite_batch);
//			}
//			else if (m_current_shape_ptr->needs_height_width) {
//				if (m_camera.isBoxInView(cwobj.position, glm::vec2(cwobj.width, cwobj.height))) {
//					m_current_shape_ptr->draw(cwobj, m_sprite_batch);
//				}
//			}
//			else if (m_current_shape_ptr->needs_radius) {
//				if (m_camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
//					m_current_shape_ptr->draw(cwobj, m_sprite_batch);
//				}
//			}
//			else
//			{
//				SBX_ERROR("Unidentified shape found for {} ", cwobj.name);
//			}
//		}
//	}
//
//	m_sprite_batch.end();
//	//the textures
//	m_texture_program.use();
//	preUpdateShader(&m_texture_program, "mySampler");
//	preUpdateCamera(&m_texture_program, &m_camera, "P");
//	m_sprite_batch.renderBatch();
//	m_texture_program.unuse();
//
//	m_debug_renderer.begin();//for the rest of the debug lines
//	if (m_debug_mode) {
//		// Draw axis
//		// +X axis
//		int screenWidth = m_window->getScreenWidth();
//		int screenHeight = m_window->getScreenHeight();
//		glm::vec2 debug_center;
//		debug_center = m_camera.getPosition();
//		/*Feature*/
//		//debug_center=glm::vec2(screenWidth*0.5, screenHeight*0.5);
//
//		float camera_scale = m_camera.getScale();
//
//		float width_top_up = 0;
//		float height_top_up = 0;
//		if (camera_scale < 1.0f) {
//			width_top_up = screenWidth / camera_scale;
//			height_top_up = screenHeight / camera_scale;
//		}
//		//+X
//		m_debug_renderer.drawLine(debug_center, glm::vec2((screenWidth*0.5) + width_top_up + screenWidth * 0.5, screenHeight*0.5), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 200));
//
//		// -X axis
//		m_debug_renderer.drawLine(debug_center, glm::vec2(-width_top_up - screenWidth * 0.5, debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 100));
//
//		// +Y axis
//		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y + height_top_up + screenHeight * 0.5), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 200));
//		// -Y axis
//		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, -height_top_up - screenHeight * 0.5), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 100));
//
//		//draw the rest of the m_items
//		for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//		{
//			SquareBox::GWOM::ClusterObject & cwobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//			if (cwobj.physics_properties == nullptr || !cwobj.physics_properties->isIntialised()) {
//				m_border_color = m_not_physics_body_color;
//			}
//			else if (cwobj.physics_properties->isDynamic()) {
//				m_border_color = m_debug_dynamic_body_color;
//			}
//			else if (cwobj.physics_properties->isStatic()) {
//				m_border_color = m_debug_static_body_color;
//			}
//			else if (cwobj.physics_properties->isKinematic()) {
//				m_border_color = m_debug_kinetic_body_color;
//			}
//			else {
//				m_border_color = m_not_physics_body_color;
//			}
//
//			//are we among the selected Objects
//			for (unsigned int k = 0; k < m_selected_cluster_objects.size(); k++)
//			{
//				std::pair<int, int> me = std::pair(cwobj.cluster_index, cwobj.index);
//				if (me == m_selected_cluster_objects[k]) {
//					m_border_color = m_selected_body_color;
//					break;
//				}
//			}
//
//			//show debug edges
//			m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
//			m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, m_border_color);
//			//show the first and last vertices
//			if (m_current_shape_ptr->is_plotted) {
//				m_utilities.showPlottedVertices(true, m_debug_renderer, cwobj, 1.0f / m_camera.getScale());
//			}
//		}
//	}
//
//	glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
//
//	//draw the place mode sketch
//	if (m_place_mode) {
//		m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
//		m_current_shape_ptr->traceDraw(*m_current_cluster_object_ptr, m_debug_renderer, mouse_in_world, m_not_physics_body_color, m_camera.getScale());
//	}
//
//	//we are drawing a boundary around the currently selected object
//	else if (m_select_mode) {
//
//		//for Joint Creation selection Mode
//		//ploting the plotted points
//		for (unsigned int i = 0; i < m_vector_of_joint_ploting_points.size(); i++)
//		{
//			m_debug_renderer.drawCircle(m_vector_of_joint_ploting_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_camera.getScale());
//		}
//
//		if (m_current_joint_ptr != nullptr) {
//			SquareBox::GWOM::ClusterObject & jointBodyA = m_world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second];
//			SquareBox::GWOM::ClusterObject & jointBodyB = m_world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second];
//
//			m_utilities.setCurrentShapePointer(jointBodyA.shape, &m_current_shape_ptr);
//			m_debug_renderer.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyA, m_current_joint_ptr->local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_camera.getScale());
//
//			m_utilities.setCurrentShapePointer(jointBodyB.shape, &m_current_shape_ptr);
//			m_debug_renderer.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyB, m_current_joint_ptr->local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_camera.getScale());
//
//			if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
//				for (unsigned int i = 0; i < m_current_joint_ptr->vec_of_points.size(); i++)
//				{
//					m_debug_renderer.drawCircle(m_current_joint_ptr->vec_of_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_camera.getScale());
//				}
//			}
//		}
//	}
//	m_debug_renderer.end();
//
//	//render all our debug shapes
//	m_debug_program.use();
//	preUpdateCamera(&m_debug_program, &m_camera, "P");
//	m_debug_renderer.render();
//	m_debug_program.unuse();
//
//	//the light renderer
//	//
//	//drawing the lights
//	m_window->useAdditiveBlending();
//
//	m_sprite_batch.begin();
//	for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//	{
//		SquareBox::GWOM::ClusterObject & cwobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//
//		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
//
//		if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::PointLightShape) {
//			if (m_current_shape_ptr->needs_radius) {
//				if (m_camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
//					m_current_shape_ptr->draw(cwobj, m_sprite_batch);
//				}
//			}
//		}
//		else if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::CalculatedLightShape) {
//			m_current_shape_ptr->draw(cwobj, m_sprite_batch);
//		}
//	}
//	m_sprite_batch.end();
//
//	m_light_program.use();
//	preUpdateCamera(&m_light_program, &m_camera, "P");
//	m_sprite_batch.renderBatch();
//	m_light_program.unuse();
//
//	m_window->useAlphaBlending();
//	drawGUI();
//}
//
//void LevelEditor_Screen::onExit()
//{
//	if (!m_allow_exit)
//	{
//		m_game_ptr->abortExit();//abort normal exit and first see request for permission to exit
//		m_request_exit = true;//this will lead to the are u sure dialog being displayed
//	}
//	else {
//		//this line must be called by the programmer inorder to permit the game to exit
//		// for  a game to exit the programmer must have allowed it to
//		m_sprite_font.dispose();
//		m_sprite_batch.dispose();
//		m_utilities.dispose();
//		m_texture_program.dispose();
//		//m_automation.dispose();
//		//m_animatioCreator.dispose();
//		for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//		{
//			SquareBox::GWOM::ClusterObject *cwobj = &m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//			if (cwobj->physics_properties != nullptr) {
//				cwobj->physics_properties->dispose();
//				delete cwobj->physics_properties; //we should always free up this memory with delete
//				cwobj->physics_properties = nullptr;
//			}
//		}
//		//m_world_clusters.clear();
//		/*
//		calling clear here is causing a memory leak because this is infact a vector of heap memories
//		the solution will be to make the m_worldCluster vector a vector of unique_pointers which will free
//		them selves automatically
//		*/
//		/*	for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
//			{
//				delete m_vec_shapes_pointer[i];
//			}*/
//		m_physics_world.destroy();
//		m_game_ptr->initiateExit();
//	}
//}
//
//void LevelEditor_Screen::destroy()
//{
//	SBX_INFO("LevelEditor Screen Cleared out of memory");
//}
//
//int LevelEditor_Screen::getNextScreenIndex() const
//{
//	return SCREEN_INDEX_NO_SCREEN;
//}
//
//int LevelEditor_Screen::getPreviousScreenIndex() const
//{
//	return WELCOME_SCREEN_INDEX;
//}
//
//char *convertGLTextureDisplayNameToCstr(const SquareBox::AssetManager::GLTexture & glTexture_)
//{
//	char *pc = new char[glTexture_.display_name.size() + 1];
//	std::strcpy(pc, glTexture_.display_name.c_str());
//	return pc;
//}
//
//void LevelEditor_Screen::initGUI()
//{
//#ifdef __APPLE__
//	// GL 3.2 Core + GLSL 150
//	const char* glsl_version = "#version 150";
//#else
//	// GL 3.0 + GLSL 130
//	const char* glsl_version = "#version 130";
//#endif
//
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGuiContext* IMGC = ImGui::CreateContext();
//	ImGuiIO io = ImGui::GetIO(); (void)io;
//	ImGui::SetCurrentContext(IMGC);
//
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // enable Keyboard Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // enable Gamepad Controls
//
//	// Setup Platform/Renderer backends
//	ImGui_ImplSDL2_InitForOpenGL(m_window->getWindowPointerForImGui(), m_window->getGLContextPointerForImGui());
//	ImGui_ImplOpenGL3_Init(glsl_version);
//
//	//Load Fonts
//	io.Fonts->AddFontDefault();
//	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.0f);
//	if (font == NULL) {
//		SBX_INFO("Failed to load font Roboto-Medium.ttf ");
//	}
//	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/chintzy.ttf", 16.0f);
//	if (font == NULL) {
//		SBX_INFO("Failed to load font chintzy.ttf ");
//	}
//
//	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
//}
//
//void LevelEditor_Screen::showMainMenu()
//{
//	if (ImGui::BeginMainMenuBar())
//	{
//		if (ImGui::BeginMenu("Menu"))
//		{
//			if (ImGui::MenuItem("Auto Save", NULL, false)) {}
//			if (ImGui::MenuItem("Respect Ancestor", NULL, m_respect_ancestor)) {
//				m_respect_ancestor = !m_respect_ancestor;
//			}
//			if (ImGui::MenuItem("Auto Assign WorldCluster", NULL, m_auto_assign_world_cluster)) {
//				m_auto_assign_world_cluster = !m_auto_assign_world_cluster;
//				if (!m_auto_assign_world_cluster) {
//					m_mannually_choosen_world_cluster_index = m_current_active_world_cluster_index;
//					m_show_set_mannual_wc_index_dialog = true;
//				}
//			}
//			if (ImGui::MenuItem("Open", "Ctrl+O"))
//			{
//				m_show_open_level_file_dialog = true;
//			}
//			if (m_place_mode) {
//				// we can only save and  open new levels in place mode
//				if (ImGui::MenuItem("Save", "Ctrl+S"))
//				{
//					m_show_save_level_file_dialog = true;
//				}
//			}
//
//			if (ImGui::MenuItem("New")) {}
//			if (ImGui::BeginMenu("Open Recent"))
//			{
//				ImGui::MenuItem("fish_hat.c");
//				ImGui::MenuItem("fish_hat.inl");
//				ImGui::MenuItem("fish_hat.h");
//
//				ImGui::EndMenu();
//			}
//			if (ImGui::MenuItem("Save As..")) {}
//			if (ImGui::MenuItem("About", NULL)) { m_show_about_dialog = true; }
//			if (ImGui::MenuItem("Quit", "Alt+F4")) { m_request_exit = true; }
//			ImGui::EndMenu();
//		}
//
//		if (ImGui::BeginMenu("Edit"))
//		{
//			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
//			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
//			ImGui::Separator();
//			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
//			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
//			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
//			ImGui::EndMenu();
//		}
//
//		if (ImGui::BeginMenu("Extra Settings"))
//		{
//			if (ImGui::BeginMenu("Theme")) {
//				if (ImGui::MenuItem("Classic Theme")) {
//					m_theme_index = 0;
//				}
//				if (ImGui::MenuItem("Dark Theme")) {
//					m_theme_index = 2;
//				}
//				if (ImGui::MenuItem("Light Theme")) {
//					m_theme_index = 1;
//				}
//				ImGui::EndMenu();
//			}
//			if (ImGui::MenuItem("KeyBoard Bindings")) { m_show_key_bindings_dialog = true; };
//			ImGui::EndMenu();
//		}
//		if (ImGui::Button("Reload Animations Script"))
//		{
//			if (m_animation_creator.loadAnimationScript(m_animation_script)) {
//				SBX_INFO("Managed To load Animation Scipts");
//			}
//			else {
//				SBX_INFO("Failed To load Animation Scipts");
//			}
//		}
//
//		if (ImGui::Button("Reload Automation Script"))
//		{
//			if (m_automation.loadAutomationScript(m_automation_script)) {
//				SBX_INFO("Managed To load Automation Scipts");
//			}
//			else {
//				SBX_INFO("Failed To load Automation Scipts");
//			}
//		}
//
//		ImGui::EndMainMenuBar();
//	}
//
//	// Setup Dear ImGui style
//	if (m_theme_index == 0) {
//		ImGui::StyleColorsClassic();
//		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(127, 127, 127, 127));
//	}
//	else if (m_theme_index == 1) {
//		ImGui::StyleColorsLight();
//		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255));
//	}
//	else if (m_theme_index == 2) {
//		ImGui::StyleColorsDark();
//		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255));
//	}
//}
//
//void LevelEditor_Screen::drawGUI()
//{
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplSDL2_NewFrame(m_window->getWindowPointerForImGui());
//	ImGui::NewFrame();
//	bool m_show_demo_window = true;
//	ImGui::ShowDemoWindow(&m_show_demo_window);
//	showMainMenu();
//	ImGuiWindowFlags properties_tab_window_flags = 0;
//	properties_tab_window_flags |= ImGuiWindowFlags_NoMove;
//	bool* properties_tab__open = false;
//	ImGui::Begin("Properties Tab", properties_tab__open, properties_tab_window_flags);
//	ImGui::SetWindowPos(ImVec2(0, 20));
//	{
//		if (m_request_exit) {
//			ImGui::OpenPopup("CloseLevelEditor?");
//		}
//		if (m_show_set_mannual_wc_index_dialog) {
//			ImGui::OpenPopup("AutoAssignWorldCluster?");
//		}
//		if (m_show_world_locked_dialog) {
//			ImGui::OpenPopup("WorldLocked");
//		}
//
//		if (m_show_about_dialog) {
//			ImGui::OpenPopup("About?");
//		}
//
//		if (m_show_key_bindings_dialog) {
//			ImGui::OpenPopup("KeyBindings");
//		}
//
//		//Close Editor Tab
//				// Always center this window when appearing
//		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
//		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
//
//		if (ImGui::BeginPopupModal("CloseLevelEditor?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			ImGui::Text("Warning : Your Work is not Saved!!.\nAll these beautiful files will be deleted.This operation can not be reversed.\n\n");
//			ImGui::Separator();
//
//			//static int unused_i = 0;
//			//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");
//
//			static bool dont_ask_me_next_time = false;
//			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
//			ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
//			ImGui::PopStyleVar();
//
//			if (ImGui::Button("OK", ImVec2(120, 0))) {
//				ImGui::CloseCurrentPopup();
//				//Close  LevelEditor
//				m_allow_exit = true;
//				onExit();
//				m_request_exit = false;
//			}
//			ImGui::SetItemDefaultFocus();
//			ImGui::SameLine();
//			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
//				ImGui::CloseCurrentPopup();
//				m_request_exit = false;
//			}
//			ImGui::EndPopup();
//		}
//
//		if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			ImGui::Text("ABOUT:");
//			ImGui::BulletText("This is a 2D game world Editing tool");
//			ImGui::BulletText("Built and maintained by stande studios ");
//			ImGui::BulletText("Established 2021");
//
//			if (ImGui::Button("OK", ImVec2(120, 0))) {
//				m_show_about_dialog = false;
//				ImGui::CloseCurrentPopup();
//			}
//			ImGui::EndPopup();
//		}
//
//		if (ImGui::BeginPopupModal("KeyBindings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			ImGui::Text("PLACE MODE");
//			ImGui::BulletText("Mouse Left-Click       :  Place new cluster object into World");
//
//			ImGui::Separator();
//			ImGui::Text("SELECT MODE");
//			ImGui::Text("Cluster Mode:");
//			ImGui::BulletText("Mouse Left-Click        :  Select hovered world cluster");
//			ImGui::BulletText("Mouse Right-Click       :  Shifts selected world cluster to current mouse position");
//			ImGui::Text("Object Mode:");
//			ImGui::BulletText("Mouse Left-Click        :  Select hovered cluster object");
//			ImGui::BulletText("Mouse Right-Click       :  Shifts selected  cluster object to current mouse position");
//			ImGui::BulletText("TAB + NUM_ARROW_KEY     :  Duplicates selected cluster object");
//
//			ImGui::Text("Joint Mode:");
//
//			if (ImGui::Button("OK", ImVec2(120, 0))) {
//				m_show_key_bindings_dialog = false;
//				ImGui::CloseCurrentPopup();
//			}
//			ImGui::EndPopup();
//		}
//
//		if (ImGui::BeginPopupModal("AutoAssignWorldCluster?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			ImGui::Text("Must be in the range of the WorldCluster vector size()\n");
//			ImGui::Separator();
//			ImGui::DragInt("WorldClusterIndex", &m_mannually_choosen_world_cluster_index, 1, 0, m_world_clusters.size(), "%d%", ImGuiSliderFlags_AlwaysClamp);
//
//			if (m_mannually_choosen_world_cluster_index > m_world_clusters.size() - 1) {
//				m_mannually_choosen_world_cluster_index = m_world_clusters.size() - 1;
//			}
//
//			ImGuiSliderFlags test;
//			if (ImGui::Button("OK", ImVec2(120, 0))) {
//				m_show_set_mannual_wc_index_dialog = false;
//				ImGui::CloseCurrentPopup();
//			}
//			ImGui::SetItemDefaultFocus();
//			ImGui::SameLine();
//			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
//				m_auto_assign_world_cluster = true;
//				m_show_set_mannual_wc_index_dialog = false;
//				ImGui::CloseCurrentPopup();
//			}
//			ImGui::EndPopup();
//		}
//
//		if (ImGui::BeginPopupModal("WorldLocked", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			ImGui::Text(" The Physics Engine started at a bad memory Address .\nPlease reload the Editor");
//			ImGui::Separator();
//
//			static bool dont_ask_me_next_time_about_world_locked = false;
//			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
//			ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time_about_world_locked);
//			ImGui::PopStyleVar();
//
//			if (ImGui::Button("OK", ImVec2(120, 0))) {
//				ImGui::CloseCurrentPopup();
//				//Close  LevelEditor
//				m_allow_exit = true;
//				m_game_ptr->exitGame();
//				m_request_exit = false;
//			}
//			ImGui::SetItemDefaultFocus();
//			ImGui::SameLine();
//			ImGui::EndPopup();
//		}
//
//
//		if (m_place_mode) {
//			if (ImGui::Button("New Cluster")) {
//				m_auto_assign_world_cluster = true;//we are lossing control
//				m_world_clusters.emplace_back();
//				m_current_active_world_cluster_index = m_world_clusters.size() - 1;
//				m_current_world_cluster_ptr = &m_world_clusters[m_current_active_world_cluster_index];
//				m_current_world_cluster_ptr->index = m_current_active_world_cluster_index;
//				m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);
//
//				m_current_world_cluster_ptr->cluster_objects.emplace_back();
//
//				m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_world_cluster_ptr->cluster_objects.size() - 1];
//				updateStates();
//				m_current_cluster_object_ptr->cluster_index = m_world_clusters[m_current_active_world_cluster_index].index;
//				m_current_cluster_object_ptr->index = 0;
//				m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//			}
//		}
//		else if (m_select_mode) {
//			ImGui::Text("Selection");
//			int before = m_selection_mode_index;
//			ImGui::RadioButton("Cluster", &m_selection_mode_index, 0); ImGui::SameLine();
//			ImGui::RadioButton("Object ", &m_selection_mode_index, 1); ImGui::SameLine();
//			ImGui::RadioButton("Joints", &m_selection_mode_index, 2);
//
//			if (before != m_selection_mode_index) {//we changed our selection critria
//				m_selected_cluster_objects.clear();
//				m_current_cluster_object_ptr = nullptr;
//				m_current_shape_ptr = nullptr;
//				m_current_joint_ptr = nullptr;
//			}
//		}
//		if (m_current_cluster_object_ptr != nullptr) {
//			ImGui::Separator();
//			const char* camera_labels[]{ "1","2","3","4","5","6","7","8" };//hopefully imgui can handle this for us
//
//			ImGui::Text("Cluster");
//
//			if (m_auto_assign_world_cluster) {
//				ImGui::InputText("###CurrentWorldClustersName", (char*)&m_current_world_cluster_ptr->name, 20);
//				ImGui::Text("Camera"); ImGui::SameLine();
//				ImGui::Combo("###Camera", &m_current_world_cluster_ptr->cameraIndex, camera_labels, IM_ARRAYSIZE(camera_labels));
//			}
//			else {
//				ImGui::InputText("###CurrentWorldClustersName", (char*)&m_world_clusters[m_mannually_choosen_world_cluster_index].name, 20);
//				ImGui::Text("Camera : "); ImGui::SameLine();
//				ImGui::Combo("###Camera", &m_world_clusters[m_mannually_choosen_world_cluster_index].cameraIndex, camera_labels, IM_ARRAYSIZE(camera_labels));
//			}
//
//			if (ImGui::TreeNode("Camera Properties"))
//			{
//				static float cameraScale = m_camera.getScale();
//				ImGui::DragFloat("Scale", &cameraScale, 0.2, 0, 400, "%.3f");
//				m_camera.setScale(cameraScale);
//				ImGui::TreePop();
//			}
//
//			ImGui::Text("Object name");
//			ImGui::InputText("###CurrentClusterObjectsName", (char*)&m_current_cluster_object_ptr->name, 20);
//			ImGui::Separator();
//		}
//		ImGui::Text("Mode");
//
//		static int e = 0;
//		int beforeMode = e;
//		ImGui::RadioButton("Place Mode", &e, 0); ImGui::SameLine();
//		ImGui::RadioButton("Select Mode", &e, 1); ImGui::SameLine();
//		ImGui::Checkbox("Debug Mode", &m_debug_mode);
//
//		if (e == 0) {
//			m_place_mode = true;
//			m_select_mode = false;
//
//			m_selected_cluster_objects.clear();
//			if (m_auto_assign_world_cluster) {
//				m_current_active_cluster_object_index = m_world_clusters[m_current_active_world_cluster_index].cluster_objects.size() - 1;
//				m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_active_cluster_object_index];
//				if (m_current_cluster_object_ptr->is_alive) {
//					/*
//						this is true for when we have done dupping , because there the last ClusterObjects in this world cluster
//						is the dup we just placed ther
//					*/
//					m_world_clusters[m_current_active_world_cluster_index].cluster_objects.emplace_back();
//
//					m_current_active_cluster_object_index = m_world_clusters[m_current_active_world_cluster_index].cluster_objects.size() - 1;
//					m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_active_cluster_object_index];
//					//give it tis coordinates
//					m_current_cluster_object_ptr->cluster_index = m_current_active_world_cluster_index;
//					m_current_cluster_object_ptr->index = m_current_active_cluster_object_index;
//					//name it
//					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//				}
//				updateStates();
//			}
//			else {
//				m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects[m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.size() - 1];
//
//				if (m_current_cluster_object_ptr->is_alive) {
//					/*
//					this is true for when we have done dupping , because there the last ClusterObjects in this world cluster
//					is the dup we just placed ther
//
//					*/
//					m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.emplace_back();
//
//					const int coindex = m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.size() - 1;
//					m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects[coindex];
//					//give it tis coordinates
//					m_current_cluster_object_ptr->cluster_index = m_mannually_choosen_world_cluster_index;
//					m_current_cluster_object_ptr->index = coindex;
//					//name it
//					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//				}
//				updateStates();
//			}
//		}
//
//		else if (e == 1) {
//			m_place_mode = false;
//			m_select_mode = true;
//			if (beforeMode != 1) {
//				m_current_cluster_object_ptr = nullptr;
//			}
//		}
//		if (((m_select_mode&& m_selection_mode_index == 1) || m_place_mode) && m_current_cluster_object_ptr != nullptr) {
//			ImGui::Text("Color");
//			static float col2[4];
//			col2[0] = m_current_cluster_object_ptr->color.x;
//			col2[1] = m_current_cluster_object_ptr->color.y;
//			col2[2] = m_current_cluster_object_ptr->color.z;
//			col2[3] = m_current_cluster_object_ptr->color.w;
//
//			ImGui::ColorEdit4("Clear color", col2);
//
//			m_current_cluster_object_ptr->color.x = col2[0];
//			m_current_cluster_object_ptr->color.y = col2[1];
//			m_current_cluster_object_ptr->color.z = col2[2];
//			m_current_cluster_object_ptr->color.w = col2[3];
//
//			ImGui::RadioButton("Single Shape", &m_place_mode_index, 0); ImGui::SameLine();
//			ImGui::RadioButton("Compound Shape", &m_place_mode_index, 1);
//
//			if (m_place_mode_index == 0) {
//				ImGui::Text("Object Shape");
//				//Object Types
//
//				const char** shapes_labels = new const char*[m_vec_shapes_pointer.size()];
//
//				for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
//				{
//					shapes_labels[i] = ToString(m_vec_shapes_pointer[i]->body_shape);
//				}
//				//display our shapes names
//				ImGui::Combo("Shape ", &m_selected_shape, shapes_labels, m_vec_shapes_pointer.size());
//				delete shapes_labels;
//				//get our shapes dimensions
//
//				for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
//				{
//					if (m_selected_shape == i) {
//						ImGui::Text("Dimensions For "); ImGui::SameLine(); ImGui::Text(ToString(m_vec_shapes_pointer[i]->body_shape));
//						ImGui::Separator();
//
//						m_current_cluster_object_ptr->shape = m_vec_shapes_pointer[i]->body_shape;
//						m_current_shape_ptr = m_vec_shapes_pointer[i];
//
//						if (m_vec_shapes_pointer[i]->needs_height_width) {
//							ImGui::Text("Height : "); ImGui::SameLine();
//							ImGui::InputFloat("###b_h", &m_current_cluster_object_ptr->height, 0.05f, 0, "%.3f");
//							ImGui::Text("Width  : "); ImGui::SameLine();
//							ImGui::InputFloat("###b_w", &m_current_cluster_object_ptr->width, 0.05f, 0, "%.3f");
//						}
//						if (m_vec_shapes_pointer[i]->needs_radius || m_vec_shapes_pointer[i]->is_calculated_light) {
//							ImGui::Text("Radius  : "); ImGui::SameLine();
//							ImGui::InputFloat("###c_r", &m_current_cluster_object_ptr->radius, 0.025f, 0, "%.3f");
//						}
//
//						if (m_vec_shapes_pointer[i]->is_plotted || m_vec_shapes_pointer[i]->is_calculated_light) {
//							ImGui::Checkbox("Show Cords", &m_show_polygon_plots);
//							if (m_show_polygon_plots) {
//								if (m_current_cluster_object_ptr->vertices.size() >= 2) {
//									if (m_current_cluster_object_ptr->vertices.size() % 2 == 0) {
//										for (int i = 0; i < m_current_cluster_object_ptr->vertices.size() - 1; i++)
//										{
//											std::stringstream ss;
//											//TODO find out how to empty the stringstream that willbe more efficent than recreating it each time we need it
//											ss << "(";
//											ss << m_current_cluster_object_ptr->vertices[i].x;
//											ss << ",";
//											ss << m_current_cluster_object_ptr->vertices[i].y;
//											ss << ")";
//											ss << " ";
//
//											ss << "(";
//											ss << m_current_cluster_object_ptr->vertices[i + 1].x;
//											ss << ",";
//											ss << m_current_cluster_object_ptr->vertices[i + 1].y;
//											ss << ")";
//
//											ImGui::Text(ss.str().c_str());
//										}
//									}
//									else {
//										for (int i = 0; i < m_current_cluster_object_ptr->vertices.size() - 2; i++)
//										{
//											std::stringstream ss;
//
//											ss << "(";
//											ss << m_current_cluster_object_ptr->vertices[i].x;
//											ss << ",";
//											ss << m_current_cluster_object_ptr->vertices[i].y;
//											ss << ")";
//											ss << " ";
//
//											ss << "(";
//											ss << m_current_cluster_object_ptr->vertices[i + 1].x;
//											ss << ",";
//											ss << m_current_cluster_object_ptr->vertices[i + 1].y;
//											ss << ")";
//											ImGui::Text(ss.str().c_str());
//										}
//										std::stringstream ss;
//
//										ss << "(";
//										ss << m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].x;
//										ss << ",";
//										ss << m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].y;
//										ss << ")";
//										ss << "(? , ?)";
//										ImGui::Text(ss.str().c_str());
//									}
//								}
//							}
//
//							if (m_vec_shapes_pointer[i]->is_calculated_light) {
//								if (ImGui::Button("Add Edge")) {
//									if (m_current_cluster_object_ptr->vertices.size() >= m_vec_shapes_pointer[i]->min_plotting_vertices)
//									{
//										glm::vec2 start = m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 2];
//										glm::vec2 end = m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1];
//										m_current_cluster_object_ptr->vec_of_edges.push_back(std::pair(start, end));
//										m_current_cluster_object_ptr->vertices.clear();
//									}
//								}
//
//								if (ImGui::Button("Draw Light")) {
//									if (m_current_cluster_object_ptr->vec_of_edges.size() >= 4)
//									{
//										m_draw_plotted_shape = true;
//									}
//									else {
//										ImGui::OpenPopup("CantDrawPolt");
//									}
//								}
//							}
//							else {
//								if (ImGui::Button("Draw")) {
//									if (m_current_cluster_object_ptr->vertices.size() >= m_vec_shapes_pointer[i]->min_plotting_vertices)
//									{
//										m_draw_plotted_shape = true;
//									}
//									else {
//										ImGui::OpenPopup("CantDrawPolt");
//									}
//								}
//							}
//
//							if (ImGui::Button("Clear plots")) {
//								m_current_cluster_object_ptr->vec_of_edges.clear();
//								m_current_cluster_object_ptr->vertices.clear();
//							}
//
//							// Always center this window when appearing
//							ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
//							ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
//
//							if (ImGui::BeginPopupModal("CantDrawPolt", NULL, ImGuiWindowFlags_AlwaysAutoResize))
//							{
//								std::stringstream numToString;
//								int min_points = m_current_cluster_object_ptr->shape == SquareBox::BodyShapeEnum::CalculatedLightShape ? 4 : m_vec_shapes_pointer[i]->min_plotting_vertices;
//								numToString << min_points;
//								ImGui::Text("A minimum of "); ImGui::SameLine(); ImGui::Text(numToString.str().c_str()); ImGui::SameLine(); ImGui::Text(" points is need\n");
//								ImGui::Separator();
//
//								if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
//								ImGui::SetItemDefaultFocus();
//								ImGui::EndPopup();
//							}
//						}
//					}
//				}
//
//				ImGui::Text("Rotation  : "); ImGui::SameLine();
//				ImGui::InputFloat("###rtv", &m_current_cluster_object_ptr->angle, 0.05f, 0, "%.3f");
//				ImGui::Separator();
//
//				ImGui::Text("Texture");
//				ImGui::Separator();
//				static int current_texture_type = 0;
//				current_texture_type = m_current_cluster_object_ptr->texture_type;
//				ImGui::RadioButton("Single Sprite", &current_texture_type, 0); ImGui::SameLine();
//				ImGui::RadioButton("Tile Sheet", &current_texture_type, 1);
//				m_current_cluster_object_ptr->texture_type = current_texture_type;
//
//				if (m_current_cluster_object_ptr->texture_type == 0) {
//					ImGui::Text("uv_rect ");
//					ImGui::InputFloat("uv_x", &m_current_cluster_object_ptr->uv_rect.x, 0.01f, 0, "%.2f");
//					ImGui::InputFloat("uv_y", &m_current_cluster_object_ptr->uv_rect.y, 0.01f, 0, "%.2f");
//					ImGui::InputFloat("uv_z", &m_current_cluster_object_ptr->uv_rect.z, 0.01f, 0, "%.2f");
//					ImGui::InputFloat("uv_w", &m_current_cluster_object_ptr->uv_rect.w, 0.01f, 0, "%.2f");
//					std::vector<char*>  vc;
//					vc.reserve(m_single_textures.size());
//					std::transform(m_single_textures.begin(), m_single_textures.end(), std::back_inserter(vc), convertGLTextureDisplayNameToCstr);
//
//					//compute how many single textures we want to display
//					//we are only displaying those that are not empty strings
//					if (m_single_textures.size() > 0) {
//						ImGui::Combo("Single", &m_selected_single_texture_index, &vc[0], m_single_textures.size());
//						//current active texture
//						ImTextureID my_tex_id;
//						my_tex_id = (ImTextureID)m_single_textures[m_selected_single_texture_index].id;
//
//						float my_tex_w = 50;
//						float my_tex_h = 50;
//						float zoom = 4.0f;
//						ImGui::Text("Preview");
//						//get the uvCoords
//						glm::vec4 uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
//
//						ImVec2 uv_min = ImVec2(uv_rect.x, uv_rect.y);                 // Top-left
//						ImVec2 uv_max = ImVec2(uv_rect.z, uv_rect.w);                 // Lower-right
//						ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
//						ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
//						ImGui::Image(my_tex_id, ImVec2(my_tex_w* zoom, my_tex_h* zoom), uv_min, uv_max, tint_col, border_col);
//					}
//					else {
//						ImGui::Combo("Single", &m_selected_single_texture_index, "Empty", m_single_textures.size());
//					}
//
//					if (ImGui::Button("+ Single")) {
//						m_show_open_texture_file_dialog = true;
//					}
//				}
//				else if (m_current_cluster_object_ptr->texture_type == 1) {
//					ImGui::Text("Tiling:");
//					ImGui::Text("rows : "); ImGui::SameLine();
//					ImGui::InputFloat("###rows", &m_current_cluster_object_ptr->texture.tiling.x, 1.0f, 0, "%.0f");
//					ImGui::Text("cols : "); ImGui::SameLine();
//					ImGui::InputFloat("###cols", &m_current_cluster_object_ptr->texture.tiling.y, 1.0f, 0, "%.0f");
//
//					std::vector<char*>  vc;
//					vc.reserve(m_tile_sheet_textures.size());
//					std::transform(m_tile_sheet_textures.begin(), m_tile_sheet_textures.end(), std::back_inserter(vc), convertGLTextureDisplayNameToCstr);
//
//					//compute how many single textures we want to display
//					//we are only displaying those that are not empty strings
//					if (m_tile_sheet_textures.size() > 0) {
//						//adjust the stored textures tiling
//						m_tile_sheet_textures[m_selected_tile_sheet_texture_index].tiling = m_current_cluster_object_ptr->texture.tiling;
//						ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], m_tile_sheet_textures.size());
//
//						//current active texture
//						ImTextureID my_tex_id;
//						my_tex_id = (ImTextureID)m_tile_sheet_textures[m_selected_tile_sheet_texture_index].id;
//
//						float my_tex_w = 50;
//						float my_tex_h = 50;
//						float zoom = 4.0f;
//						ImGui::Text("Preview");
//						//get the uvCoords
//						SquareBox::AssetManager::TileSheet tileSheet;
//						tileSheet.readGrid(m_current_cluster_object_ptr->texture);
//						glm::vec4 uv_rect = tileSheet.getUVCords(m_current_tile_sheet_index);
//
//						ImVec2 uv_min = ImVec2(uv_rect.x, uv_rect.y);                 // Top-left
//						ImVec2 uv_max = ImVec2(uv_rect.z, uv_rect.w);                 // Lower-right
//						ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
//						ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
//						ImGui::Image(my_tex_id, ImVec2(my_tex_w* zoom, my_tex_h* zoom), uv_min, uv_max, tint_col, border_col);
//					}
//					else {
//						ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, "Empty", m_tile_sheet_textures.size());
//					}
//					ImVec2 posee = ImGui::GetCursorScreenPos();
//
//					if (ImGui::Button("+ TileSheet")) {
//						m_show_open_tile_sheet_file_dialog = true;
//					}
//
//					ImGui::Text("Index : "); ImGui::SameLine();
//					ImGui::InputInt("###m_current_tile_sheet_index", &m_current_tile_sheet_index);
//				}
//			}
//			else if (m_place_mode_index == 1) {
//			}
//			ImGui::Separator();
//		}
//	}
//	ImGui::End();
//
//	//setting our current texture for this current object, regard less if we have interacted with the GUI code or not
//
//	if (m_current_cluster_object_ptr != nullptr) {
//		if (m_current_cluster_object_ptr->texture_type == 0) {
//			//Singles
//			if (m_single_textures.size() > 0) {
//				m_current_cluster_object_ptr->texture = m_single_textures[m_selected_single_texture_index];
//				m_current_cluster_object_ptr->texture_index = m_selected_single_texture_index;
//			}
//		}
//		else if (m_current_cluster_object_ptr->texture_type == 1) {
//			//sheets
//			if (m_tile_sheet_textures.size() > 0) {
//				m_current_cluster_object_ptr->texture = m_tile_sheet_textures[m_selected_tile_sheet_texture_index];
//				m_current_cluster_object_ptr->texture_index = m_selected_tile_sheet_texture_index;
//			}
//		}
//	}
//
//	ImGuiWindowFlags physics_tab_window_flags = 0;
//	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
//	bool* physics_tab_open = false;
//	ImGui::Begin("Physics Tab", physics_tab_open, physics_tab_window_flags);
//
//	ImGui::SetWindowPos(ImVec2(m_window->getScreenWidth() - ImGui::GetWindowWidth() - 2, 20));
//	{
//		if (m_play_simulation) {
//			if (ImGui::Button("Pause")) {
//				m_play_simulation = false;
//			}
//		}
//		else {
//			if (ImGui::Button("Play")) {
//				m_play_simulation = true;
//			}
//		}
//
//		std::vector<std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>> vectorOfPairVector;
//
//		for (unsigned int i = 0; i < m_alive_cluster_objects.size(); i++)
//		{
//			SquareBox::GWOM::WorldCluster & cwcl = m_world_clusters[m_alive_cluster_objects[i].first];
//			SquareBox::GWOM::ClusterObject & cwobj = m_world_clusters[m_alive_cluster_objects[i].first].cluster_objects[m_alive_cluster_objects[i].second];
//			bool isAlreadyPresentInVecOfpair = false;
//			for (unsigned int j = 0; j < vectorOfPairVector.size(); j++) {
//				if (vectorOfPairVector[j].first.first == cwcl.name) {
//					isAlreadyPresentInVecOfpair = true;
//					vectorOfPairVector[j].second.push_back(std::pair(cwobj.name, cwobj.index));
//				}
//			}
//			if (isAlreadyPresentInVecOfpair == false) {
//				vectorOfPairVector.push_back(std::pair(std::pair(cwcl.name, cwcl.index), std::vector<std::pair<std::string, int>>()));
//				vectorOfPairVector[vectorOfPairVector.size() - 1].second.push_back(std::pair(cwobj.name, cwobj.index));
//			}
//		}
//
//		if (ImGui::TreeNode("World Clusters"))
//		{
//			for (unsigned int i = 0; i < vectorOfPairVector.size(); i++)
//			{
//				if (ImGui::TreeNode(vectorOfPairVector[i].first.first.c_str()))
//				{
//					for (int j = 0; j < vectorOfPairVector[i].second.size(); j++)
//					{
//						if (ImGui::TreeNode((void*)(intptr_t)j, "%s %d", vectorOfPairVector[i].second[j].first.c_str(), vectorOfPairVector[i].second[j].second))
//						{
//							if (ImGui::SmallButton("up")) {
//								m_utilities.movePairInVector(m_alive_cluster_objects, std::pair(vectorOfPairVector[i].first.second, vectorOfPairVector[i].second[j].second), -1);
//							}
//							ImGui::SameLine();
//							if (ImGui::SmallButton("down")) {
//								m_utilities.movePairInVector(m_alive_cluster_objects, std::pair(vectorOfPairVector[i].first.second, vectorOfPairVector[i].second[j].second), 1);
//							}
//							ImGui::SameLine();
//							SquareBox::GWOM::ClusterObject & ccwobj = m_world_clusters[vectorOfPairVector[i].first.second].cluster_objects[vectorOfPairVector[i].second[j].second];
//							if (ccwobj.is_hidden) {
//								if (ImGui::SmallButton("Show")) {
//									ccwobj.is_hidden = false;
//								}
//							}
//
//							else {
//								if (ImGui::SmallButton("Hide")) {
//									ccwobj.is_hidden = true;
//								}
//							}
//
//							ImGui::TreePop();
//						}
//					}
//					ImGui::TreePop();
//				}
//			}
//
//			ImGui::TreePop();
//		}
//
//		if (m_select_mode && m_selection_mode_index == 1 && m_current_cluster_object_ptr != nullptr) {
//			ImGui::Text("Current Cluster Object's Properties ");
//		}
//
//		if ((m_select_mode && m_selection_mode_index == 1 || m_place_mode) && m_current_cluster_object_ptr != nullptr) {
//			ImGui::Checkbox("enable Physics", &m_enable_physics);
//			ImGui::Text("Body Type");
//			ImGui::RadioButton("Dynamic ", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::DynamicBody)); ImGui::SameLine();
//			ImGui::RadioButton("Static", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::StaticBody)); ImGui::SameLine();
//			ImGui::RadioButton("Kinematic", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::KinematicBody));
//
//			m_current_cluster_object_ptr->type = static_cast<SquareBox::BodyTypeEnum>(m_selected_body_type);
//
//			std::stringstream ss;
//			ss << "World Cluster  Index ";
//			ss << m_current_cluster_object_ptr->cluster_index;
//			ImGui::Text(ss.str().c_str());
//			ss.clear();
//
//			std::stringstream ss1;
//			ss1 << "Cluster Object Index ";
//			ss1 << m_current_cluster_object_ptr->index;
//			ImGui::Text(ss1.str().c_str());
//			ss1.clear();
//
//			bool enable_cluster_object_rotation = !m_current_cluster_object_ptr->is_fixed_rotation;
//			ImGui::Text("Rotation  : "); ImGui::SameLine();
//			ImGui::Checkbox("###rot", &enable_cluster_object_rotation);
//			m_current_cluster_object_ptr->is_fixed_rotation = !enable_cluster_object_rotation;
//
//			bool is_cluster_object_bullet = !m_current_cluster_object_ptr->is_bullet;
//			ImGui::Text("Bullet  : "); ImGui::SameLine();
//			ImGui::Checkbox("###bulet", &is_cluster_object_bullet);
//			m_current_cluster_object_ptr->is_bullet = !is_cluster_object_bullet;
//
//			ImGui::Text("Density    : "); ImGui::SameLine();
//			ImGui::InputFloat("###d_t", &m_current_cluster_object_ptr->density, 0.05f, 0, "%.3f");
//
//			ImGui::Text("Fricition  : "); ImGui::SameLine();
//			ImGui::InputFloat("###f_r", &m_current_cluster_object_ptr->friction, 0.05f, 0, "%.3f");
//
//			ImGui::Text("Restitution : "); ImGui::SameLine();
//			ImGui::InputFloat("###r_s", &m_current_cluster_object_ptr->restitution, 0.05f, 0, "%.3f");
//
//			ImGui::Text("gravity_scale: "); ImGui::SameLine();
//			ImGui::InputFloat("###g_s", &m_current_cluster_object_ptr->gravity_scale, 0.01f, 0, "%.3f");
//
//			ImGui::Text("linear_damping : "); ImGui::SameLine();
//			ImGui::InputFloat("###l_d", &m_current_cluster_object_ptr->linear_damping, 0.01f, 0, "%.3f");
//
//			ImGui::Text("angular_damping: "); ImGui::SameLine();
//			ImGui::InputFloat("###a_d", &m_current_cluster_object_ptr->angular_damping, 0.01f, 0, "%.3f");
//
//			ImGui::Text("IAV	    : "); ImGui::SameLine();
//			ImGui::InputFloat("###i_v", &m_current_cluster_object_ptr->init_angular_velocity, 0.05f, 0, "%.3f");
//
//			ImGui::RadioButton("Family a", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_a));
//			ImGui::RadioButton("Family b", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_b));
//			ImGui::RadioButton("Family c", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_c));
//			ImGui::RadioButton("Family d", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_d));
//			ImGui::RadioButton("Family e", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_e));
//			ImGui::RadioButton("Family f", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_f));
//			ImGui::RadioButton("Family g", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_g));
//			ImGui::RadioButton("Family h", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_h));
//			ImGui::RadioButton("Family i", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_i));
//			ImGui::RadioButton("Family j", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_j));
//			ImGui::RadioButton("Family k", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_k));
//			ImGui::RadioButton("Family l", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_l));
//			ImGui::RadioButton("Family m", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_m));
//			ImGui::RadioButton("Family n", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_n));
//			ImGui::RadioButton("Family o", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_o));
//			ImGui::RadioButton("Family p", &m_selected_family, static_cast<int>(SquareBox::FamilyBitsEnum::Family_p));
//
//			m_current_cluster_object_ptr->family = static_cast<SquareBox::FamilyBitsEnum>(m_selected_family);
//
//			ImGui::Text("Mask Bits");
//			{
//				static bool has_mask_a;
//				//getting current state
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a))) {
//					has_mask_a = true;
//				}
//				else {
//					has_mask_a = false;
//				}
//				ImGui::Checkbox("Mask a", &has_mask_a);
//				//acknowledging the changes
//				if (has_mask_a) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_a));
//				}
//
//				static bool has_mask_b = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b))) {
//					has_mask_b = true;
//				}
//				else {
//					has_mask_b = false;
//				}
//				ImGui::Checkbox("Mask b", &has_mask_b);
//				//acknowledging the changes
//				if (has_mask_b) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_b));
//				}
//
//				static bool has_mask_c = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c))) {
//					has_mask_c = true;
//				}
//				else {
//					has_mask_c = false;
//				}
//				ImGui::Checkbox("Mask c", &has_mask_c);
//				//acknowledging the changes
//				if (has_mask_c) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_c));
//				}
//
//				static bool has_mask_d = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d))) {
//					has_mask_d = true;
//				}
//				else {
//					has_mask_d = false;
//				}
//				ImGui::Checkbox("Mask d", &has_mask_d);
//				//acknowledging the changes
//				if (has_mask_d) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_d));
//				}
//
//				static bool has_mask_e = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e))) {
//					has_mask_e = true;
//				}
//				else {
//					has_mask_e = false;
//				}
//				ImGui::Checkbox("Mask e", &has_mask_e);
//				//acknowledging the changes
//				if (has_mask_e) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_e));
//				}
//
//				static bool has_mask_f = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f))) {
//					has_mask_f = true;
//				}
//				else {
//					has_mask_f = false;
//				}
//				ImGui::Checkbox("Mask f", &has_mask_f);
//				//acknowledging the changes
//				if (has_mask_f) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_f));
//				}
//
//				static bool has_mask_g = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g))) {
//					has_mask_g = true;
//				}
//				else {
//					has_mask_g = false;
//				}
//				ImGui::Checkbox("Mask g", &has_mask_g);
//				//acknowledging the changes
//				if (has_mask_g) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_g));
//				}
//
//				static bool has_mask_h = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h))) {
//					has_mask_h = true;
//				}
//				else {
//					has_mask_h = false;
//				}
//				ImGui::Checkbox("Mask h", &has_mask_h);
//				//acknowledging the changes
//				if (has_mask_h) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_h));
//				}
//
//				static bool has_mask_i = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i))) {
//					has_mask_i = true;
//				}
//				else {
//					has_mask_i = false;
//				}
//				ImGui::Checkbox("Mask i", &has_mask_i);
//				//acknowledging the changes
//				if (has_mask_i) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_i));
//				}
//
//				static bool has_mask_j = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j))) {
//					has_mask_j = true;
//				}
//				else {
//					has_mask_j = false;
//				}
//				ImGui::Checkbox("Mask j", &has_mask_j);
//				//acknowledging the changes
//				if (has_mask_j) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_j));
//				}
//
//				static bool has_mask_k = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k))) {
//					has_mask_k = true;
//				}
//				else {
//					has_mask_k = false;
//				}
//				ImGui::Checkbox("Mask k", &has_mask_k);
//				//acknowledging the changes
//				if (has_mask_k) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_k));
//				}
//
//				static bool has_mask_l = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l))) {
//					has_mask_l = true;
//				}
//				else {
//					has_mask_l = false;
//				}
//				ImGui::Checkbox("Mask l", &has_mask_l);
//				//acknowledging the changes
//				if (has_mask_l) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_l));
//				}
//
//				static bool has_mask_m = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m))) {
//					has_mask_m = true;
//				}
//				else {
//					has_mask_m = false;
//				}
//				ImGui::Checkbox("Mask m", &has_mask_m);
//				//acknowledging the changes
//				if (has_mask_m) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_m));
//				}
//
//				static bool has_mask_n = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n))) {
//					has_mask_n = true;
//				}
//				else {
//					has_mask_n = false;
//				}
//				ImGui::Checkbox("Mask n", &has_mask_n);
//				//acknowledging the changes
//				if (has_mask_n) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_n));
//				}
//
//				static bool has_mask_o = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o))) {
//					has_mask_o = true;
//				}
//				else {
//					has_mask_o = false;
//				}
//				ImGui::Checkbox("Mask o", &has_mask_o);
//				//acknowledging the changes
//				if (has_mask_o) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_o));
//				}
//
//				static bool has_mask_p = false;
//				//getting current state
//
//				if (m_utilities.isIntVectorMember(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p))) {
//					has_mask_p = true;
//				}
//				else {
//					has_mask_p = false;
//				}
//				ImGui::Checkbox("Mask p", &has_mask_p);
//				//acknowledging the changes
//				if (has_mask_p) {
//					//if not part add it
//					m_utilities.addIntToVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p));
//				}
//				else {
//					//if part remove it
//					m_utilities.removeIntFromVector(m_current_cluster_object_ptr->vec_of_mask_bits, static_cast<int>(SquareBox::MaskBitsEnum::Mask_p));
//				}
//			}
//
//			ImGui::Text("Grouping Index");
//			ImGui::SameLine();
//			ImGui::InputInt("###grouping_index", &m_current_cluster_object_ptr->group_index);
//			ImGui::Separator();
//		}
//		else if (m_selection_mode_index == 2 && m_select_mode) {
//			ImGui::Checkbox("Show Joints", &m_debug_show_joints);
//			static bool plot_joints = false;
//
//			if (m_current_joint_ptr == nullptr) {
//				ImGui::Checkbox("Plot Bridge", &plot_joints);
//			}
//			//for the bridgeWrapper
//			if (plot_joints) {
//				if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))) {
//					glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
//					if (m_vector_of_joint_ploting_points.size() < 2) {
//						m_vector_of_joint_ploting_points.push_back(mouse_in_world);
//					}
//				}
//
//				if (ImGui::Button("Clear Plots")) {
//					m_vector_of_joint_ploting_points.clear();
//				}
//
//				if (m_vector_of_joint_ploting_points.size() == 2 && m_current_joint_ptr == nullptr) {
//					static float bridgeWeightLoss = 0.1f;
//					ImGui::InputFloat("###bridgeWeightLoss", &bridgeWeightLoss, 0.1f, 0, "%.2f");
//
//					static int bridge_shape = 0;
//					ImGui::RadioButton("Boxes", &bridge_shape, 0);
//					ImGui::RadioButton("Circles", &bridge_shape, 1);
//
//					if (bridge_shape == 0) {
//						m_temp_bridge_joints_object.shape = SquareBox::BodyShapeEnum::Box;
//						ImGui::InputFloat("###boxBridgeHeight", &m_temp_bridge_joints_object.height, 0.1f, 0, "%.2f");
//						ImGui::InputFloat("###boxBridgeWidth", &m_temp_bridge_joints_object.width, 0.1f, 0, "%.2f");
//					}
//					else if (bridge_shape == 1) {
//						m_temp_bridge_joints_object.shape = SquareBox::BodyShapeEnum::Circle;
//						ImGui::InputFloat("###circleBridgeRadius", &m_temp_bridge_joints_object.radius, 0.1f, 0, "%.2f");
//					}
//
//					bool enable_bridge_objects_rotation = !m_temp_bridge_joints_object.is_fixed_rotation;
//					ImGui::Text("Rotation  : "); ImGui::SameLine();
//					ImGui::Checkbox("###bridge_objects_rot", &enable_bridge_objects_rotation);
//					m_temp_bridge_joints_object.is_fixed_rotation = !enable_bridge_objects_rotation;
//
//					//the texture will be added later , after when the bridge has already been created
//
//					if (ImGui::Button("Draw Bridge")) {
//						SquareBox::Shapes::BridgeWrapper bridgewrapper;
//						if (m_auto_assign_world_cluster) {
//							bridgewrapper.createBridge(m_vector_of_joint_ploting_points[0], m_vector_of_joint_ploting_points[1], m_world_clusters, m_physics_world, m_current_active_world_cluster_index, m_alive_cluster_objects, m_temp_bridge_joints_object, bridgeWeightLoss);
//						}
//						else {
//							bridgewrapper.createBridge(m_vector_of_joint_ploting_points[0], m_vector_of_joint_ploting_points[1], m_world_clusters, m_physics_world, m_mannually_choosen_world_cluster_index, m_alive_cluster_objects, m_temp_bridge_joints_object, bridgeWeightLoss);
//						}
//						m_vector_of_joint_ploting_points.clear();
//					}
//				}
//			}
//			//for the pulley Joint
//			if (m_current_joint_ptr != nullptr) {
//				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
//					if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))) {
//						glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
//						if (m_current_joint_ptr->vec_of_points.size() < 2) {
//							m_current_joint_ptr->vec_of_points.push_back(mouse_in_world);
//						}
//					}
//
//					if (ImGui::Button("Clear Plots")) {
//						m_current_joint_ptr->vec_of_points.clear();
//					}
//				}
//			}
//			if (m_selected_cluster_objects.size() == 2) {
//				//loop through these bodies joints and get the details of the joint between them
//				//clusterObjects can have onlyOne joint between them
//				SquareBox::GWOM::ClusterObject & ccobjA = m_world_clusters[m_selected_cluster_objects[0].first].cluster_objects[m_selected_cluster_objects[0].second];
//				SquareBox::GWOM::ClusterObject & ccobjB = m_world_clusters[m_selected_cluster_objects[1].first].cluster_objects[m_selected_cluster_objects[1].second];
//				for (unsigned int i = 0; i < ccobjA.joints.size(); i++)
//				{
//					std::pair<int, int> bodyAACoordinates = std::pair(ccobjA.joints[i]->body_a_coordinates.first, ccobjA.joints[i]->body_a_coordinates.second);
//					std::pair<int, int> bodyABCoordinates = std::pair(ccobjA.joints[i]->body_b_coordinates.first, ccobjA.joints[i]->body_b_coordinates.second);
//
//					if (
//						(
//							bodyAACoordinates == std::pair(ccobjA.cluster_index, ccobjA.index)
//							&&
//							bodyABCoordinates == std::pair(ccobjB.cluster_index, ccobjB.index)
//							)
//						||
//						(
//							bodyAACoordinates == std::pair(ccobjB.cluster_index, ccobjB.index)
//							&&
//							bodyABCoordinates == std::pair(ccobjA.cluster_index, ccobjA.index)
//							)
//						) {
//						m_current_joint_ptr = ccobjA.joints[i];
//						break;//break the loop
//					}
//				}
//				if (m_current_joint_ptr == nullptr) {
//					//we dont have a joint so create one
//					m_current_joint_ptr = new SquareBox::GWOM::Joint();
//					m_current_joint_ptr->is_joint_alive = false;
//
//					m_current_joint_ptr->body_a_coordinates = std::pair(ccobjA.cluster_index, ccobjA.index);
//					m_current_joint_ptr->body_b_coordinates = std::pair(ccobjB.cluster_index, ccobjB.index);
//				}
//				std::pair<int, int> currentjointbodyACoords = std::pair(m_current_joint_ptr->body_a_coordinates.first, m_current_joint_ptr->body_a_coordinates.second);
//				std::pair<int, int> currentjointbodyBCoords = std::pair(m_current_joint_ptr->body_b_coordinates.first, m_current_joint_ptr->body_b_coordinates.second);
//				//incase of any discripases this is to be considered as a new joint
//				if (
//					!(
//
//					(currentjointbodyACoords == std::pair(ccobjA.cluster_index, ccobjA.index) &&
//						currentjointbodyBCoords == std::pair(ccobjB.cluster_index, ccobjB.index))
//						||
//						(currentjointbodyACoords == std::pair(ccobjB.cluster_index, ccobjB.index) &&
//							currentjointbodyBCoords == std::pair(ccobjA.cluster_index, ccobjA.index)))
//					) {
//					m_current_joint_ptr = new SquareBox::GWOM::Joint();
//					m_current_joint_ptr->is_joint_alive = false;
//					m_current_joint_ptr->body_a_coordinates = std::pair(ccobjA.cluster_index, ccobjA.index);
//					m_current_joint_ptr->body_b_coordinates = std::pair(ccobjB.cluster_index, ccobjB.index);
//				}
//
//				ImGui::Text("Joint");
//				ImGui::Text("Body A : "); ImGui::SameLine();
//				ImGui::Text(m_world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second].name);
//				ImGui::Text("Body B : "); ImGui::SameLine();
//				ImGui::Text(m_world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second].name);
//
//				const char * JointTypes[]{ ToString(SquareBox::JointTypeEnum::distance_joint),ToString(SquareBox::JointTypeEnum::revolute_joint),ToString(SquareBox::JointTypeEnum::prismatic_joint),ToString(SquareBox::JointTypeEnum::pulley_joint) };
//
//				int selectedjointtype = static_cast<int>(m_current_joint_ptr->joint_type);
//				ImGui::Combo("Joint Type ", &selectedjointtype, JointTypes, IM_ARRAYSIZE(JointTypes));
//				m_current_joint_ptr->joint_type = static_cast<SquareBox::JointTypeEnum>(selectedjointtype);
//
//				bool enable_collision_at_joint = m_current_joint_ptr->collide_connected_bodies;
//				ImGui::Checkbox("Collision  ", &enable_collision_at_joint);
//				m_current_joint_ptr->collide_connected_bodies = enable_collision_at_joint;
//
//				ImGui::Text("Body A anchor: ");
//				ImGui::InputFloat("###j_body_a_anchor_x", &m_current_joint_ptr->local_anchor_a.x, 0.01f, 0, "%.3f");
//				ImGui::InputFloat("###j_body_a_anchor_y", &m_current_joint_ptr->local_anchor_a.y, 0.01f, 0, "%.3f");
//
//				ImGui::Text("Body B anchor: ");
//				ImGui::InputFloat("###j_body_b_anchor_x", &m_current_joint_ptr->local_anchor_b.x, 0.01f, 0, "%.3f");
//				ImGui::InputFloat("###j_body_b_anchor_y", &m_current_joint_ptr->local_anchor_b.y, 0.01f, 0, "%.3f");
//
//				//unique Attributes
//				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint) {
//					ImGui::Text("Lower Angle: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_lower_angle", &m_current_joint_ptr->lower_angle, 0.01f, 0, "%.3f");
//					ImGui::Text("Upper Angle: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_upper_angle", &m_current_joint_ptr->upper_angle, 0.01f, 0, "%.3f");
//					ImGui::Text("Max Motor Torque"); ImGui::SameLine();
//					ImGui::InputFloat("###j_maxMotorTorque", &m_current_joint_ptr->max_motor_torque, 0.01f, 0, "%.3f");
//				}
//				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
//					ImGui::Text("Lower Translation: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_lower_translation", &m_current_joint_ptr->lower_translation, 0.01f, 0, "%.3f");
//					ImGui::Text("Upper Translation: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_upper_translation", &m_current_joint_ptr->upper_translation, 0.01f, 0, "%.3f");
//
//					ImGui::Text("Max Motor Force"); ImGui::SameLine();
//					ImGui::InputFloat("###j_maxMotorForce", &m_current_joint_ptr->max_motor_force, 0.01f, 0, "%.3f");
//
//					ImGui::Text("Local Axis A: ");
//					ImGui::InputFloat("###j_localAxisA_x", &m_current_joint_ptr->local_axis_a.x, 0.01f, 0, "%.3f");
//					ImGui::InputFloat("###j_localAxisA_y", &m_current_joint_ptr->local_axis_a.y, 0.01f, 0, "%.3f");
//				}
//				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::distance_joint) {
//					//distance
//					ImGui::Text("DampingRatio   : "); ImGui::SameLine();
//					ImGui::InputFloat("###j_dr", &m_current_joint_ptr->damping_ratio, 0.01f, 0, "%.3f");
//
//					ImGui::Text("frequency_hz: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_f", &m_current_joint_ptr->frequency_hz, 0.01f, 0, "%.3f");
//
//					ImGui::Text("Stiffness: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_stiffness", &m_current_joint_ptr->stiffness, 0.01f, 0, "%.3f");
//
//					ImGui::Text("Length: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_length", &m_current_joint_ptr->length, 0.01f, 0, "%.3f");
//
//					ImGui::Text("Max Length: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_max_length", &m_current_joint_ptr->max_length, 0.01f, 0, "%.3f");
//
//					ImGui::Text("MinLength: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_min_length", &m_current_joint_ptr->min_length, 0.01f, 0, "%.3f");
//				}
//				else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
//					if (m_current_joint_ptr->vec_of_points.size() == 2) {
//						m_current_joint_ptr->ground_anchor_a = m_current_joint_ptr->vec_of_points[0];
//						m_current_joint_ptr->ground_anchor_b = m_current_joint_ptr->vec_of_points[1];
//						m_current_joint_ptr->length_a = fabs(glm::length(m_current_joint_ptr->vec_of_points[0] - ccobjA.physics_properties->getXYPosition()));
//						m_current_joint_ptr->length_b = fabs(glm::length(m_current_joint_ptr->vec_of_points[1] - ccobjB.physics_properties->getXYPosition()));
//					}
//					ImGui::Text("Pulley Ratio: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_pulley_ratio", &m_current_joint_ptr->pulley_ratio, 0.01f, 0, "%.3f");
//					static float pulley_joint_color[4];
//					pulley_joint_color[0] = m_current_joint_ptr->color.x;
//					pulley_joint_color[1] = m_current_joint_ptr->color.y;
//					pulley_joint_color[2] = m_current_joint_ptr->color.z;
//					pulley_joint_color[3] = m_current_joint_ptr->color.w;
//
//					ImGui::ColorEdit4("Pulley color", pulley_joint_color);
//
//					m_current_joint_ptr->color.x = pulley_joint_color[0];
//					m_current_joint_ptr->color.y = pulley_joint_color[1];
//					m_current_joint_ptr->color.z = pulley_joint_color[2];
//					m_current_joint_ptr->color.w = pulley_joint_color[3];
//
//					ImGui::InputFloat("###j_pulley_rope_thickness", &m_current_joint_ptr->thickness, 0.01f, 0, "%.3f");
//				}
//
//				//shared attributes
//				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint || m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
//					ImGui::Text("Reference Angle: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_reference_angle", &m_current_joint_ptr->reference_angle, 0.01f, 0, "%.3f");
//					ImGui::Text("Motor Speed: "); ImGui::SameLine();
//					ImGui::InputFloat("###j_motor_speed", &m_current_joint_ptr->motor_speed, 0.01f, 0, "%.3f");
//
//					bool enable_joint_limit = m_current_joint_ptr->enable_limit;
//					ImGui::Checkbox("enable Limit: ", &enable_joint_limit);
//					m_current_joint_ptr->enable_limit = enable_joint_limit;
//
//					bool enable_joint_motor = m_current_joint_ptr->enable_motor;
//					ImGui::Checkbox("enable Motor: ", &enable_joint_motor);
//					m_current_joint_ptr->enable_motor = enable_joint_motor;
//				}
//
//				if (m_current_joint_ptr->is_joint_alive) {
//					if (ImGui::Button("Update Joint")) {
//						if (ccobjA.physics_properties != nullptr && ccobjB.physics_properties != nullptr) {
//							m_utilities.destroyClusterObjectJoint(ccobjA, ccobjB);
//							m_physics_world.createJoint(ccobjA, ccobjB, *m_current_joint_ptr);
//							m_current_joint_ptr = nullptr;
//						}
//					}
//					ImGui::SameLine();
//					if (ImGui::Button("Destory Joint")) {
//						m_utilities.destroyClusterObjectJoint(ccobjA, ccobjB);
//						m_current_joint_ptr = nullptr;
//					}
//				}
//				else {
//					if (ImGui::Button("Create Joint")) {
//						if (ccobjA.physics_properties != nullptr && ccobjB.physics_properties != nullptr) {
//							m_utilities.setCurrentShapePointer(ccobjA.shape, &m_current_shape_ptr);
//							if (m_current_shape_ptr->can_be_joint_memeber) {
//								m_utilities.setCurrentShapePointer(ccobjB.shape, &m_current_shape_ptr);
//								if (m_current_shape_ptr->can_be_joint_memeber) {
//									m_physics_world.createJoint(ccobjA, ccobjB, *m_current_joint_ptr);
//									m_vector_of_joint_ploting_points.clear();
//									m_current_joint_ptr = nullptr;
//								}
//							}
//						}
//					}
//				}
//			}
//			else {
//				m_current_joint_ptr = nullptr;
//			}
//		}
//
//		if (m_selection_mode_index == 0) {
//			//WorldCluster
//			if (m_selected_cluster_objects.size() > 0) {
//				std::stringstream ss;
//				ss << "World Cluster  Index ";
//				ss << m_world_clusters[m_selected_cluster_objects[0].first].index;
//				ImGui::Text(ss.str().c_str());
//				ss.clear();
//
//				ImGui::Text("World Cluster Name");
//				ImGui::InputText("###SelectedClusterObjectsName", (char*)&m_world_clusters[m_selected_cluster_objects[0].first].name, 20);
//
//				ImGui::Separator();
//				if (ImGui::Button("Delete")) {
//					for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
//					{
//						m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second].life_span = 0;
//					}
//				}
//			}
//		}
//		else if (m_selection_mode_index == 1 && m_current_cluster_object_ptr != nullptr) {
//			//ClusterObject
//			if (m_select_mode && m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
//				if (ImGui::Button("Update"))
//				{
//					m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
//					//we are trusting our program to always make the selected object the current object
//					if (m_current_shape_ptr->is_plotted) {
//						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//					}
//					else
//					{
//						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//					}
//				}
//
//				if (ImGui::Button("Delete")) {
//					m_current_cluster_object_ptr->life_span = 0;
//				}
//			}
//		}
//		else if (m_selection_mode_index == 2) {
//			//joint Selection
//			if (ImGui::Button("Clear Selection")) {
//				m_selected_cluster_objects.clear();
//			}
//			if (m_selected_cluster_objects.size() > 0) {
//				if (ImGui::Button("Delete")) {
//					for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
//					{
//						m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second].life_span = 0;
//					}
//					if (m_selected_cluster_objects.size() == 2) {
//						m_current_joint_ptr = nullptr;//nullfy the current joints data since we just deleted it
//					}
//					m_selected_cluster_objects.clear();
//				}
//			}
//		}
//	}
//	ImGui::End();
//	//file explore dialog
//	{
//		//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
//		if (m_show_open_level_file_dialog)
//			ImGui::OpenPopup("Open Level File");
//		if (m_show_save_level_file_dialog)
//			ImGui::OpenPopup("Save Level File");
//		if (m_show_open_texture_file_dialog)
//			ImGui::OpenPopup("Open Texture File");
//
//		if (m_show_open_tile_sheet_file_dialog)
//			ImGui::OpenPopup("Open TileSheet File");
//
//		/* Optional third parameter. Support opening only compressed rar/zip files.
//		 * Opening any other file will show error, return false and won't close the dialog.
//		 */
//		if (m_file_dialog.showFileDialog("Open Level File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_level_file_extension))//".txt,.dat"
//		{
//			std::cout << m_file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
//			std::cout << m_file_dialog.selected_path << std::endl;    // The absolute path to the selected file
//			std::cout << m_file_dialog.ext << std::endl;    // The absolute path to the selected file
//
//			if (m_file_dialog.ext == m_level_file_extension)
//			{
//				SBX_INFO("Loading {} Level", m_level_file_extension);
//				//clean out the current world
//				for (size_t i = 0; i < m_world_clusters.size(); i++)
//				{
//					if (m_world_clusters[i].cluster_objects.size() > 0) {
//						for (size_t j = 0; j < m_world_clusters[i].cluster_objects.size(); j++)
//						{
//							SquareBox::GWOM::ClusterObject & ccobj = m_world_clusters[i].cluster_objects[j];
//							if (m_world_clusters[i].cluster_objects[j].physics_properties != nullptr &&m_world_clusters[i].cluster_objects[j].physics_properties->isIntialised()) {
//								m_world_clusters[i].cluster_objects[j].physics_properties->dispose();
//								m_world_clusters[i].cluster_objects[j].physics_properties = nullptr;
//							}
//						}
//					}
//					else {
//						m_world_clusters[i].cluster_objects.clear();
//					}
//				}
//				m_world_clusters.clear();
//				m_alive_cluster_objects.clear();
//				m_selected_cluster_objects.clear();
//				m_tile_sheet_textures.clear();
//				m_single_textures.clear();
//				m_current_active_world_cluster_index = 0;
//				m_current_active_cluster_object_index = 0;
//				m_auto_assign_world_cluster = true;
//				m_respect_ancestor = false;
//				/*we need a temp WorldCluster vec because create ClusterObject handles adding new cluster objects
//				to the currentActiveWorld by its self , so we have to do some logic to make sure we are were are always in the cluster_object array
//				*/
//				m_level_reader_writer.loadLevelDataAsBinary(m_file_dialog.selected_path, m_world_clusters);
//
//				for (unsigned int i = 0; i < m_world_clusters.size(); i++)
//				{
//					m_current_active_world_cluster_index = i;
//					m_current_world_cluster_ptr = &m_world_clusters[i];
//					if (m_world_clusters[i].cluster_objects.size() > 0)
//					{
//						printf("THIS WORLD HAS %d cluster Objects \n", m_world_clusters[i].cluster_objects.size());
//						int numWorldClusterObjects = m_world_clusters[i].cluster_objects.size();//so that its not baised as we add new cwcobj
//						for (unsigned int j = 0; j < numWorldClusterObjects; j++)
//						{
//							m_current_active_cluster_object_index = j;
//							bool lastClusterObjectMemberinLastWorldCluster = (i == m_world_clusters.size() - 1) && (j == numWorldClusterObjects - 1) ? true : false;
//							SquareBox::GWOM::ClusterObject* cobj = &m_world_clusters[i].cluster_objects[j];
//							m_utilities.setCurrentShapePointer(cobj->shape, &m_current_shape_ptr);
//							if (cobj->texture_type == 0) {
//								//first confirm that we dont have this texture already
//								bool isAlreadyPresent = false;
//
//								for (size_t k = 0; k < m_single_textures.size(); k++)
//								{
//									if (m_single_textures[k].asset_file_path == cobj->texture.asset_file_path) {
//										isAlreadyPresent = true;
//										cobj->texture_index = k;
//										break;
//									}
//								}
//								if (cobj->texture.asset_file_path != "") {
//									m_texture = SquareBox::AssetManager::TextureManager::getTexture(cobj->texture.asset_file_path);
//									m_texture.asset_file_path = cobj->texture.asset_file_path;
//									m_texture.display_name = cobj->texture.display_name;
//								}
//								else {
//									m_texture = SquareBox::AssetManager::GLTexture();
//								}
//
//								cobj->texture = m_texture;
//
//								if (!isAlreadyPresent) {
//									m_single_textures.push_back(m_texture);
//									cobj->texture_index = m_single_textures.size() - 1;
//								}
//							}
//							else if (cobj->texture_type == 1) {
//								//first confirm that we dont have this texture already
//								bool isAlreadyPresent = false;
//
//								for (unsigned int k = 0; k < m_tile_sheet_textures.size(); k++)
//								{
//									if (m_tile_sheet_textures[k].asset_file_path == cobj->texture.asset_file_path) {
//										isAlreadyPresent = true;
//										cobj->texture_index = k;
//										break;
//									}
//								}
//
//								if (cobj->texture.asset_file_path != "") {
//									m_texture = SquareBox::AssetManager::TextureManager::getTexture(cobj->texture.asset_file_path);
//									m_texture.asset_file_path = cobj->texture.asset_file_path;
//									m_texture.display_name = cobj->texture.display_name;
//								}
//								else {
//									m_texture = SquareBox::AssetManager::GLTexture();
//								}
//								cobj->texture = m_texture;
//								if (!isAlreadyPresent) {
//									m_tile_sheet_textures.push_back(m_texture);
//									cobj->texture_index = m_tile_sheet_textures.size() - 1;
//								}
//							}
//
//							m_current_cluster_object_ptr = cobj;
//
//							if (!lastClusterObjectMemberinLastWorldCluster) {
//								if (m_current_shape_ptr->is_plotted) {
//									printf("Create a ploted Shape Called with %d vertices and index id %d \n", cobj->vertices.size(), cobj->index);
//									SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//									//add to m_alive_cluster_objects Objects if its alive
//									if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//										if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//											m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//										}
//										else {
//											SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//										}
//									}
//								}
//								else
//								{
//									SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//									//add to m_alive_cluster_objects Objects if its alive
//									if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//										if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//											m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//										}
//										else {
//											SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//										}
//									}
//								}
//							}
//							else {
//								if (m_current_shape_ptr->is_plotted) {
//									printf("Create a ploted Shape Called with %d vertices and index id %d \n", cobj->vertices.size(), cobj->index);
//									SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, true, false);
//									//add to m_alive_cluster_objects Objects if its alive
//									if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//										if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//											m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//										}
//										else {
//											SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//										}
//									}
//									//get a pointer to the shell
//									if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
//										m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
//										m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
//										m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
//									}
//									else {
//										SBX_ERROR("Missing shell!!");
//										__debugbreak();
//									}
//								}
//								else
//								{
//									SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, true, false);
//									//add to m_alive_cluster_objects Objects if its alive
//									if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
//										if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
//											m_utilities.addPairToVector(m_alive_cluster_objects, details.settlementCoordinates);
//										}
//										else {
//											SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
//										}
//									}
//									//get a pointer to the shell
//									if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
//										m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
//										m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
//										m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
//									}
//									else {
//										SBX_ERROR("Missing shell!!");
//										__debugbreak();
//									}
//								}
//
//								updateStates();
//							}
//						}
//					}
//				}
//
//				SBX_INFO("Creating Joints");
//				m_utilities.createWorldClusterJoints(m_world_clusters, m_physics_world);
//				SBX_INFO("Finished Creating Joints");
//				SBX_INFO("Loaded {} Level", m_level_file_extension);
//			}
//			else {
//				SBX_ERROR("Un Supported File format");
//			}
//		}
//		if (m_file_dialog.showFileDialog("Open Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png"))
//		{
//			m_texture = SquareBox::AssetManager::TextureManager::getTexture(m_file_dialog.selected_path);
//			m_texture.asset_file_path = m_file_dialog.selected_path;
//
//			m_texture.display_name = m_file_dialog.selected_fn + m_file_dialog.ext;
//			//m_single_textures.push_back(m_texture);
//			//first confirm that we don't have this texture already
//			bool isAlreadyPresent = false;
//
//			for (size_t i = 0; i < m_single_textures.size(); i++)
//			{
//				if (m_single_textures[i].asset_file_path == m_texture.asset_file_path) {
//					isAlreadyPresent = true;
//					break;
//				}
//			}
//
//			if (!isAlreadyPresent) {
//				m_single_textures.push_back(m_texture);
//			}
//		}
//		if (m_file_dialog.showFileDialog("Open TileSheet File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png"))
//		{
//			m_texture = SquareBox::AssetManager::TextureManager::getTexture(m_file_dialog.selected_path);
//			m_texture.asset_file_path = m_file_dialog.selected_path;
//			m_texture.display_name = m_file_dialog.selected_fn + m_file_dialog.ext;
//			//first confirm that we don't have this texture already
//			bool isAlreadyPresent = false;
//
//			for (size_t i = 0; i < m_tile_sheet_textures.size(); i++)
//			{
//				if (m_tile_sheet_textures[i].asset_file_path == m_texture.asset_file_path) {
//					isAlreadyPresent = true;
//					break;
//				}
//			}
//
//			if (!isAlreadyPresent) {
//				m_tile_sheet_textures.push_back(m_texture);
//			}
//		}
//		if (m_file_dialog.showFileDialog("Save Level File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_level_file_extension))
//		{
//			std::cout << m_file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
//			std::cout << m_file_dialog.selected_path << std::endl;    // The absolute path to the selected file
//			std::cout << m_file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)
//
//			if (m_file_dialog.ext == m_level_file_extension) {
//				SBX_INFO("Saving Level as {} File", m_level_file_extension);
//				//eliminate all the file extensions just incase the user included an extra file extension
//				//e.g level.txt.txt
//				const std::string ext(m_level_file_extension);
//				std::string m_file_path = m_file_dialog.selected_path;
//				while (m_file_path.find(m_level_file_extension) != std::string::npos) {
//					// Check if the last three characters match the ext.
//					if (m_file_path != ext &&
//						m_file_path.size() > ext.size() &&
//						m_file_path.substr(m_file_path.size() - ext.size()) == m_level_file_extension)
//					{
//						// if so then strip them off
//						m_file_path = m_file_path.substr(0, m_file_path.size() - ext.size());
//					}
//				}
//				m_alive_cluster_objects.clear();
//				m_selected_cluster_objects.clear();
//				m_utilities.worldIndiciesCleanUp(m_world_clusters);
//				if (m_level_reader_writer.saveLevelDataAsBinary(m_file_path + m_file_dialog.ext, m_world_clusters))
//				{
//					SBX_INFO("Level Saved");
//				}
//				else {
//					SBX_WARN("Failed To Save");
//				}
//				//repopulate the m_alive_cluster_objects
//				for (unsigned int i = 0; i < m_world_clusters.size(); i++)
//				{
//					for (unsigned int j = 0; j < m_world_clusters[i].cluster_objects.size(); j++) {
//						SquareBox::GWOM::ClusterObject & ccobj = m_world_clusters[i].cluster_objects[j];
//						if (ccobj.is_alive) {
//							m_alive_cluster_objects.push_back(std::pair(ccobj.cluster_index, ccobj.index));
//						}
//					}
//				}
//				//return back , the m_currentObject
//				if (m_auto_assign_world_cluster) {
//					m_current_active_world_cluster_index = m_world_clusters.size() - 1;
//					m_current_world_cluster_ptr = &m_world_clusters[m_current_active_world_cluster_index];
//					m_current_world_cluster_ptr->cluster_objects.emplace_back();
//					int	last_index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
//					m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects[last_index];
//					updateStates();
//					m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
//					m_current_cluster_object_ptr->index = last_index;
//					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//				}
//				else {
//					m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.emplace_back();
//					int	last_index = m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.size() - 1;
//					m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects[last_index];
//					updateStates();
//					m_current_cluster_object_ptr->cluster_index = m_world_clusters[m_mannually_choosen_world_cluster_index].index;
//					m_current_cluster_object_ptr->index = last_index;
//					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
//				}
//			}
//			else {
//				SBX_ERROR("Un Supported File Format");
//			}
//		}
//
//		//reset of all dialogs
//		m_show_open_level_file_dialog = false;
//		m_show_save_level_file_dialog = false;
//		m_show_open_texture_file_dialog = false;
//		m_show_open_tile_sheet_file_dialog = false;
//	}
//
//	// Rendering
//	ImGui::EndFrame();
//	ImGui::Render();
//	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}
//
//void LevelEditor_Screen::updateStates()
//{
//	//updates all states to show the values of the currently active/selected object so that it doesnt loss its values
//	//by taking up the ones of its predessors
//
//	//this function should be called each time we change objects if we want to use the expected new state
//
//	if (m_current_cluster_object_ptr != nullptr) {
//		//the shape
//		m_selected_shape = static_cast<int>(m_current_cluster_object_ptr->shape);
//
//		//the body Type
//		m_selected_body_type = static_cast<int>(m_current_cluster_object_ptr->type);
//
//		if (m_current_cluster_object_ptr->physics_properties == nullptr) {
//			m_enable_physics = false;
//		}
//		else
//		{
//			m_enable_physics = true;
//		}
//
//		if (m_current_cluster_object_ptr->texture_type == 0) {
//			//Singles
//			m_selected_single_texture_index = m_current_cluster_object_ptr->texture_index;
//		}
//		else {
//			//sheets
//			m_selected_tile_sheet_texture_index = m_current_cluster_object_ptr->texture_index;
//		}
//
//		//the family
//		m_selected_family = static_cast<int>(m_current_cluster_object_ptr->family);
//
//		/* When we have multiple cameras , we shall also add there variable here */
//	}
//}
//
//void LevelEditor_Screen::checkPhysicsStatus()
//{
//	if (m_current_cluster_object_ptr != nullptr) {
//		if (m_enable_physics) {
//			if (m_current_cluster_object_ptr->physics_properties == nullptr) {
//				if (m_place_mode) {
//					//the object had not yet been created just give it physics back
//					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
//				}
//				else if (m_select_mode) {
//					//the object had already been created !!!
//					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
//					//recreate it into the physics world
//					if (m_current_shape_ptr->is_plotted)
//					{
//						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//					}
//					else
//					{
//						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
//					}
//				}
//			}
//		}
//		else if (!m_enable_physics) {
//			//if this object has a physics object attacted to it delete it
//			if (m_current_cluster_object_ptr->physics_properties != nullptr) {
//				m_utilities.destroryClusterObjectJoints(m_world_clusters, *m_current_cluster_object_ptr);
//				m_current_cluster_object_ptr->physics_properties->dispose();
//				//free up the memory
//				delete m_current_cluster_object_ptr->physics_properties;
//				m_current_cluster_object_ptr->physics_properties = nullptr;
//			}
//		}
//	}
//}