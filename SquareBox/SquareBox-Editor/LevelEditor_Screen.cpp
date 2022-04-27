#include "LevelEditor_Screen.h"
#include "ScreenIndices.h"
#include<utility>


std::vector<std::string> m_fresh_logs;
int last_line_count = 0;
void onFolderModify(std::string file_path_) {
		std::ifstream logFileRead(file_path_);
		std::string my_text;
		int current_line_count = 0;
		while (std::getline(logFileRead, my_text))
		{
			if (current_line_count >= last_line_count) {
				m_fresh_logs.push_back(my_text);
				last_line_count++;
			}
			current_line_count++;
		}
		logFileRead.close();
}

char *convertGLTextureDisplayNameToCstr(const SquareBox::GWOM::ParentTexture& parentTexture_)
{
	//this new here is never deleted, and is called alot, that is a problem to us 
	char *pc = new char[parentTexture_.texture.display_name.size() + 1];
	std::strcpy(pc, parentTexture_.texture.display_name.c_str());
	return pc;
}

SquareBoxEditor::LevelEditor_Screen::LevelEditor_Screen(SquareBox::RenderEngine::Window* window)
	:m_window(window)
{
}

void SquareBoxEditor::LevelEditor_Screen::build()
{
	SBX_INFO("LevelEditor Screen Loaded into memory");
}

void SquareBoxEditor::LevelEditor_Screen::onEntry() {
	m_sprite_font.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);
	m_sprite_batch.init();
	m_debug_renderer.init();
	m_physics_world.init();
	m_utilities.init();
	m_animation_creator.init();
	m_animation_creator.loadAnimationScript(m_animation_script);
	m_automation.init();
	m_automation.loadAutomationScript(m_automation_script);

	//Texture Program
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();

	//Complie our light shader
	m_light_program.compileShaders("Assets/Shaders/lightShading.vert", "Assets/Shaders/lightShading.frag");
	m_light_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
	m_light_program.addAttribute("vertexColor");
	m_light_program.addAttribute("vertexUV");
	m_light_program.linkShaders();

	//the debugProgram
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertexColor");
	m_debug_program.linkShaders();


	//Clear out the world
	m_world_clusters.clear();

	//Will reserve a few world clusters here in the name of cache friendlyness
	m_world_clusters.reserve(60);

	//first WorldCluster
	m_world_clusters.emplace_back();
	m_current_world_cluster_ptr = &m_world_clusters.back();
	m_current_world_cluster_ptr->index = m_world_clusters.size() - 1;
	m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);

	//First Cluster object
	m_current_world_cluster_ptr->cluster_objects.emplace_back();
	m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects.back();
	m_current_cluster_object_ptr->index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
	m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
	m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);

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

	//lets start off with atleast one layer
	//First layer is created mannually
	m_layers.emplace_back();
	//get a reference to the last placed layer
	SquareBox::GWOM::Layer& layer = m_layers.back();
	layer.alive_cluster_objects.clear();
	//some cache friendlness
	layer.alive_cluster_objects.reserve(60);
	layer.single_textures.reserve(5);
	layer.tiled_textures.reserve(10);
	layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	layer.camera.setScale(m_universal_camera_scale);
	layer.camera.setPosition(m_universal_camera_position);
	layer.is_visible = true;
	layer.is_locked = false;
	m_active_layer_index = 0;

	updateStates();

	initGUI();
	/*This is currently giving ,me a thread execption , TO be debugged later*/
	//std::thread my_thread_object(FolderChangeTracker(),"test", onFolderModify);
	//my_thread_object.detach();
}

void SquareBoxEditor::LevelEditor_Screen::update(const float & deltaTime_)
{
	m_universal_camera_scale = m_layers[m_active_layer_index].camera.getScale();
	m_universal_camera_position = m_layers[m_active_layer_index].camera.getPosition();

	for (unsigned int i = 0; i < m_layers.size(); i++)
	{
		if (i != m_active_layer_index) {
			/* mainiting the same properties ensures that we have the same selectable world*/
			m_layers[i].camera.setScale(m_universal_camera_scale);
			m_layers[i].camera.setPosition(m_universal_camera_position);
		}
	}

	//update all camera
	for (unsigned int i = 0; i < m_layers.size(); i++)
	{
		m_layers[i].camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	}


	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
	{
		SDL_Event evnt;
		//stop our game engine from processing input
		m_game_ptr->setProcessingInput(false);
		while (SDL_PollEvent(&evnt))
		{
			//hand over all the events to imGUI
			ImGui_ImplSDL2_ProcessEvent(&evnt);
		}
	}
	else {
		m_game_ptr->setProcessingInput(true);
	}

	SquareBox::GWOM::Layer& active_layer = m_layers[m_active_layer_index];

	/*  The way we handle a tiled layer will be different from the way we handle a none Tiled layer that is relying
		on the WorldCluster Objects vector
	*/

	if (active_layer.tile_system.getTiling() != SquareBox::LayerTilingEnum::None && active_layer.tile_system.isInitialised()) {
		if (m_game_ptr->isProcessingInput()) {
			//where the mouse currently is at in our world
			glm::vec2& mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
            
			if (m_select_mode) {
				//figure out if we are in the tile system
				if (active_layer.tile_system.isInTileSystem(mouse_in_world)) {

					//get the tile being hovered
					if (m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
						SquareBox::GWOM::Tile* selected_tile = active_layer.tile_system.getTile(mouse_in_world);
						
						if (m_tiled_layer_selection_mode_index == TiledLayerSelectModeEnum::SINGLETILE) {
							m_selected_tiles.clear();
							m_selected_tiles.push_back(selected_tile);
						}
						else if (m_tiled_layer_selection_mode_index == TiledLayerSelectModeEnum::MULTIPLETILES) {
							m_selected_tiles.push_back(selected_tile);
						}
					}
				}

			}
			else if (m_place_mode) {
				if (active_layer.tile_system.isInTileSystem(mouse_in_world)) {
					SquareBox::GWOM::Tile* hovered_tile = active_layer.tile_system.getTile(mouse_in_world);
					if (m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::RIGHT_CLICK)) {
						//we have to edit the hovered tile and give it a texture to display
						hovered_tile->key = m_active_sub_texture_key;
						m_is_all_work_saved = false;
					}
				}
			}
		}
	}
	else {
		//	m_layers[m_active_layer_index].tiling == SquareBox::LayerTilingEnum::None
		std::vector<std::pair<int, int>> active_layer_alive_cluster_objects = active_layer.alive_cluster_objects;

		/*
		 these operations are only carried out when we are processing input ,(The mouse is in our world space)
		 please take note of that
		 If an operation does not depend on us processing input then it should join its similar friends below this if
		 //statement
		*/
		if (m_game_ptr->isProcessingInput()) {
	
			//where the mouse currently is at in our world
			glm::vec2& mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);

			if (m_select_mode) {
				bool hoveringOverCluster = false;
				bool hoveringOverClusterObject = false;
				for (unsigned int i = 0; i < active_layer_alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[active_layer_alive_cluster_objects[i].first].cluster_objects[active_layer_alive_cluster_objects[i].second];
					m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
					// is being hovered and doesn't belong to a camera which is closed
					if (m_current_shape_ptr->containtsPoint(ccobj, mouse_in_world)) {
						hoveringOverCluster = true;
						hoveringOverClusterObject = true;
						m_current_hovered_world_cluster_index = active_layer_alive_cluster_objects[i].first;
						m_current_hovered_cluster_object_index = active_layer_alive_cluster_objects[i].second;
					}
				}



				if (m_selection_mode_index == SelectModeEnum::WORLDCLUSTER) {
					//if we are hovering over a cluster object and we Left click let that objects' whole cluster become selected in our world scene
					if (hoveringOverClusterObject && m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
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
						clustermousepos = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
					}
				}
				else if (m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) {
					//Cluster object Selection
					//this is the only place where we actually set the active worldClusters and Objects
					//if we are hovering over a cluster object and we Left click let that object become the current active object in our world scene
					if (hoveringOverClusterObject && m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
						m_current_world_cluster_ptr = &m_world_clusters[m_current_hovered_world_cluster_index];
						m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects[m_current_hovered_cluster_object_index];
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
					std::pair<int, int> m_current_cluster_object_coordinates = m_editor_assitant.currentClusterObjectDupper(m_physics_world, m_world_clusters, active_layer_alive_cluster_objects, m_selected_cluster_objects, m_current_cluster_object_ptr, m_current_shape_ptr, m_game_ptr, m_utilities);
					if (m_current_cluster_object_coordinates.first != -1 && m_current_cluster_object_coordinates.second != -1) { //Why would this be (-1,-1) in the first case !!!! i need to better comment this 
						m_current_cluster_object_ptr = &m_world_clusters[m_current_cluster_object_coordinates.first].cluster_objects[m_current_cluster_object_coordinates.second];
					}
				}
				else if (m_selection_mode_index == SelectModeEnum::FREESELECT) {
					//Free selection
					//if we are hovering over a cluster object and we Left click add this cluster object to the list of selected cluster objects
					if (hoveringOverClusterObject && m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
						m_utilities.addPairToVector(m_selected_cluster_objects, std::pair<int, int>(m_current_hovered_world_cluster_index, m_current_hovered_cluster_object_index));

						/*
							keep track of where the mouse was in the world cluster when we selected this
							cluster object , we shall use this while dragging the world cluster later
						*/
						clustermousepos = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
					}

					//drag select
					if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::SAPCE) && m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::MouseEnum::RIGHT_CLICK)) {
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
								SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[active_layer_alive_cluster_objects[i].first].cluster_objects[active_layer_alive_cluster_objects[i].second];
								if (!ccobj.is_hidden && m_utilities.isInBox(ccobj.position, drag_select_destRect)) {
									m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(active_layer_alive_cluster_objects[i].first, active_layer_alive_cluster_objects[i].second));
								}
							}
						}
						/*
							keep track of where the mouse was in the world cluster when we selected this
							cluster object , we shall use this while dragging the world cluster later
						*/
						clustermousepos = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
					}
					else {
						m_is_drag_selecting = false;
					}
				}
				else if (m_selection_mode_index == SelectModeEnum::JOINTSSELECT) {
					//Joint Creation
					//only two objects can be selected at once
					//and only one joint can exit between two bodies
					if (hoveringOverClusterObject && m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
						//clicked on object
						SquareBox::GWOM::ClusterObject& clicked_on_object = m_world_clusters[m_current_hovered_world_cluster_index].cluster_objects[m_current_hovered_cluster_object_index];
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
				}
				if (m_selection_mode_index != SelectModeEnum::FREESELECT) {
					m_is_drag_selecting = false;
				}

				//dragging implemetation and copying implementations
					if (m_selection_mode_index == SelectModeEnum::WORLDCLUSTER || m_selection_mode_index == SelectModeEnum::FREESELECT && !(m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::SAPCE))) {
						//dont drag and drag select at the same time

						if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::MouseEnum::RIGHT_CLICK))
						{
							float diplacementinX = mouse_in_world.x - clustermousepos.x;
							float diplacementinY = mouse_in_world.y - clustermousepos.y;
							for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
							{
								SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
								m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
								updateStates();/*having this here is irrelvant because we dont really call to the varibales in this time
											   frame at all ,maybe after when we have finished this loop */

											   //we should do the proper displacement at this point

								if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
									for (unsigned int j = 0; j < ccobj.vertices.size(); j++)
									{
										ccobj.vertices[j].x += diplacementinX;
										ccobj.vertices[j].y += diplacementinY;
									}
									//need to do the logic for displacing edges,chains and ploygons and bridges since
									//we want to displace the vertices based on the mouses motion
									m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
									m_is_all_work_saved = false;
								}
								else
								{
									ccobj.position.x += diplacementinX;
									ccobj.position.y += diplacementinY;
									m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
									m_is_all_work_saved = false;
								}
							}
							clustermousepos = mouse_in_world;
							m_current_shape_ptr = nullptr;
						}

						//copying with Ctrl-C implementation ,
						//Ctrl-V is not yet implemented thou, nno need for that
						if (m_selected_cluster_objects.size() > 0 && (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_CTRL) || m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::RIGHT_CTRL)) && m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::KeyBoardEnum::KEY_c))
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
									SquareBox::GWOM::ClusterObject copy_of_original_ccobj = m_world_clusters[copy_of_selected_objects[0].first].cluster_objects[copy_of_selected_objects[0].second];

									copy_of_original_ccobj.is_alive = false;
									copy_of_original_ccobj.life_span = 10;
									//TODO :
									//joints are not copied , i will implement this later
									copy_of_original_ccobj.joints.clear();
									//change its index 
									copy_of_original_ccobj.index = m_world_clusters[world_cluster_index].cluster_objects.size();
									// rename it
									m_utilities.nameClusterObjectByIndex(copy_of_original_ccobj);

									//add the copy to the worldCLusters
									m_world_clusters[world_cluster_index].cluster_objects.push_back(copy_of_original_ccobj);

									// get a referenece to it 
									new_ccobj = &m_world_clusters[world_cluster_index].cluster_objects.back();

								}
								else {
									//depend on the shell we got 
									if (got_shell_coordinates.first >= 0 && got_shell_coordinates.second >= 0) {
										//copy properties
										m_world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second] = m_world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second];
										//point to our object
										new_ccobj = &m_world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second];
										//restore the cluster_index && index
										new_ccobj->cluster_index = got_shell_coordinates.first;
										new_ccobj->index = got_shell_coordinates.second;
										//restore the default name
										m_utilities.nameClusterObjectByIndex(*new_ccobj);
										new_ccobj->is_alive = false;
										new_ccobj->life_span = 10;
										//joints are not copied , i will implement this later
										new_ccobj->joints.clear();
									}
									else {

										SBX_ERROR("Invalid shell coordinates");
										__debugbreak();
									}
								}

								//The actual creatioin into the world starts here
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
									details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), new_ccobj, m_world_clusters, desired_shell_world_cluster_index, &m_physics_world, create_shell, false);
								}
								else {
									details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), new_ccobj, m_world_clusters, desired_shell_world_cluster_index, &m_physics_world, create_shell, false);
								}
								if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
									m_utilities.addPairToVector(active_layer_alive_cluster_objects, details.settlementCoordinates);
									m_utilities.addPairToVector(m_selected_cluster_objects, details.settlementCoordinates);
								}
								else {
									SBX_ERROR("We failed to get back settelment coordinates when copying a world cluster ");
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
							if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::MouseEnum::RIGHT_CLICK))
							{
								m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);

								//we are trusting our program to always make the selected object the current object ,which it does perfectly
								if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
									//need to do the logic for displacing edges,chains and ploygons and bridges since
									//we want to displace the vertices basinf on the mouses motion
									m_utilities.createClusterObjectIntoWorld(true, true, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
									m_is_all_work_saved = false;
								}
								else
								{
									m_utilities.createClusterObjectIntoWorld(false, true, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
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
			else if (m_place_mode) {
				if (m_game_ptr->getInputManager()->isInputIdReceived((int)SquareBox::MouseEnum::RIGHT_CLICK)) {


						//place plotting points for the ploted shapes , and draw the other objects
						SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, true, mouse_in_world, m_current_cluster_object_ptr, m_world_clusters, m_auto_assign_world_cluster ? m_current_active_world_cluster_index : m_mannually_choosen_world_cluster_index, &m_physics_world, true, m_respect_ancestor);
						//add to m_alive_cluster_objects Objects if its alive
						if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
							if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
								m_utilities.addPairToVector(m_layers[m_active_layer_index].alive_cluster_objects, details.settlementCoordinates);
								m_is_all_work_saved = false;
							}
							else {
								SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
							}
						}
						//get a pointer to the shell
						if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
							m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
							m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
							m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
						}

				}
			}
		}
		else {
			//when imgui is the one doing the processing
			//keeping up with the physics
			checkPhysicsStatus();//to keep track when we toggle an objects Physics abilities
			//this maybe better off when moved into the physics toggling block
			if (m_place_mode) {
				if (m_draw_plotted_shape) {
					SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_auto_assign_world_cluster ? m_current_active_world_cluster_index : m_mannually_choosen_world_cluster_index, &m_physics_world, true, m_respect_ancestor);
					//add to m_alive_cluster_objects Objects if its alive
					if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
						if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
							m_utilities.addPairToVector(active_layer_alive_cluster_objects, details.settlementCoordinates);
							m_is_all_work_saved = false;
						}
						else {
							SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
						}
					}
					//get a pointer to the shell
					if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
						m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
						m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
						m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
						if (m_respect_ancestor) {
							checkPhysicsStatus();
						}
					}
					else {
						SBX_ERROR("Missing shell!!");
						__debugbreak();
					}

					m_draw_plotted_shape = false;
				}
			}
		}

		//done regardless of whats doing processing

		bool deleted_a_cluster_object = false;
		// deleting objects

		for (std::vector<std::pair<int, int>>::iterator it = m_layers[m_active_layer_index].alive_cluster_objects.begin(); it != m_layers[m_active_layer_index].alive_cluster_objects.end();)
		{
			bool erased = false;
			SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[(*it).first].cluster_objects[(*it).second];

			//Destory those that have expired
			if (ccobj.life_span < 1) {
				for (unsigned int j = 0; j < ccobj.joints.size(); j++)
				{
					SquareBox::GWOM::Joint* joint = ccobj.joints[j];
					SquareBox::GWOM::ClusterObject& jointBodyA = m_world_clusters[joint->body_a_coordinates.first].cluster_objects[joint->body_a_coordinates.second];
					SquareBox::GWOM::ClusterObject& jointBodyB = m_world_clusters[joint->body_b_coordinates.first].cluster_objects[joint->body_b_coordinates.second];
					m_utilities.destroyClusterObjectJoint(jointBodyA, jointBodyB);
				}

				it = m_layers[m_active_layer_index].alive_cluster_objects.erase(it);
				erased = true;
				deleted_a_cluster_object = true;
				ccobj.is_alive = false;
				if (ccobj.physics_properties != nullptr) {
					ccobj.physics_properties->dispose();
					ccobj.physics_properties = nullptr;
				}

				m_current_cluster_object_ptr = nullptr;
			}
			if (!erased) {
				++it;
			}
		}

		if (deleted_a_cluster_object) {
			m_selected_cluster_objects.clear();
		}
		//run update loop for our world objects
		if (m_play_simulation) {
			m_physics_world.update();//update our physics
			for each (auto & layer in m_layers) {
				m_automation.update(m_world_clusters, layer.alive_cluster_objects, deltaTime_);
				m_animation_creator.update(1.0f, m_world_clusters, layer.alive_cluster_objects, m_game_ptr->getFps(), m_game_ptr->getGameLoopElapsedSeconds(), m_game_ptr->getInputManager());
			}
		}

		//usefull key board short cuts
		if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::LEFT_SHIFT) && m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEY_c)) {
			//Clear selected
			if (m_select_mode && m_selection_mode_index != SelectModeEnum::CLUSTEROBJECT) {
				m_selected_cluster_objects.clear();
				m_current_joint_ptr = nullptr;//Safety precaution in joint select mode
				//would be nice to also be able to do this for cluster object mode.
			}
		}

		if (m_game_ptr->getInputManager()->isInputIdBeingReceived((int)SquareBox::KeyBoardEnum::KEYBOARD_DELETE)) {
			//Delete selected
			if (m_select_mode) {
				for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
				{
					m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second].life_span = 0;
				}
				m_selected_cluster_objects.clear();
				m_current_joint_ptr = nullptr;//Safety precaution in joint select mode
			}
		}
	}


	if (m_game_ptr->isProcessingInput()) {
		//controling the active layers camera in our world
		m_editor_assitant.cameraControls(m_layers[m_active_layer_index].camera, m_game_ptr);
	}
	else {

	}

	/*these operations are carried out regardless of what is doing the processing , game or imgui*/
	//keeping track of the clusterObjects positions in the world
	//important because we have physics engine that edits positions and angles, and yet we dont save the Physics engine in our file
	SquareBox::IShape * orignalShapePointer = m_current_shape_ptr;

	for each (auto  & layer in m_layers) {

		/*	should only do this for none tiled layers
			because tiled layers can have up to millions of objects and doing this will slow us down
			.. this updating position will also mess up for pixel perfect grid system
			and in tilled layers , we are no really expecting the position of an object to change ,for now (flatTiling)
		*/
		if (layer.tile_system.getTiling() == SquareBox::LayerTilingEnum::None) {
			for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
				m_utilities.setCurrentShapePointer(ccobj.shape, &m_current_shape_ptr);
				m_current_shape_ptr->keepTrackOfPosition(ccobj);//Must be run before our automation.update

				//recreate objects that requested recreation, mainly through the SquareBox::AI
				if (ccobj.requesting_recreation) {
					/* Requested recreation does on take into account the current mouse position */
					if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else {
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), &ccobj, m_world_clusters, ccobj.cluster_index, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					ccobj.requesting_recreation = false;
				}
			}
		}
	}

	m_current_shape_ptr = orignalShapePointer;

	if (m_physics_world.isLocked()) {
		m_show_world_locked_dialog = true;
	}
	else {
		m_show_world_locked_dialog = false;
	}
}

void SquareBoxEditor::LevelEditor_Screen::draw() {
	//will draw layer by layer starting with the first one
	int layer_index = 0;
	for (unsigned int h = 0; h < m_layers.size();h++) {
		SquareBox::GWOM::Layer& layer = m_layers[h];
		if (layer.is_visible) {
			//get the vector of alive cluster objects to draw
			if (layer.tile_system.getTiling() != SquareBox::LayerTilingEnum::None && layer.tile_system.isInitialised()) {
			//fix up the draw calls
			glm::vec2 camera_center = layer.camera.getPosition();
			float camera_width = layer.camera.getCameraDimensions().x;
			float camera_height = layer.camera.getCameraDimensions().y;
			glm::vec4 camera_destRect(camera_center - glm::vec2(camera_width, camera_height)*0.5f, glm::vec2(camera_width, camera_height));
				auto vector_of_visible_tiles = layer.tile_system.getAllTilesInBox(camera_destRect);
				//TEXTURES
				m_sprite_batch.begin(SquareBox::RenderEngine::NONE);//helps us respect the order we already have predifined in the m_alive_cluster_objects class
				for (auto it = vector_of_visible_tiles.begin(); it != vector_of_visible_tiles.end(); it++)
				{
					if ((*it).second->key != -1) {
						//its set so we can draw it
						glm::vec4 texture_uvRect(0.0f, 0.0f, 1.0f, 1.0f);
						int texture_id = 0;

						std::map<int,SquareBox::GWOM::SubTexture>::iterator sub_texture_it = layer.sub_textures_table.find((*it).second->key);
						if (sub_texture_it == layer.sub_textures_table.end()) {
							//We didnt find this texture key
							//SBX_ERROR("{} not found in sub textures table", (*it).second->key);
						}
						else {
							const auto & target_sub_texture =  (*sub_texture_it).second;

							if (target_sub_texture.parent_type == SquareBox::TextureEnum::SINGLE) {
								texture_id = layer.single_textures[target_sub_texture.parent_texture_index].texture.id;
							}else if(target_sub_texture.parent_type == SquareBox::TextureEnum::TILESHEET){
								texture_id = layer.tiled_textures[target_sub_texture.parent_texture_index].texture.id;
								//Get the uvCoordinates
								SquareBox::AssetManager::TileSheet tile_sheet;
								tile_sheet.readGrid(layer.tiled_textures[target_sub_texture.parent_texture_index].texture);
								texture_uvRect = tile_sheet.getUVCords(target_sub_texture.tiling_index);
							}

						}
						//we need ot figure these out using the key that this tile has 
						m_sprite_batch.draw(glm::vec4((*it).second->position, glm::vec2(layer.tile_system.getTileSize())), texture_uvRect,texture_id,1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
					}
					}
					
					
				m_sprite_batch.end();

				m_texture_program.use();
				preUpdateShader(&m_texture_program, "mySampler");
				uploadCameraInfo(&m_texture_program, &layer.camera, "P");
				m_sprite_batch.renderBatch();
				m_texture_program.unuse();


				//DEBUG LINES
				m_debug_renderer.begin();
				if (m_debug_mode) {
				for (auto it = vector_of_visible_tiles.begin(); it != vector_of_visible_tiles.end(); it++)
				{
					m_debug_renderer.drawBox(glm::vec4((*it).second->position, glm::vec2(layer.tile_system.getTileSize())), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);
				}
				}
				m_debug_renderer.end();
				//render all our debug shapes
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &layer.camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();
			}
			else {
				std::vector<std::pair<int, int>> vector_of_alive_cluster_objects = layer.alive_cluster_objects;
				m_debug_renderer.begin();
				//PULLEY JOINTS ROPES
				for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];
					if (cwobj.joints.size() > 0) {
						for (unsigned int j = 0; j < cwobj.joints.size(); j++)
						{
							if (cwobj.joints[j]->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
								//draw the pulley ropes

								SquareBox::GWOM::ClusterObject& jointObjectA = m_world_clusters[cwobj.joints[j]->body_a_coordinates.first].cluster_objects[cwobj.joints[j]->body_a_coordinates.second];
								SquareBox::GWOM::ClusterObject& jointObjectB = m_world_clusters[cwobj.joints[j]->body_b_coordinates.first].cluster_objects[cwobj.joints[j]->body_b_coordinates.second];

								//check if we are dealing with physics bodies , which should always be the case for joints 
								if (jointObjectA.physics_properties != nullptr && jointObjectB.physics_properties != nullptr && cwobj.joints[j]->vec_of_points.size() == 2) {
									m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
									m_debug_renderer.drawLine(cwobj.joints[j]->vec_of_points[0], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectA, cwobj.joints[j]->local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.joints[j]->color.x), (255 * cwobj.joints[j]->color.y), (255 * cwobj.joints[j]->color.z), (255 * cwobj.joints[j]->color.w))));
									m_debug_renderer.drawLine(cwobj.joints[j]->vec_of_points[1], m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointObjectB, cwobj.joints[j]->local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.joints[j]->color.x), (255 * cwobj.joints[j]->color.y), (255 * cwobj.joints[j]->color.z), (255 * cwobj.joints[j]->color.w))));
								}
								else {
									SBX_ERROR("Error while trying to draw pulley Joint ropes");
								}
							}
						}
					}
				}
				m_debug_renderer.end();

				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &layer.camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();


				//TEXTURES
				m_sprite_batch.begin(SquareBox::RenderEngine::NONE);//helps us respect the order we already have predifined in the m_alive_cluster_objects class

				for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

					m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
					//lights are drawn using the light render later
					if (m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::PointLightShape && m_current_shape_ptr->body_shape != SquareBox::BodyShapeEnum::CalculatedLightShape) {
						/*
							we should only be drawing the clusterObjects that are in the cameras view
							where we can, these will safe resources
						*/

						if (m_current_shape_ptr->is_plotted && cwobj.shape == SquareBox::BodyShapeEnum::Edge) {
							if (m_always_draw_edges) {
								if (cwobj.vertices.size() > 1) {
									m_debug_renderer.begin();
									m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, SquareBox::RenderEngine::ColorRGBA8((255 * cwobj.color.x), (255 * cwobj.color.y), (255 * cwobj.color.z), (255 * cwobj.color.w) * (0.01 * layer.opacity)));
									m_debug_renderer.end();
									m_debug_program.use();
									uploadCameraInfo(&m_debug_program, &layer.camera, "P");
									m_debug_renderer.render();
									m_debug_program.unuse();
								}
							}
						}
						else if (m_current_shape_ptr->needs_height_width) {
							if (layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.width, cwobj.height))) {
								m_current_shape_ptr->draw(cwobj, m_sprite_batch, layer.opacity);
							}
						}
						else if (m_current_shape_ptr->needs_radius) {
							if (layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
								m_current_shape_ptr->draw(cwobj, m_sprite_batch, layer.opacity);
							}
						}
						else
						{
							SBX_ERROR("Unidentified shape found for {} ", cwobj.name);
						}
					}
				}

				m_sprite_batch.end();

				m_texture_program.use();
				preUpdateShader(&m_texture_program, "mySampler");
				uploadCameraInfo(&m_texture_program, &layer.camera, "P");
				m_sprite_batch.renderBatch();
				m_texture_program.unuse();


				//DEBUG LINES
				m_debug_renderer.begin();
				if (m_debug_mode) {

					// Draw axis

					// +X axis
					glm::vec2 debug_center = layer.camera.getPosition();
					float camera_width = layer.camera.getCameraDimensions().x;
					float camera_height = layer.camera.getCameraDimensions().y;

					//+X
					m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x + (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 200));
					// -X axis
					m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x - (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 100));

					// +Y axis
					m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y + (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 200));
					// -Y axis
					m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y - (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 100));


					//draw all debug lines for objects on this layer
					//if m_show_all_debug_lines is set to true



					if (m_show_all_debug_lines) {
						for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
						{
							SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

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
								m_border_color = m_unknown_body_color;
							}



							//show debug edges
							m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
							m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, m_border_color);
							//show the first and last vertices
							if (m_current_shape_ptr->is_plotted) {
								m_utilities.showPlottedVertices(true, m_debug_renderer, cwobj, 1.0f / layer.camera.getScale());
							}

						}
					}
					else if (layer_index == m_active_layer_index) {
						//only show for the active layer
						for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
						{
							SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

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
							m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, m_border_color);
							//show the first and last vertices
							if (m_current_shape_ptr->is_plotted) {
								m_utilities.showPlottedVertices(true, m_debug_renderer, cwobj, 1.0f / layer.camera.getScale());
							}

						}
					}

				}

				m_debug_renderer.end();
				//render all our debug shapes
				m_debug_program.use();
				uploadCameraInfo(&m_debug_program, &layer.camera, "P");
				m_debug_renderer.render();
				m_debug_program.unuse();


				//LIGHTS
				m_window->useAdditiveBlending();

				m_sprite_batch.begin();
				for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

					m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);

					if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::PointLightShape) {
						if (m_current_shape_ptr->needs_radius) {
							if (layer.camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
								m_current_shape_ptr->draw(cwobj, m_sprite_batch);
							}
						}
					}
					else if (m_current_shape_ptr->body_shape == SquareBox::BodyShapeEnum::CalculatedLightShape) {
						m_current_shape_ptr->draw(cwobj, m_sprite_batch);
					}
				}
				m_sprite_batch.end();

				m_light_program.use();
				uploadCameraInfo(&m_light_program, &layer.camera, "P");
				m_sprite_batch.renderBatch();
				m_light_program.unuse();

				m_window->useAlphaBlending();
			}

		}
		layer_index++;
	}


	m_debug_renderer.begin();
	//place ment should be handled with the active layer

	//draw the place mode sketch
	if (m_place_mode) {
	

		glm::vec2 mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
		
		if (m_layers[m_active_layer_index].tile_system.getTiling() != SquareBox::LayerTilingEnum::None && m_layers[m_active_layer_index].tile_system.isInitialised()) {
			//Draw a view of the tile below us
			if (m_layers[m_active_layer_index].tile_system.isInTileSystem(mouse_in_world)) {
				auto hovered_tile = m_layers[m_active_layer_index].tile_system.getTile(mouse_in_world);
				m_debug_renderer.drawBox(glm::vec4(hovered_tile->position, glm::vec2(m_layers[m_active_layer_index].tile_system.getTileSize())), m_place_mode_sketch_color, 0.0f);
			}
		}
		else {
			m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
			m_current_shape_ptr->traceDraw(*m_current_cluster_object_ptr, m_debug_renderer, mouse_in_world, m_place_mode_sketch_color, m_layers[m_active_layer_index].camera.getScale());
		}
	}
	else if (m_select_mode) {
		if (m_layers[m_active_layer_index].tile_system.getTiling() != SquareBox::LayerTilingEnum::None && m_layers[m_active_layer_index].tile_system.isInitialised()) {
			for (auto it = m_selected_tiles.begin(); it != m_selected_tiles.end(); it++)
			{
				m_debug_renderer.drawBox(glm::vec4((*it)->position, glm::vec2(m_layers[m_active_layer_index].tile_system.getTileSize())), m_selected_body_color, 0.0f);
			}
		}
		else {
			//drawing the selected objects debug lines
			//are we among the selected Objects
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
				m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, m_selected_body_color);
			}
			glm::vec2 mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);

			//are we drag selecting
			if (m_is_drag_selecting) {
				//construct the destRect 
				glm::vec4 drag_select_destRect(m_drag_select_origin, (mouse_in_world - m_drag_select_origin));
				m_debug_renderer.drawBox(drag_select_destRect, m_drag_selection_box_color, 0.0f);
			}
			//drawing the scaling lines
			if ((m_selected_cluster_objects.size() == 1) && m_show_scaling_in_debug_mode && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) {//dont show in freelance mode
				SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[m_selected_cluster_objects[0].first].cluster_objects[m_selected_cluster_objects[0].second];
				m_current_shape_ptr->scalingDraw(cwobj, m_debug_renderer, mouse_in_world, m_game_ptr->getInputManager());
			}
			//for Joint Creation selection Mode
			//ploting the plotted points
			for (unsigned int i = 0; i < m_vector_of_joint_ploting_points.size(); i++)
			{
				m_debug_renderer.drawCircle(m_vector_of_joint_ploting_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_layers[m_active_layer_index].camera.getScale());
			}

			if (m_current_joint_ptr != nullptr) {
				SquareBox::GWOM::ClusterObject& jointBodyA = m_world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second];
				SquareBox::GWOM::ClusterObject& jointBodyB = m_world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second];

				m_utilities.setCurrentShapePointer(jointBodyA.shape, &m_current_shape_ptr);
				m_debug_renderer.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyA, m_current_joint_ptr->local_anchor_a), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_layers[m_active_layer_index].camera.getScale());

				m_utilities.setCurrentShapePointer(jointBodyB.shape, &m_current_shape_ptr);
				m_debug_renderer.drawCircle(m_current_shape_ptr->localCoordinatesToWorldCoordinates(jointBodyB, m_current_joint_ptr->local_anchor_b), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_layers[m_active_layer_index].camera.getScale());

				if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
					for (unsigned int i = 0; i < m_current_joint_ptr->vec_of_points.size(); i++)
					{
						m_debug_renderer.drawCircle(m_current_joint_ptr->vec_of_points[i], SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 1.0f / m_layers[m_active_layer_index].camera.getScale());
					}
				}
			}
		}
	}
	m_debug_renderer.end();
	m_debug_program.use();
	uploadCameraInfo(&m_debug_program, &m_layers[m_active_layer_index].camera, "P");
	m_debug_renderer.render();
	m_debug_program.unuse();

	drawGUI();
}

void SquareBoxEditor::LevelEditor_Screen::onExit()
{
	if (!m_allow_exit)
	{
		m_game_ptr->abortExit();//abort normal exit and first see request for permission to exit
		m_request_exit = true;//this will lead to the are u sure dialog being displayed
	}
	else {
		//this line must be called by the programmer inorder to permit the game to exit
		// for  a game to exit the programmer must have allowed it to
		m_sprite_font.dispose();
		m_sprite_batch.dispose();
		m_utilities.dispose();
		m_texture_program.dispose();
		//m_automation.dispose();
		//m_animatioCreator.dispose();
		for each (auto & layer in m_layers) {
			if (layer.tile_system.getTiling() == SquareBox::LayerTilingEnum::None) {
				for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
					if (cwobj->physics_properties != nullptr) {
						cwobj->physics_properties->dispose();
						delete cwobj->physics_properties; //we should always free up this memory with delete
						cwobj->physics_properties = nullptr;
					}
				}
			}
			
		}
		
		//m_world_clusters.clear();
		/*
		calling clear here is causing a memory leak because this is infact a vector of heap memories
		the solution will be to make the m_worldCluster vector a vector of unique_pointers which will free
		them selves automatically
		*/
		/*	for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
			{
				delete m_vec_shapes_pointer[i];
			}*/
		m_physics_world.destroy();
		m_game_ptr->initiateExit();
	}
}

void SquareBoxEditor::LevelEditor_Screen::destroy()
{
	SBX_INFO("LevelEditor Screen Cleared out of memory");
}

int SquareBoxEditor::LevelEditor_Screen::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int SquareBoxEditor::LevelEditor_Screen::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void SquareBoxEditor::LevelEditor_Screen::createNewLayer()
{
	//push back a new layer
	m_layers.emplace_back();
	//get a reference to the last placed layer
	SquareBox::GWOM::Layer& layer = m_layers.back();
	layer.alive_cluster_objects.clear();
	layer.alive_cluster_objects.reserve(60);//some cache friendlness
	layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	layer.camera.setScale(m_layers[m_active_layer_index].camera.getScale());
	layer.camera.setPosition(m_layers[m_active_layer_index].camera.getPosition());
	layer.is_visible = true;
	layer.is_locked = false;

	//t this layer accoriding to its index
	std::stringstream ss;
	int layer_index = m_layers.size() - 1;
	
	ss << "Layer ";
	ss << layer_index;

	std::string nameString = ss.str();

	for (unsigned int si = 0; si < nameString.size(); si++)
	{
		layer.name[si] = nameString[si];
	}
	layer.name[nameString.size()] = '\0';
	
}

void SquareBoxEditor::LevelEditor_Screen::createNewProject()
{
	m_current_active_world_cluster_index = 0;
	m_current_active_cluster_object_index = 0;
	m_selected_single_texture_index = 0;

	m_selected_cluster_objects.clear();

	//clear out all layers
	for each (auto & layer in m_layers) {
		if (layer.tile_system.getTiling() == SquareBox::LayerTilingEnum::None) {
			for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
				if (cwobj->physics_properties != nullptr) {
					cwobj->physics_properties->dispose();
					delete cwobj->physics_properties; //we should always free up this memory with delete
					cwobj->physics_properties = nullptr;
				}
			}
		}
	
	}
	m_layers.clear();

	//First layer is created mannually
		
	//push back a new layer

	m_layers.emplace_back();
	//get a reference to the last placed layer
	SquareBox::GWOM::Layer& layer = m_layers.back();
	layer.alive_cluster_objects.clear();
	layer.alive_cluster_objects.reserve(60);//some cache friendlness
	layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	layer.camera.setScale(m_universal_camera_scale);
	layer.camera.setPosition(m_universal_camera_position);
	layer.is_visible = true;
	layer.is_locked = false;

	m_active_layer_index = 0;//since its our only layer as per now
	//Clear out the world
	m_world_clusters.clear();

	//Wil reserve a few world clusters here to make stuff faster
	m_world_clusters.reserve(60);

	//first WorldCluster
	m_world_clusters.emplace_back();
	m_current_world_cluster_ptr = &m_world_clusters.back();
	m_current_world_cluster_ptr->index = m_world_clusters.size() - 1;
	m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);

	//First Cluster object
	m_current_world_cluster_ptr->cluster_objects.emplace_back();
	m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects.back();
	m_current_cluster_object_ptr->index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
	m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
	m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
}

void SquareBoxEditor::LevelEditor_Screen::initGUI()
{
#ifdef __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
#endif

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGuiContext* IMGC = ImGui::CreateContext();
	ImGuiIO io = ImGui::GetIO(); (void)io;
	ImGui::SetCurrentContext(IMGC);

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_window->getWindowPointerForImGui(), m_window->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_INFO("Failed to load font Roboto-Medium.ttf ");
	}
	font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/chintzy.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_INFO("Failed to load font chintzy.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void SquareBoxEditor::LevelEditor_Screen::showMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Respect Ancestor", NULL, m_respect_ancestor)) {
				m_respect_ancestor = !m_respect_ancestor;
			}
			if (ImGui::MenuItem("Draw Edges", NULL, m_always_draw_edges)) {
				m_always_draw_edges = !m_always_draw_edges;
			}
			if (ImGui::MenuItem("Show scaling lines", NULL, m_show_scaling_in_debug_mode)) {
				m_show_scaling_in_debug_mode = !m_show_scaling_in_debug_mode;
			}	
			if (ImGui::MenuItem("Show grid lines", NULL, m_show_grid_lines)) {
				m_show_grid_lines = !m_show_grid_lines;
			}
			if (ImGui::MenuItem("Show All Debug Lines", NULL, m_show_all_debug_lines)) {
				m_show_all_debug_lines = !m_show_all_debug_lines;
			}
			if (ImGui::MenuItem("Show Console", NULL, m_show_console)) {
				m_show_console = !m_show_console;
			}
			if (ImGui::MenuItem("Auto Assign WorldCluster", NULL, m_auto_assign_world_cluster)) {
				m_auto_assign_world_cluster = !m_auto_assign_world_cluster;
				if (!m_auto_assign_world_cluster) {
					m_mannually_choosen_world_cluster_index = m_current_active_world_cluster_index;
					m_show_set_mannual_wc_index_dialog = true;
				}
			}
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				m_show_open_level_file_dialog = true;
			}
			if (m_place_mode) {
				// we can only save and  open new levels in place mode
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					m_show_save_level_file_dialog = true;
				}
				if (ImGui::MenuItem("New")) { m_show_create_new_project_dialog = true; }
			}
			if (ImGui::BeginMenu("Open Recent"))
			{
				ImGui::MenuItem("null.sbx",NULL,false,false);
				ImGui::MenuItem("sample.sbx",NULL, false, false);
				ImGui::MenuItem("hello.dat",NULL, false, false);
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("About", NULL)) { m_show_about_dialog = true; }
			if (ImGui::MenuItem("Quit", NULL)) { m_request_exit = true; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", NULL,false,false)) {}
			if (ImGui::MenuItem("Redo", NULL, false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", NULL,false,false)) {}
			if (ImGui::MenuItem("Copy", NULL,false,false)) {}
			if (ImGui::MenuItem("Paste", NULL,false,false)) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Extra Settings"))
		{
			if (ImGui::BeginMenu("Theme")) {
				if (ImGui::MenuItem("Classic Theme")) {
					m_theme_index = EditorThemeEnum::ClassicTheme;
				}
				if (ImGui::MenuItem("Dark Theme")) {
					m_theme_index = EditorThemeEnum::DarkTheme;
				}
				if (ImGui::MenuItem("Light Theme")) {
					m_theme_index = EditorThemeEnum::LightTheme;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("KeyBoard Bindings")) { m_show_key_bindings_dialog = true; };
			ImGui::EndMenu();
		}
		if (ImGui::Button("Reload Animations Script"))
		{
			m_animation_creator.loadAnimationScript(m_animation_script);
		}

		if (ImGui::Button("Reload Automation Script"))
		{
			m_automation.loadAutomationScript(m_automation_script);
		}

		ImGui::EndMainMenuBar();
	}

	// Setup Dear ImGui style
	if (m_theme_index == EditorThemeEnum::ClassicTheme) {
		ImGui::StyleColorsClassic();
		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(127, 127, 127, 127));
	}
	else if (m_theme_index == EditorThemeEnum::LightTheme) {
		ImGui::StyleColorsLight();
		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255));
	}
	else if (m_theme_index == EditorThemeEnum::DarkTheme) {
		ImGui::StyleColorsDark();
		m_window->setBackGroundColor(SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255));
	}
}

void SquareBoxEditor::LevelEditor_Screen::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window->getWindowPointerForImGui());
	ImGui::NewFrame();
	static bool m_show_demo_window = true;
	ImGui::ShowDemoWindow(&m_show_demo_window);
	showMainMenu();
	ImGuiWindowFlags properties_tab_window_flags = 0;
	properties_tab_window_flags |= ImGuiWindowFlags_NoMove;
	if (m_show_console) {
		properties_tab_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}
	bool* properties_tab__open = false;
	ImGui::Begin("Properties Tab", properties_tab__open, properties_tab_window_flags);
	ImGui::SetWindowPos(ImVec2(0, 20));
	
	if (m_show_console) {
		auto console_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), m_window->getScreenHeight()*0.75);
		ImGui::SetWindowSize(console_size);
	}
	else {
		auto console_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), static_cast<float>(m_window->getScreenHeight()));
		ImGui::SetWindowSize(console_size);
	}

	{
		if (m_request_exit) {
			ImGui::OpenPopup("CloseLevelEditor?");
		}
		if (m_show_set_mannual_wc_index_dialog) {
			ImGui::OpenPopup("AutoAssignWorldCluster?");
		}
		if (m_show_shift_cobjs_to_another_layer_dialog) {
			ImGui::OpenPopup("ShiftCobjsToAnotherLayer?");
		}
		if (m_show_world_locked_dialog) {
			ImGui::OpenPopup("WorldLocked");
		}

		if (m_show_about_dialog) {
			ImGui::OpenPopup("About?");
		}

		if (m_show_key_bindings_dialog) {
			ImGui::OpenPopup("KeyBindings");
		}

		if (m_show_failed_to_save_world_dialog) {
			ImGui::OpenPopup("FailedToSaveWorld?");
		}

		if (m_show_create_new_project_dialog) {
			ImGui::OpenPopup("CreateNewProject");
		}

		if (m_show_update_layer_tiling_dialog) {
			ImGui::OpenPopup("UpdateTilingDialog");
		}
		//Close Editor Tab
				// Always center this window when appearing
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("CloseLevelEditor?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (!m_is_all_work_saved) {
				ImGui::Text("Warning : Your progress is not fully saved!!.\nAll un saved work will be deleted.This operation can not be reversed.\n\n");
				ImGui::Separator();
			}
			else {
				ImGui::Text("Do you want to close the editor ?");
				ImGui::Separator();
			}
			static bool dont_ask_me_next_time = false;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
			ImGui::PopStyleVar();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				//Close  LevelEditor
				m_allow_exit = true;
				onExit();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				m_request_exit = false;
			}
			ImGui::EndPopup();
		}
		
		if (ImGui::BeginPopupModal("UpdateTilingDialog", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (m_layers[m_active_layer_index].alive_cluster_objects.size() > 0) {
				ImGui::Text("Only an empty layers grid can be updated");
				/* this is because gid objects are made to exaclty feed the grid cells*/
				if (ImGui::Button("Ok", ImVec2(120, 0))) {
					m_show_update_layer_tiling_dialog = false;
					ImGui::CloseCurrentPopup();
				}
			}
			else {
				static float layer_width = 0.0f;
				static float layer_height = 0.0f;
				static float layer_tile_size = 0.0f;
				ImGui::Text("Layer Width   :"); ImGui::SameLine();
				ImGui::InputFloat("###layerwidth", &layer_width);

				ImGui::Text("Layer Height  :"); ImGui::SameLine();
				ImGui::InputFloat("###layerheight", &layer_height);

				float layer_x_origin = 0 - layer_width * 0.5f;
				float layer_y_origin = 0 - layer_height * 0.5f;
				ImGui::Text("Layer Origin  :  %.3f  ,%.3f", layer_x_origin, layer_y_origin);

				static int world_tiling = static_cast<int>(m_layers[m_active_layer_index].tile_system.getTiling());
				const char* world_styling_styles[] = { ToString(SquareBox::LayerTilingEnum::FlatTiling),ToString(SquareBox::LayerTilingEnum::IsoMetricTiling),ToString(SquareBox::LayerTilingEnum::None) };
				ImGui::Text("Layer Tiling  :"); ImGui::SameLine();
				ImGui::Combo("###layertiling", &world_tiling, world_styling_styles, 3);
				
				if (static_cast<SquareBox::LayerTilingEnum>(world_tiling) != SquareBox::LayerTilingEnum::None) {
					ImGui::Text("Tile Size     :"); ImGui::SameLine();
					ImGui::InputFloat("###layertileszie", &layer_tile_size);
				}

				if (ImGui::Button("Ok", ImVec2(120, 0))) {
					if (static_cast<SquareBox::LayerTilingEnum>(world_tiling) != SquareBox::LayerTilingEnum::None) {
						m_layers[m_active_layer_index].tile_system.setTiling(static_cast<SquareBox::LayerTilingEnum>(world_tiling));
						m_layers[m_active_layer_index].tile_system.init(layer_x_origin, layer_y_origin, layer_width, layer_height, layer_tile_size);
					}
					else {
						m_layers[m_active_layer_index].tile_system.setTiling(static_cast<SquareBox::LayerTilingEnum>(world_tiling));
						m_layers[m_active_layer_index].tile_system.resetTileSystem();
					}
					m_show_update_layer_tiling_dialog = false;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Cancel", ImVec2(120, 0))) {
					m_show_update_layer_tiling_dialog = false;
					ImGui::CloseCurrentPopup();
				}
			}
				
				ImGui::EndPopup();
			}

		if (ImGui::BeginPopupModal("CreateNewProject", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (!m_place_mode) {
				ImGui::Text("New projects can only be created in place mode ");
				ImGui::Separator();
				if (ImGui::Button("Ok", ImVec2(120, 0))) {
					m_show_create_new_project_dialog = false;
					ImGui::CloseCurrentPopup();
				}
			}
			else if (m_place_mode)
			{
				if (!m_is_all_work_saved) {
					ImGui::Text("Not all  progress has been saved. Continuing will discard unsaved Changes");
					ImGui::Separator();

					if (ImGui::Button("Continue", ImVec2(120, 0))) {
						m_is_all_work_saved = true;
						createNewProject();
						m_show_create_new_project_dialog = false;
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::Button("Cancel", ImVec2(120, 0))) {
						m_show_create_new_project_dialog = false;
						ImGui::CloseCurrentPopup();
					}
				}
				else {
					ImGui::Text("Create New Project ? ");
					ImGui::Separator();
					if (ImGui::Button("Ok", ImVec2(120, 0))) {
						createNewProject();
						m_show_create_new_project_dialog = false;
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("ABOUT:");
			ImGui::BulletText("This is a 2D game world Editing tool");
			ImGui::BulletText("Built and maintained by stande studios ");
			ImGui::BulletText("Established 2021");

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_about_dialog = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("FailedToSaveWorld?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Failed to save world!\nCheck Logs for more information");

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_failed_to_save_world_dialog = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("KeyBindings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("PLACE MODE");
	  ImGui::BulletText("Mouse Left-Click              :  Place new cluster object into World");

			ImGui::Separator();
			ImGui::Text("Camera Controls");
			ImGui::Text("LEFT_ALT + LEFT_CLICK         :  Camera Panning");
			ImGui::Text("LEFT_ALT + mouse wheel        :  Camera Zoom");
			ImGui::Text("LEFT_CTRL + mouse wheel       :  Camera movement in x direction");
			ImGui::Text("mouse wheel                   :  Camera movement in y direction");
			
			ImGui::Separator();
			ImGui::Text("SELECT MODE");
			ImGui::Text("\nCluster Mode");
			ImGui::Text("LEFT_CLICK                    :  makes hovered objects world cluster get selected");

			ImGui::Text("\nObject Mode");
			ImGui::Text("LEFT_CLICK                    :  makes hovered object active object");
			ImGui::Text("TAB + ARROW KEYS              :  dupilcates the selected cluster object in the arrows' direction");

			ImGui::Text("\nFree Select Mode");
			ImGui::Text("LEFT_CLICK                    :  adds hovered object to selected objects");
			ImGui::Text("SPACE_BAR + RIGHT_CLICK       :  creates selection rectangle");

		ImGui::Text("\n\nRIGHT_CLICK                   :  drags selected objects");
			
			ImGui::Text("\nJoint Mode:");
			ImGui::Text("Still under development");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_key_bindings_dialog = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("AutoAssignWorldCluster?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Must be in the range of the WorldCluster vector size()\n");
			ImGui::Separator();
			ImGui::DragInt("WorldClusterIndex", &m_mannually_choosen_world_cluster_index, 1, 0, m_world_clusters.size(), "%d%", ImGuiSliderFlags_AlwaysClamp);

			if (m_mannually_choosen_world_cluster_index > static_cast<int>(m_world_clusters.size() - 1)) {
				m_mannually_choosen_world_cluster_index = m_world_clusters.size() - 1;
			}

			
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_set_mannual_wc_index_dialog = false;
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
		if (ImGui::BeginPopupModal("ShiftCobjsToAnotherLayer?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Shifting cluster Objects\nFrom: %s \n",m_layers[m_active_layer_index].name);
			ImGui::Separator();
			
			ImGui::DragInt("New Layer index", &m_target_shift_to_layer_index, 1, 0, m_layers.size(), "%d%", ImGuiSliderFlags_AlwaysClamp);

			if (m_target_shift_to_layer_index > static_cast<int>(m_layers.size() - 1)) {
				//force stuff to go to the last layer instead
				m_target_shift_to_layer_index = m_layers.size() - 1;
			}
			ImGui::Text("To: %s", m_layers[m_target_shift_to_layer_index].name);
			

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				m_show_shift_cobjs_to_another_layer_dialog = false;
				if (m_target_shift_to_layer_index == m_active_layer_index) {
					/*
					we are still going with the flow here because it gives the user a way to move a bunch of
					objects to the back of the layers draw calls
					*/
				}


				/* currently we can only select from the active layer , so we assume that all layers are
				in the currently active  layer*/

				//remove all the selected cluster objects from the active layer
				for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
					m_utilities.removePairFromVector(m_layers[m_active_layer_index].alive_cluster_objects, m_selected_cluster_objects[i]);
				}
				//add to the target layer
				for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
					m_utilities.addPairToVector(m_layers[m_target_shift_to_layer_index].alive_cluster_objects, m_selected_cluster_objects[i]);
				}

				/* make the layer that has aquired the new m_items the target layer in reference to maintaing
				the only objects that can be selected are the ones on the active layer rule*/
				m_active_layer_index = m_target_shift_to_layer_index;

				/* to change this default setting then this would be the case
				m_selected_cluster_objects.clear();

				// this will maintain the active layer to what it is 
				*/
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
				m_allow_exit = true;
				onExit();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			ImGui::EndPopup();
		}
		float camera_scale = m_layers[m_active_layer_index].camera.getScale();
		ImGui::DragFloat("Zoom", &camera_scale, 1, 0, 2000, "%.3f");
		for (unsigned int i = 0; i < m_layers.size(); i++)
		{
			/* mainiting the same scale throught  ensures that we have the same scaling in all worlds */
			m_layers[i].camera.setScale(camera_scale);
		}
		if (m_place_mode) {
			if (ImGui::Button("New Cluster")) {
				m_auto_assign_world_cluster = true;//we are lossing control
				m_world_clusters.emplace_back();
				m_current_active_world_cluster_index = m_world_clusters.size() - 1;
				m_current_world_cluster_ptr = &m_world_clusters[m_current_active_world_cluster_index];
				m_current_world_cluster_ptr->index = m_current_active_world_cluster_index;
				m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);

				m_current_world_cluster_ptr->cluster_objects.emplace_back();

				m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_world_cluster_ptr->cluster_objects.size() - 1];
				updateStates();
				m_current_cluster_object_ptr->cluster_index = m_world_clusters[m_current_active_world_cluster_index].index;
				m_current_cluster_object_ptr->index = 0;
				m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
			}
		}
		else if (m_select_mode) {
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

			if (m_current_world_cluster_ptr != nullptr) {
				if (m_auto_assign_world_cluster) {
					ImGui::InputText("###CurrentWorldClustersName", m_current_world_cluster_ptr->name, 20);
				}
				else {
					ImGui::InputText("###CurrentWorldClustersName", m_world_clusters[m_mannually_choosen_world_cluster_index].name, 20);
				}
			}
			else {
				ImGui::Text(" m_current_world_cluster_ptr is null");//i still dont know why this is null some times
			}

			ImGui::Text("Object name");
			ImGui::InputText("###CurrentClusterObjectsName", m_current_cluster_object_ptr->name, 20);
			ImGui::Separator();
		}
		ImGui::Text("Mode");

		static int e = 0;
		int beforeMode = e;
		ImGui::RadioButton("Place Mode", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Select Mode", &e, 1); ImGui::SameLine();
		ImGui::Checkbox("Debug Mode", &m_debug_mode);

		if (e == 0) {
			m_place_mode = true;
			m_select_mode = false;

			m_selected_cluster_objects.clear();
			if (m_auto_assign_world_cluster) {
				m_current_active_cluster_object_index = m_world_clusters[m_current_active_world_cluster_index].cluster_objects.size() - 1;
				m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_active_cluster_object_index];
				if (m_current_cluster_object_ptr->is_alive) {
					/*
						this is true for when we have done dupping , because there the last ClusterObjects in this world cluster
						is the dup we just placed into the world
					*/
					m_world_clusters[m_current_active_world_cluster_index].cluster_objects.emplace_back();

					m_current_active_cluster_object_index = m_world_clusters[m_current_active_world_cluster_index].cluster_objects.size() - 1;
					m_current_cluster_object_ptr = &m_world_clusters[m_current_active_world_cluster_index].cluster_objects[m_current_active_cluster_object_index];
					//give it tis coordinates
					m_current_cluster_object_ptr->cluster_index = m_current_active_world_cluster_index;
					m_current_cluster_object_ptr->index = m_current_active_cluster_object_index;
					//name it
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				}
				updateStates();
			}
			else {
				m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.back();

				if (m_current_cluster_object_ptr->is_alive) {
					/*
					this is true for when we have done dupping , because there the last ClusterObjects in this world cluster
					is the dup we just placed there

					For cases where we are not from dupping , due to our auto new shell creation mechanisim we always leave behind an 
					empty shell when we create a new world cluster , and its empty that we jump back to when m_auto_assign_world_cluster
					is disbaled 

					*/
					m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.emplace_back();

					const int coindex = m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.size() - 1;
					m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects[coindex];
					//give it tis coordinates
					m_current_cluster_object_ptr->cluster_index = m_mannually_choosen_world_cluster_index;
					m_current_cluster_object_ptr->index = coindex;
					//name it
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				}
				updateStates();
			}
			m_current_world_cluster_ptr = &m_world_clusters[m_current_cluster_object_ptr->cluster_index];
		}

		else if (e == 1) {
			m_place_mode = false;
			m_select_mode = true;
			if (beforeMode != 1) {
				m_current_cluster_object_ptr = nullptr;
				m_current_world_cluster_ptr = nullptr;
			}
		}
		if (((m_select_mode&& m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT) || m_place_mode) && m_current_cluster_object_ptr != nullptr) {
			ImGui::Text("Color");
			static float col2[4];
			
			col2[0] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->color.x), static_cast<float>(255));
			col2[1] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->color.y), static_cast<float>(255));
			col2[2] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->color.z), static_cast<float>(255));
			col2[3] = SquareBox::MathLib::Float::divideAndGetFloat(static_cast<float>(m_current_cluster_object_ptr->color.w), static_cast<float>(255));

			ImGui::ColorEdit4("Clear color", col2);
			m_current_cluster_object_ptr->color.x = static_cast<int>(col2[0] * 255);
			m_current_cluster_object_ptr->color.y = static_cast<int>(col2[1] * 255);
			m_current_cluster_object_ptr->color.z = static_cast<int>(col2[2] * 255);
			m_current_cluster_object_ptr->color.w = static_cast<int>(col2[3] * 255);

			int shape_suject = static_cast<int>(m_place_mode_index);
			ImGui::RadioButton("Single Shape", &shape_suject, 0); ImGui::SameLine();
			ImGui::RadioButton("Compound Shape", &shape_suject, 1);
			m_place_mode_index = static_cast<ShapeTypeEnum>(shape_suject);
			if (m_place_mode_index == ShapeTypeEnum::SINGLESHAPE) {
				ImGui::Text("Object Shape");
				//Object Types

	
				//display our shapes names
				ImGui::Text("Shape     : "); ImGui::SameLine();
				if (m_layers[m_active_layer_index].tile_system.isInitialised()) {
					if (m_layers[m_active_layer_index].tile_system.getTiling() == SquareBox::LayerTilingEnum::FlatTiling) {
						ImGui::Text(ToString(SquareBox::BodyShapeEnum::Box));
						m_current_cluster_object_ptr->shape = SquareBox::BodyShapeEnum::Box;
						m_current_cluster_object_ptr->width = m_layers[m_active_layer_index].tile_system.getTileSize();
						m_current_cluster_object_ptr->height = m_layers[m_active_layer_index].tile_system.getTileSize();
						ImGui::Text("Height    : "); ImGui::SameLine();
						ImGui::Text("%.3f", m_layers[m_active_layer_index].tile_system.getTileSize());
						ImGui::Text("Width     : "); ImGui::SameLine();
						ImGui::Text("%.3f", m_layers[m_active_layer_index].tile_system.getTileSize());
					}
					else {
						ImGui::Text("UNdefined Tiling");
					}
				}
				else {
					const char** shapes_labels = new const char* [m_vec_shapes_pointer.size()];

					for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
					{
						shapes_labels[i] = ToString(m_vec_shapes_pointer[i]->body_shape);
					}
					ImGui::Combo("###Shape", &m_selected_shape, shapes_labels, m_vec_shapes_pointer.size());

					delete[] shapes_labels;

					//get our shapes dimensions

					for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
					{
						if (m_selected_shape == i) {
							ImGui::Text("Dimensions For "); ImGui::SameLine(); ImGui::Text(ToString(m_vec_shapes_pointer[i]->body_shape));
							ImGui::Separator();

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
												std::stringstream ss;
												//TODO find out how to empty the stringstream that willbe more efficent than recreating it each time we need it
												ss << "(";
												ss << m_current_cluster_object_ptr->vertices[i].x;
												ss << ",";
												ss << m_current_cluster_object_ptr->vertices[i].y;
												ss << ")";
												ss << " ";

												ss << "(";
												ss << m_current_cluster_object_ptr->vertices[i + 1].x;
												ss << ",";
												ss << m_current_cluster_object_ptr->vertices[i + 1].y;
												ss << ")";

												ImGui::Text(ss.str().c_str());
											}
										}
										else {
											for (int i = 0; i < static_cast<int>(m_current_cluster_object_ptr->vertices.size() - 2); i++)
											{
												std::stringstream ss;

												ss << "(";
												ss << m_current_cluster_object_ptr->vertices[i].x;
												ss << ",";
												ss << m_current_cluster_object_ptr->vertices[i].y;
												ss << ")";
												ss << " ";

												ss << "(";
												ss << m_current_cluster_object_ptr->vertices[i + 1].x;
												ss << ",";
												ss << m_current_cluster_object_ptr->vertices[i + 1].y;
												ss << ")";
												ImGui::Text(ss.str().c_str());
											}
											std::stringstream ss;

											ss << "(";
											ss << m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].x;
											ss << ",";
											ss << m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1].y;
											ss << ")";
											ss << "(? , ?)";
											ImGui::Text(ss.str().c_str());
										}
									}
								}

								if (m_current_shape_ptr->is_calculated_light) {
									if (ImGui::Button("Add Edge")) {
										if (static_cast<int>(m_current_cluster_object_ptr->vertices.size()) >= m_vec_shapes_pointer[i]->min_plotting_vertices)
										{
											const glm::vec2& start = m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 2];
											const glm::vec2& end = m_current_cluster_object_ptr->vertices[m_current_cluster_object_ptr->vertices.size() - 1];
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
									std::stringstream numToString;
									int min_points = m_current_cluster_object_ptr->shape == SquareBox::BodyShapeEnum::CalculatedLightShape ? 4 : m_vec_shapes_pointer[i]->min_plotting_vertices;
									numToString << min_points;
									ImGui::Text("A minimum of "); ImGui::SameLine(); ImGui::Text(numToString.str().c_str()); ImGui::SameLine(); ImGui::Text(" points is need\n");
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
				}


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
					vc.reserve(m_layers[m_active_layer_index].single_textures.size());
					std::transform(m_layers[m_active_layer_index].single_textures.begin(), m_layers[m_active_layer_index].single_textures.end(), std::back_inserter(vc), convertGLTextureDisplayNameToCstr);

					//compute how many single textures we want to display
					//we are only displaying those that are not empty strings
					if (m_layers[m_active_layer_index].single_textures.size() > 0) {
						ImGui::Combo("Single", &m_selected_single_texture_index, &vc[0], m_layers[m_active_layer_index].single_textures.size());
						//update the current objects texture info
						m_current_cluster_object_ptr->texture_info.texture_index = m_selected_single_texture_index;
						m_current_cluster_object_ptr->texture_info.texture_id = m_layers[m_active_layer_index].single_textures[m_selected_single_texture_index].texture.id;
						//current active texture
						ImTextureID my_tex_id;
						my_tex_id = (ImTextureID)m_layers[m_active_layer_index].single_textures[m_selected_single_texture_index].texture.id;

						float my_tex_w = 50;
						float my_tex_h = 50;
						float zoom = 4.0f;
						ImGui::Text("Preview");
						//get the uvCoords
						glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

						ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);                 // Top-left
						ImVec2 uv_max = ImVec2(uvRect.z, uvRect.w);                 // Lower-right
						ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
						ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
						ImGui::Image(my_tex_id, ImVec2(my_tex_w* zoom, my_tex_h* zoom), uv_min, uv_max, tint_col, border_col);
					
						if (m_layers[m_active_layer_index].tile_system.getTiling() != SquareBox::LayerTilingEnum::None && m_layers[m_active_layer_index].tile_system.isInitialised())
						{
							//set the m_active_sub_texture_key
							for (unsigned int i = 0; i < m_layers[m_active_layer_index].sub_textures_table.size(); i++) {
								const auto& sub_texture = m_layers[m_active_layer_index].sub_textures_table[i];
								if (sub_texture.parent_texture_index == m_layers[m_active_layer_index].single_textures[m_selected_single_texture_index].texture_index) {
									m_active_sub_texture_key = sub_texture.sub_texture_key;
								}
							}

						}
					
					}
					else {
						ImGui::Combo("Single", &m_selected_single_texture_index, "Empty", m_layers[m_active_layer_index].single_textures.size());
					}

					if (ImGui::Button("+ Single")) {
						m_show_open_texture_file_dialog = true;
					}
				}
				else if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::TILESHEET) {
					/* tiling changes will be refected on the stored texture , this will save us time
					by not having to redefine the tiling grid each time*/
					
					if (m_layers[m_active_layer_index].tiled_textures.size() > 0) {
						ImGui::Text("Tiling:");
						ImGui::Text("cols : "); ImGui::SameLine();
						ImGui::Text("%d", m_layers[m_active_layer_index].tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.x);
						ImGui::Text("rows : "); ImGui::SameLine();
						ImGui::Text("%d", m_layers[m_active_layer_index].tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.y);

						std::vector<char*>  vc;
						vc.reserve(m_layers[m_active_layer_index].tiled_textures.size());
						std::transform(m_layers[m_active_layer_index].tiled_textures.begin(), m_layers[m_active_layer_index].tiled_textures.end(), std::back_inserter(vc), convertGLTextureDisplayNameToCstr);

						ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], m_layers[m_active_layer_index].tiled_textures.size());
						//update the current objects texture info
						auto & selected_tile_sheet_texture = m_layers[m_active_layer_index].tiled_textures[m_selected_tile_sheet_texture_index].texture;
						m_current_cluster_object_ptr->texture_info.texture_index = m_selected_tile_sheet_texture_index;
						m_current_cluster_object_ptr->texture_info.texture_id = selected_tile_sheet_texture.id;
						//current active texture
						ImTextureID my_tex_id;
						float my_tex_w = 0.0f;
						float my_tex_h = 0.0f;
						my_tex_id = (ImTextureID)selected_tile_sheet_texture.id;
						if (selected_tile_sheet_texture.tiling.x<1 && selected_tile_sheet_texture.tiling.y<1) {
							//the tilesheet sheet hasn't yet been grided
							if (selected_tile_sheet_texture.width > selected_tile_sheet_texture.height) {
								my_tex_w = 75;
								my_tex_h = 30;
							}
							else {
								//give it a different aspect ratio
								my_tex_w = 55;
								my_tex_h = 100;
							}
						}
						else {
							my_tex_w = 50;
							my_tex_h = 50;
						}
						
					
						float zoom = 4.0f;
						ImGui::Text("Preview");
						//get the uvCoords
						SquareBox::AssetManager::TileSheet tileSheet;
						tileSheet.readGrid(selected_tile_sheet_texture);
						/*
						we update the cluster objects uv coords here .if it is a freelance cluster Object

						*/
						glm::vec4 uvRect = tileSheet.getUVCords(m_current_cluster_object_ptr->texture_info.tile_sheet_index);

						if (m_current_cluster_object_ptr->controller_type == SquareBox::ControlledTypeEnum::freelance) {
							m_current_cluster_object_ptr->texture_info.uv_rect = uvRect;
						}

						ImVec2 uv_min = ImVec2(uvRect.x, uvRect.y);                 // Top-left
						ImVec2 uv_max = ImVec2(uvRect.z, uvRect.w);                 // Lower-right
						ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
						ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
						ImGui::Image(my_tex_id, ImVec2(my_tex_w* zoom, my_tex_h* zoom), uv_min, uv_max, tint_col, border_col);
						ImGui::Text("Index: "); ImGui::SameLine();
						ImGui::InputInt("###m_current_tile_sheet_index", &m_current_cluster_object_ptr->texture_info.tile_sheet_index);
						if (m_current_cluster_object_ptr->texture_info.tile_sheet_index<0) {
							m_current_cluster_object_ptr->texture_info.tile_sheet_index = 0;
						}
						else if(m_current_cluster_object_ptr->texture_info.tile_sheet_index>=(selected_tile_sheet_texture.tiling.x * selected_tile_sheet_texture.tiling.y)) {
							m_current_cluster_object_ptr->texture_info.tile_sheet_index = (selected_tile_sheet_texture.tiling.x * selected_tile_sheet_texture.tiling.y) - 1;
						}

						if (m_layers[m_active_layer_index].tile_system.getTiling() != SquareBox::LayerTilingEnum::None && m_layers[m_active_layer_index].tile_system.isInitialised())
						{
							//set the m_active_sub_texture_key
							for (unsigned int i = 0; i < m_layers[m_active_layer_index].sub_textures_table.size(); i++) {
								const auto & sub_texture = m_layers[m_active_layer_index].sub_textures_table[i];
								if (sub_texture.parent_texture_index == m_layers[m_active_layer_index].tiled_textures[m_selected_tile_sheet_texture_index].texture_index) {
									//Check if this sub texture is the one
									if (sub_texture.tiling_index == m_current_cluster_object_ptr->texture_info.tile_sheet_index) {
										m_active_sub_texture_key = sub_texture.sub_texture_key;
									}
								}
							}

						}
					}
					else {
						ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, "Empty", m_layers[m_active_layer_index].tiled_textures.size());
					}
					ImVec2 posee = ImGui::GetCursorScreenPos();

					ImGui::Text("New  TileSheet Tiling:");
					ImGui::Text("cols : "); ImGui::SameLine();
					ImGui::InputInt("###cols", &m_new_tiled_texture_tiling.x);
					ImGui::Text("rows : "); ImGui::SameLine();
					ImGui::InputInt("###rows", &m_new_tiled_texture_tiling.y);

					if (ImGui::Button("+ TileSheet")) {
						m_show_open_tile_sheet_file_dialog = true;
					}
				}
			}
			else if (m_place_mode_index == ShapeTypeEnum::COMPOUNDSHAPE) {
			}
			ImGui::Separator();
		}
		else if (m_select_mode && m_selection_mode_index == SelectModeEnum::FREESELECT && m_selected_cluster_objects.size() > 0) {

		ImGui::InputFloat("Universal Angle : ", &m_free_select_rotation_universal_angle, 0.01f, 0.1f, "%.3f");
		if (ImGui::Button("Fix Angles")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle = m_free_select_rotation_universal_angle;
				ccobj.requesting_recreation = true;
			}
		}
		ImGui::InputFloat("Angle Padding : ", &m_free_select_rotation_padding, 0.01f, 0.1f, "%.3f");
		ImGui::Text("Angle Rotations");
		if (ImGui::SmallButton("Angle +")) {
			//loop through all the selected objects and apply the roation to them 
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle -= m_free_select_rotation_padding;
				ccobj.requesting_recreation = true;
			}
		 }
		ImGui::SameLine();
		if (ImGui::SmallButton("Angle -")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				ccobj.angle += m_free_select_rotation_padding;
				ccobj.requesting_recreation = true;
			}
		}

		ImGui::Text("Position Rotations");
		if (ImGui::SmallButton("Position +")) {
			//loop through all the selected objects and apply the roation to them 
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				SBX_INFO("Before {} {}",ccobj.position.x,ccobj.position.y);
				m_utilities.rotatePoint(ccobj.position, -m_free_select_rotation_padding);
				SBX_INFO("After {} {}", ccobj.position.x, ccobj.position.y);

				ccobj.requesting_recreation = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Position -")) {
			for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++) {
				SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
				m_utilities.rotatePoint(ccobj.position, m_free_select_rotation_padding);
				ccobj.requesting_recreation = true;
			}
		}
		}
	}
	ImGui::Text("\n\n"); //provide some bottom padding
	ImGui::End();

	//setting our current texture for this current object, regard less if we have interacted with the GUI code or not
	//Why  do have to do this ?
	//Date :22 October 2021 , if it we dont get weird Texture issues then its safe to remove this
	/* Because even we are just copying a previous objects attributes , texture is automatically adoted*/
	//if (m_current_cluster_object_ptr != nullptr) {
	//	if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::SINGLE) {
	//		//Singles
	//		if (m_layers[m_active_layer_index].single_textures.size() > 0 ) {
	//			m_current_cluster_object_ptr->texture = m_layers[m_active_layer_index].single_textures[m_selected_single_texture_index].texture;
	//			m_current_cluster_object_ptr->texture_index = m_selected_single_texture_index;
	//		}
	//		
	//	}
	//	else if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::TILESHEET) {
	//		//sheets
	//		if (m_layers[m_active_layer_index].tiled_textures.size() > 0) {
	//			m_current_cluster_object_ptr->texture = m_layers[m_active_layer_index].tiled_textures[m_selected_tile_sheet_texture_index].texture;
	//			m_current_cluster_object_ptr->texture_index = m_selected_tile_sheet_texture_index;
	//		}
	//	}
	//}

	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	if (m_show_console) {
		physics_tab_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}
	bool* physics_tab_open = false;
	ImGui::Begin("Side Tab", physics_tab_open, physics_tab_window_flags);
	if (m_show_console) {
		auto console_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), m_window->getScreenHeight()*0.75);
		ImGui::SetWindowSize(console_size);
	}
	else {
		auto console_size = ImVec2(static_cast<float>(ImGui::GetWindowWidth()), static_cast<float>(m_window->getScreenHeight()));
		ImGui::SetWindowSize(console_size);
	}
	ImGui::SetWindowPos(ImVec2(m_window->getScreenWidth() - ImGui::GetWindowWidth() - 2, 20));
	{
		int right_tab_subject = static_cast<int>(m_side_view);
		ImGui::Text("FPS: %.2f", static_cast<float>(m_game_ptr->getFps()));
		ImGui::NewLine();
		ImGui::RadioButton("Physics", &right_tab_subject, 0); ImGui::SameLine();
		ImGui::RadioButton("Layers ", &right_tab_subject, 1); ImGui::SameLine();
		ImGui::RadioButton("Layer Properties ", &right_tab_subject, 2);
		m_side_view = static_cast<RightTabDisplayEnum>(right_tab_subject);

		if (m_side_view == RightTabDisplayEnum::PHYSICS) {
			if (m_play_simulation) {
				if (ImGui::Button("Pause")) {
					m_play_simulation = false;
				}
			}
			else {
				if (ImGui::Button("Play")) {
					m_play_simulation = true;
				}
			}

			/*
			
			
			vector {



			    pair {

				first pair( [ClusterIndex , LayerIndex])
				second  vector {
									cluster Object Name
									cluster Object Index
								}
				}

			}
			
			
			*/

			std::vector<std::pair<std::pair<int, int>, std::vector<std::pair<std::string, int>>>> vectorOfPairVector;

			for (unsigned int h = 0; h < m_layers.size(); h++) {
				SquareBox::GWOM::Layer& layer = m_layers[h];

				if (layer.tile_system.getTiling() == SquareBox::LayerTilingEnum::None) {
					//a tiled layer wont be part of this
					for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
					{
						SquareBox::GWOM::ClusterObject& cwobj = m_world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
						bool isAlreadyPresentInVecOfpair = false;
						for (unsigned int j = 0; j < vectorOfPairVector.size(); j++) {
							if (vectorOfPairVector[j].first.second == h) {
								isAlreadyPresentInVecOfpair = true;
								vectorOfPairVector[j].second.push_back(std::pair(cwobj.name, cwobj.index));
								break;

							}
						}
						if (isAlreadyPresentInVecOfpair == false) {
							vectorOfPairVector.emplace_back();
							vectorOfPairVector.back().first = std::pair(std::pair(cwobj.cluster_index, h));
							vectorOfPairVector.back().second.push_back(std::pair(cwobj.name, cwobj.index));
						}
					}
				}
				
			}
				if (ImGui::TreeNode("Tree view"))
				{
					for (unsigned int i = 0; i < vectorOfPairVector.size(); i++)
					{
						if (ImGui::TreeNode(m_layers[vectorOfPairVector[i].first.second].name))
						{
							for (unsigned int j = 0; j < vectorOfPairVector[i].second.size(); j++)
							{
								if (ImGui::TreeNode((void*)(intptr_t)j, "%s %d", vectorOfPairVector[i].second[j].first.c_str(), vectorOfPairVector[i].second[j].second))
								{
									if (ImGui::SmallButton("up")) {
										m_utilities.movePairInVector(m_layers[vectorOfPairVector[i].first.second].alive_cluster_objects, std::pair(vectorOfPairVector[i].first.second, vectorOfPairVector[i].second[j].second), -1);
									}
									ImGui::SameLine();
									if (ImGui::SmallButton("down")) {
										m_utilities.movePairInVector(m_layers[vectorOfPairVector[i].first.second].alive_cluster_objects, std::pair(vectorOfPairVector[i].first.second, vectorOfPairVector[i].second[j].second), 1);
									}
									ImGui::SameLine();
									SquareBox::GWOM::ClusterObject& ccwobj = m_world_clusters[vectorOfPairVector[i].first.first].cluster_objects[vectorOfPairVector[i].second[j].second];
									if (ccwobj.is_hidden) {
										if (ImGui::SmallButton("Show")) {
											ccwobj.is_hidden = false;

										}
									}

									else {
										if (ImGui::SmallButton("Hide")) {
											ccwobj.is_hidden = true;
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


			if (m_select_mode && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT && m_current_cluster_object_ptr != nullptr) {
				ImGui::Text("Current Cluster Object's Properties ");
			}

			if ((m_select_mode && m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT || m_place_mode) && m_current_cluster_object_ptr != nullptr) {
				ImGui::Checkbox("Enable Physics", &m_enable_physics);
				ImGui::Text("Body Type");
				ImGui::RadioButton("Dynamic ", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::DynamicBody)); ImGui::SameLine();
				ImGui::RadioButton("Static", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::StaticBody)); ImGui::SameLine();
				ImGui::RadioButton("Kinematic", &m_selected_body_type, static_cast<int>(SquareBox::BodyTypeEnum::KinematicBody));

				m_current_cluster_object_ptr->type = static_cast<SquareBox::BodyTypeEnum>(m_selected_body_type);

				std::stringstream ss;
				ss << "World Cluster  Index ";
				ss << m_current_cluster_object_ptr->cluster_index;
				ImGui::Text(ss.str().c_str());
				ss.clear();

				std::stringstream ss1;
				ss1 << "Cluster Object Index ";
				ss1 << m_current_cluster_object_ptr->index;
				ImGui::Text(ss1.str().c_str());
				ss1.clear();

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

				ImGui::Separator();
			}
			else if (m_selection_mode_index == SelectModeEnum::JOINTSSELECT && m_select_mode) {
				ImGui::Checkbox("Show Joints", &m_debug_show_joints);
				static bool plot_joints = false;

				if (m_current_joint_ptr == nullptr) {
					ImGui::Checkbox("Plot Bridge", &plot_joints);
				}
				//for the bridgeWrapper
				if (plot_joints) {
					if (m_game_ptr->getInputManager()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))) {
						SBX_INFO("RIGHT CLICK RECEIVED!");
						const glm::vec2 & mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
						if (m_vector_of_joint_ploting_points.size() < 2) {
							m_vector_of_joint_ploting_points.push_back(mouse_in_world);
						}
					}

					if (ImGui::Button("Clear Plots")) {
						m_vector_of_joint_ploting_points.clear();
					}

					if (m_vector_of_joint_ploting_points.size() == 2 && m_current_joint_ptr == nullptr) {
						static float bridgeWeightLoss = 0.1f;
						ImGui::InputFloat("###bridgeWeightLoss", &bridgeWeightLoss, 0.1f, 0, "%.2f");

						static int bridge_shape = 0;
						ImGui::RadioButton("Boxes", &bridge_shape, 0);
						ImGui::RadioButton("Circles", &bridge_shape, 1);

						if (bridge_shape == 0) {
							m_temp_bridge_joints_object.shape = SquareBox::BodyShapeEnum::Box;
							ImGui::InputFloat("###boxBridgeHeight", &m_temp_bridge_joints_object.height, 0.1f, 0, "%.2f");
							ImGui::InputFloat("###boxBridgeWidth", &m_temp_bridge_joints_object.width, 0.1f, 0, "%.2f");
						}
						else if (bridge_shape == 1) {
							m_temp_bridge_joints_object.shape = SquareBox::BodyShapeEnum::Circle;
							ImGui::InputFloat("###circleBridgeRadius", &m_temp_bridge_joints_object.radius, 0.1f, 0, "%.2f");
						}

						bool enable_bridge_objects_rotation = !m_temp_bridge_joints_object.is_fixed_rotation;
						ImGui::Text("Rotation  : "); ImGui::SameLine();
						ImGui::Checkbox("###bridge_objects_rot", &enable_bridge_objects_rotation);
						m_temp_bridge_joints_object.is_fixed_rotation = !enable_bridge_objects_rotation;

						//the texture will be added later , after when the bridge has already been created

						if (ImGui::Button("Draw Bridge")) {
							SquareBox::Shapes::BridgeWrapper bridgewrapper;
							if (m_auto_assign_world_cluster) {
								bridgewrapper.createBridge(m_vector_of_joint_ploting_points[0], m_vector_of_joint_ploting_points[1], m_world_clusters, m_physics_world, m_current_active_world_cluster_index, m_layers[m_active_layer_index].alive_cluster_objects, m_temp_bridge_joints_object, bridgeWeightLoss);
							}
							else {
								bridgewrapper.createBridge(m_vector_of_joint_ploting_points[0], m_vector_of_joint_ploting_points[1], m_world_clusters, m_physics_world, m_mannually_choosen_world_cluster_index, m_layers[m_active_layer_index].alive_cluster_objects, m_temp_bridge_joints_object, bridgeWeightLoss);
							}
							m_vector_of_joint_ploting_points.clear();
						}
					}
				}
				//for the pulley Joint
				if (m_current_joint_ptr != nullptr) {
					if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
						if (m_game_ptr->getInputManager()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK))) {
							glm::vec2 mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
							if (m_current_joint_ptr->vec_of_points.size() < 2) {
								m_current_joint_ptr->vec_of_points.push_back(mouse_in_world);
							}
						}

						if (ImGui::Button("Clear Plots")) {
							m_current_joint_ptr->vec_of_points.clear();
						}
					}
				}
				if (m_selected_cluster_objects.size() == 2) {
					//loop through these bodies joints and get the details of the joint between them
					//clusterObjects can have onlyOne joint between them
					SquareBox::GWOM::ClusterObject& ccobjA = m_world_clusters[m_selected_cluster_objects[0].first].cluster_objects[m_selected_cluster_objects[0].second];
					SquareBox::GWOM::ClusterObject& ccobjB = m_world_clusters[m_selected_cluster_objects[1].first].cluster_objects[m_selected_cluster_objects[1].second];
					for (unsigned int i = 0; i < ccobjA.joints.size(); i++)
					{
						std::pair<int, int> bodyAACoordinates = std::pair(ccobjA.joints[i]->body_a_coordinates.first, ccobjA.joints[i]->body_a_coordinates.second);
						std::pair<int, int> bodyABCoordinates = std::pair(ccobjA.joints[i]->body_b_coordinates.first, ccobjA.joints[i]->body_b_coordinates.second);

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
							m_current_joint_ptr = ccobjA.joints[i];
							break;//break the loop
						}
					}
					if (m_current_joint_ptr == nullptr) {
						//we dont have a joint so create one
						m_current_joint_ptr = new SquareBox::GWOM::Joint();
						m_current_joint_ptr->is_joint_alive = false;

						m_current_joint_ptr->body_a_coordinates = std::pair(ccobjA.cluster_index, ccobjA.index);
						m_current_joint_ptr->body_b_coordinates = std::pair(ccobjB.cluster_index, ccobjB.index);
					}
					std::pair<int, int> currentjointbodyACoords = std::pair(m_current_joint_ptr->body_a_coordinates.first, m_current_joint_ptr->body_a_coordinates.second);
					std::pair<int, int> currentjointbodyBCoords = std::pair(m_current_joint_ptr->body_b_coordinates.first, m_current_joint_ptr->body_b_coordinates.second);
					//incase of any discripases this is to be considered as a new joint
					if (
						!(

							(currentjointbodyACoords == std::pair(ccobjA.cluster_index, ccobjA.index) &&
								currentjointbodyBCoords == std::pair(ccobjB.cluster_index, ccobjB.index))
							||
							(currentjointbodyACoords == std::pair(ccobjB.cluster_index, ccobjB.index) &&
								currentjointbodyBCoords == std::pair(ccobjA.cluster_index, ccobjA.index)))
						) {
						m_current_joint_ptr = new SquareBox::GWOM::Joint();
						m_current_joint_ptr->is_joint_alive = false;
						m_current_joint_ptr->body_a_coordinates = std::pair(ccobjA.cluster_index, ccobjA.index);
						m_current_joint_ptr->body_b_coordinates = std::pair(ccobjB.cluster_index, ccobjB.index);
					}

					ImGui::Text("Joint");
					ImGui::Text("Body A : "); ImGui::SameLine();
					ImGui::Text(m_world_clusters[m_current_joint_ptr->body_a_coordinates.first].cluster_objects[m_current_joint_ptr->body_a_coordinates.second].name);
					ImGui::Text("Body B : "); ImGui::SameLine();
					ImGui::Text(m_world_clusters[m_current_joint_ptr->body_b_coordinates.first].cluster_objects[m_current_joint_ptr->body_b_coordinates.second].name);

					const char* JointTypes[]{ ToString(SquareBox::JointTypeEnum::distance_joint),ToString(SquareBox::JointTypeEnum::revolute_joint),ToString(SquareBox::JointTypeEnum::prismatic_joint),ToString(SquareBox::JointTypeEnum::pulley_joint) };

					int selectedjointtype = static_cast<int>(m_current_joint_ptr->joint_type);
					ImGui::Combo("Joint Type ", &selectedjointtype, JointTypes, IM_ARRAYSIZE(JointTypes));
					m_current_joint_ptr->joint_type = static_cast<SquareBox::JointTypeEnum>(selectedjointtype);

					bool enable_collision_at_joint = m_current_joint_ptr->collide_connected_bodies;
					ImGui::Checkbox("Collision  ", &enable_collision_at_joint);
					m_current_joint_ptr->collide_connected_bodies = enable_collision_at_joint;

					ImGui::Text("Body A anchor: ");
					ImGui::InputFloat("###j_body_a_anchor_x", &m_current_joint_ptr->local_anchor_a.x, 0.01f, 0, "%.3f");
					ImGui::InputFloat("###j_body_a_anchor_y", &m_current_joint_ptr->local_anchor_a.y, 0.01f, 0, "%.3f");

					ImGui::Text("Body B anchor: ");
					ImGui::InputFloat("###j_body_b_anchor_x", &m_current_joint_ptr->local_anchor_b.x, 0.01f, 0, "%.3f");
					ImGui::InputFloat("###j_body_b_anchor_y", &m_current_joint_ptr->local_anchor_b.y, 0.01f, 0, "%.3f");

					//unique Attributes
					if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint) {
						ImGui::Text("Lower Angle: "); ImGui::SameLine();
						ImGui::InputFloat("###j_lower_angle", &m_current_joint_ptr->lower_angle, 0.01f, 0, "%.3f");
						ImGui::Text("Upper Angle: "); ImGui::SameLine();
						ImGui::InputFloat("###j_upper_angle", &m_current_joint_ptr->upper_angle, 0.01f, 0, "%.3f");
						ImGui::Text("Max Motor Torque"); ImGui::SameLine();
						ImGui::InputFloat("###j_maxMotorTorque", &m_current_joint_ptr->max_motor_torque, 0.01f, 0, "%.3f");
					}
					else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
						ImGui::Text("Lower Translation: "); ImGui::SameLine();
						ImGui::InputFloat("###j_lower_translation", &m_current_joint_ptr->lower_translation, 0.01f, 0, "%.3f");
						ImGui::Text("Upper Translation: "); ImGui::SameLine();
						ImGui::InputFloat("###j_upper_translation", &m_current_joint_ptr->upper_translation, 0.01f, 0, "%.3f");

						ImGui::Text("Max Motor Force"); ImGui::SameLine();
						ImGui::InputFloat("###j_maxMotorForce", &m_current_joint_ptr->max_motor_force, 0.01f, 0, "%.3f");

						ImGui::Text("Local Axis A: ");
						ImGui::InputFloat("###j_localAxisA_x", &m_current_joint_ptr->local_axis_a.x, 0.01f, 0, "%.3f");
						ImGui::InputFloat("###j_localAxisA_y", &m_current_joint_ptr->local_axis_a.y, 0.01f, 0, "%.3f");
					}
					else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::distance_joint) {
						//distance
						ImGui::Text("DampingRatio   : "); ImGui::SameLine();
						ImGui::InputFloat("###j_dr", &m_current_joint_ptr->damping_ratio, 0.01f, 0, "%.3f");

						ImGui::Text("frequencyHz: "); ImGui::SameLine();
						ImGui::InputFloat("###j_f", &m_current_joint_ptr->frequency_hz, 0.01f, 0, "%.3f");

						ImGui::Text("Stiffness: "); ImGui::SameLine();
						ImGui::InputFloat("###j_stiffness", &m_current_joint_ptr->stiffness, 0.01f, 0, "%.3f");

						ImGui::Text("Length: "); ImGui::SameLine();
						ImGui::InputFloat("###j_length", &m_current_joint_ptr->length, 0.01f, 0, "%.3f");

						ImGui::Text("Max Length: "); ImGui::SameLine();
						ImGui::InputFloat("###j_max_length", &m_current_joint_ptr->max_length, 0.01f, 0, "%.3f");

						ImGui::Text("MinLength: "); ImGui::SameLine();
						ImGui::InputFloat("###j_min_length", &m_current_joint_ptr->min_length, 0.01f, 0, "%.3f");
					}
					else if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::pulley_joint) {
						if (m_current_joint_ptr->vec_of_points.size() == 2) {
							m_current_joint_ptr->ground_anchor_a = m_current_joint_ptr->vec_of_points[0];
							m_current_joint_ptr->ground_anchor_b = m_current_joint_ptr->vec_of_points[1];
							m_current_joint_ptr->length_a = fabs(glm::length(m_current_joint_ptr->vec_of_points[0] - ccobjA.physics_properties->getXYPosition()));
							m_current_joint_ptr->length_b = fabs(glm::length(m_current_joint_ptr->vec_of_points[1] - ccobjB.physics_properties->getXYPosition()));
						}
						ImGui::Text("Pulley Ratio: "); ImGui::SameLine();
						ImGui::InputFloat("###j_pulley_ratio", &m_current_joint_ptr->pulley_ratio, 0.01f, 0, "%.3f");
						static float pulley_joint_color[4];
						pulley_joint_color[0] = m_current_joint_ptr->color.x;
						pulley_joint_color[1] = m_current_joint_ptr->color.y;
						pulley_joint_color[2] = m_current_joint_ptr->color.z;
						pulley_joint_color[3] = m_current_joint_ptr->color.w;

						ImGui::ColorEdit4("Pulley color", pulley_joint_color);

						m_current_joint_ptr->color.x = pulley_joint_color[0];
						m_current_joint_ptr->color.y = pulley_joint_color[1];
						m_current_joint_ptr->color.z = pulley_joint_color[2];
						m_current_joint_ptr->color.w = pulley_joint_color[3];

						ImGui::InputFloat("###j_pulley_rope_thickness", &m_current_joint_ptr->thickness, 0.01f, 0, "%.3f");
					}

					//shared attributes
					if (m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::revolute_joint || m_current_joint_ptr->joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
						ImGui::Text("Reference Angle: "); ImGui::SameLine();
						ImGui::InputFloat("###j_reference_angle", &m_current_joint_ptr->reference_angle, 0.01f, 0, "%.3f");
						ImGui::Text("Motor Speed: "); ImGui::SameLine();
						ImGui::InputFloat("###j_motor_speed", &m_current_joint_ptr->motor_speed, 0.01f, 0, "%.3f");

						bool enable_joint_limit = m_current_joint_ptr->enable_limit;
						ImGui::Checkbox("Enable Limit: ", &enable_joint_limit);
						m_current_joint_ptr->enable_limit = enable_joint_limit;

						bool enable_joint_motor = m_current_joint_ptr->enable_motor;
						ImGui::Checkbox("Enable Motor: ", &enable_joint_motor);
						m_current_joint_ptr->enable_motor = enable_joint_motor;
					}

					if (m_current_joint_ptr->is_joint_alive) {
						if (ImGui::Button("Update Joint")) {
							if (ccobjA.physics_properties != nullptr && ccobjB.physics_properties != nullptr) {
								m_utilities.destroyClusterObjectJoint(ccobjA, ccobjB);
								m_physics_world.createJoint(ccobjA, ccobjB, *m_current_joint_ptr);
								m_current_joint_ptr = nullptr;
							}
						}
						ImGui::SameLine();
						if (ImGui::Button("Destory Joint")) {
							m_utilities.destroyClusterObjectJoint(ccobjA, ccobjB);
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
										m_physics_world.createJoint(ccobjA, ccobjB, *m_current_joint_ptr);
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
					std::stringstream ss;
					ss << "World Cluster  Index ";
					ss << m_world_clusters[m_selected_cluster_objects[0].first].index;
					ImGui::Text(ss.str().c_str());
					ss.clear();

					ImGui::Text("World Cluster Name");
					ImGui::InputText("###SelectedClusterObjectsName", m_world_clusters[m_selected_cluster_objects[0].first].name, 20);
				
				}
			}
			else if (m_selection_mode_index == SelectModeEnum::CLUSTEROBJECT && m_current_cluster_object_ptr != nullptr) {
				//ClusterObject
				if (m_select_mode && m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
					if (ImGui::Button("Update"))
					{
						m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
						//we are trusting our program to always make the selected object the current object
						if (m_current_shape_ptr->is_plotted) {
							m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
							m_is_all_work_saved = false;
						}
						else
						{
							m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
							m_is_all_work_saved = false;
						}
					}

					if (ImGui::Button("Delete")) {
						m_current_cluster_object_ptr->life_span = 0;
					}
				}
			}
			if ((m_selection_mode_index == SelectModeEnum::WORLDCLUSTER || m_selection_mode_index == SelectModeEnum::FREESELECT || m_selection_mode_index == SelectModeEnum::JOINTSSELECT) && m_selected_cluster_objects.size() > 0) {
				ImGui::Text("%d Objects Selected ", m_selected_cluster_objects.size());
				
				if (ImGui::Button("Clear Selection")) {
						m_selected_cluster_objects.clear();
					}
					if (ImGui::Button("Delete Selected")) {
						for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
						{
							m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second].life_span = 0;
						}
						if (m_selected_cluster_objects.size() == 2) {
							//joint Selection, safe guards us if a joint had already been created
							m_current_joint_ptr = nullptr;//nullfy the current joints data since we just deleted it
						}
						m_selected_cluster_objects.clear();
					}
					if (ImGui::Button("Hide Selected")) {
					for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
					{
						SquareBox::GWOM::ClusterObject& ccobj = m_world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
						ccobj.is_hidden = true;
					}
					m_selected_cluster_objects.clear();
				}

					//shifting selected to a different world cluster
					static int target_world_cluster = 0;
					ImGui::Separator();
					ImGui::DragInt("Target Index :", &target_world_cluster, 1, 0, m_world_clusters.size()-1);
					if (target_world_cluster>static_cast<int>(m_world_clusters.size()-1))
					{
						target_world_cluster = m_world_clusters.size() - 1;
					}

					if (target_world_cluster <0)
					{
						target_world_cluster = 0;
					}
					ImGui::Text("Target Cluster Object:");
					ImGui::SameLine();
					ImGui::Text(m_world_clusters[target_world_cluster].name);
					ImGui::SameLine();
					if (ImGui::Button("Shift selected")) {
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
								const int world_cluster_index = target_world_cluster;

								//a copy not reference
								SquareBox::GWOM::ClusterObject copy_of_original_ccobj = m_world_clusters[copy_of_selected_objects[0].first].cluster_objects[copy_of_selected_objects[0].second];
								copy_of_original_ccobj.cluster_index = world_cluster_index;
								copy_of_original_ccobj.is_alive = false;
								copy_of_original_ccobj.life_span = 10;
								//TODO :
								//joints are not copied , i will implement this later
								copy_of_original_ccobj.joints.clear();
								//change its index 
								copy_of_original_ccobj.index = m_world_clusters[world_cluster_index].cluster_objects.size();
								// rename it
								m_utilities.nameClusterObjectByIndex(copy_of_original_ccobj);

								//add the copy to the worldCLusters
								m_world_clusters[world_cluster_index].cluster_objects.push_back(copy_of_original_ccobj);

								// get a referenece to it 
								new_ccobj = &m_world_clusters[world_cluster_index].cluster_objects.back();

							}
							else {
								//depend on the shell we got 
								if (got_shell_coordinates.first >= 0 && got_shell_coordinates.second >= 0) {

									//copy properties
									m_world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second] = m_world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second];
									//point to our object
									new_ccobj = &m_world_clusters[got_shell_coordinates.first].cluster_objects[got_shell_coordinates.second];
									//restore the cluster_index && index
									new_ccobj->cluster_index = got_shell_coordinates.first;
									new_ccobj->index = got_shell_coordinates.second;
									//restore the default name
									m_utilities.nameClusterObjectByIndex(*new_ccobj);
									new_ccobj->is_alive = false;
									new_ccobj->life_span = 10;
									//joints are not copied , i will implement this later
									new_ccobj->joints.clear();
								}
								else {

									SBX_ERROR("Invalid shell coordinates");
									__debugbreak();
								}
							}

							//The actual creatioin into the world starts here
							m_utilities.setCurrentShapePointer(new_ccobj->shape, &m_current_shape_ptr);
							//the world placement

							//the core should not create a shell after we create the last  cluster object
							bool create_shell = i < (copy_of_selected_objects.size() - 1);
							int desired_shell_world_cluster_index = -1;//only remain true if we are not creating a shell 
							if (create_shell) {
								desired_shell_world_cluster_index = target_world_cluster;
								/*
								* world cluster index
								* of next cluster objects in the copy_of_selected_objects
								*/
							}
							SquareBox::Utilities::creationDetails  details;
							if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
								details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), new_ccobj, m_world_clusters, desired_shell_world_cluster_index, &m_physics_world, create_shell, false);
							}
							else {
								details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), new_ccobj, m_world_clusters, desired_shell_world_cluster_index, &m_physics_world, create_shell, false);
							}
							if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
								m_utilities.addPairToVector(m_layers[m_active_layer_index].alive_cluster_objects, details.settlementCoordinates);
								m_utilities.addPairToVector(m_selected_cluster_objects, details.settlementCoordinates);
							}
							else {
								SBX_ERROR("We failed to get back settelment coordinates when copying a world cluster ");
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
							m_world_clusters[copy_of_selected_objects[i].first].cluster_objects[copy_of_selected_objects[i].second].life_span = 0;
						}
					}
			}
		}
		else if(m_side_view == RightTabDisplayEnum::LAYERS) {
		
		for (unsigned int i = 0; i < m_layers.size(); i++) {
			SquareBox::GWOM::Layer& layer = m_layers[i];
			//Visibility
			if (layer.is_visible) {
				std::stringstream ss;
				ss << "Hide ";
				ss << i;
				if (ImGui::SmallButton(ss.str().c_str())) {
					layer.is_visible = false;
				}
			}

			else {
				std::stringstream ss;
				ss << "Show ";
				ss << i;
				if (ImGui::SmallButton(ss.str().c_str())) {
					layer.is_visible = true;
				}
			}
			ImGui::SameLine();
			//is locked
			if (layer.is_locked) {
				std::stringstream ss;
				ss << "Unlock ";
				ss << i;
				if (ImGui::SmallButton(ss.str().c_str())) {
					layer.is_locked = false;
				}
			}

			else {
				std::stringstream ss;
				ss << "Lock ";
				ss << i;
				if (ImGui::SmallButton(ss.str().c_str())) {
					layer.is_locked = true;
				}
			}
			ImGui::SameLine();
			std::stringstream ss;
			ss << "####LayerName ";
			ss << i;
			ImGui::InputText(ss.str().c_str(), layer.name, 20);
			ImGui::SameLine();
			if (m_active_layer_index==i) {
				ImGui::SmallButton("active");
			
			}
			else {

				std::stringstream ss;
				ss << ". ";
				ss << i;

				if(ImGui::SmallButton(ss.str().c_str()) && layer.is_visible && !layer.is_locked) {
					//we are going to be changing layers so clear all selected
					m_selected_cluster_objects.clear();
					if (m_select_mode) {
						m_current_world_cluster_ptr = nullptr;
						m_current_cluster_object_ptr = nullptr;
					}

					m_active_layer_index = i;

				}
			}
		}

		
		ImGui::Separator();
		if(ImGui::Button("New Layer")) {
			SBX_INFO("Creating New layers");
			createNewLayer();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Layer")) {
            //can't leave the world with no layers
			if (m_layers.size() > 1) {
				//remove the active layer from m_layers

				//first clear out the active layer
				for (unsigned int i = 0; i < m_layers[m_active_layer_index].alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[m_layers[m_active_layer_index].alive_cluster_objects[i].first].cluster_objects[m_layers[m_active_layer_index].alive_cluster_objects[i].second];
					//actually delete these cluster Objects
					cwobj->life_span = 0;
					cwobj->is_alive = false;
					if (cwobj->physics_properties != nullptr) {
						cwobj->physics_properties->dispose();
						delete cwobj->physics_properties; //we should always free up this memory with delete
						cwobj->physics_properties = nullptr;
					}
				}
				m_layers[m_active_layer_index].alive_cluster_objects.clear();
				m_selected_cluster_objects.clear();

				//the real deletion
				int item_count = 0;
				for (auto it = m_layers.begin(); it != m_layers.end();it++)
				{
					if (item_count == m_active_layer_index) {
						//delete and break;
						SBX_INFO("{} deleted", std::string(it->name));
						m_layers.erase(it);
						break;
					}
					item_count = item_count + 1;
				}

				//reassign the active layer to the first layer 
				m_active_layer_index = 0;
				//force open it
				m_layers[m_active_layer_index].is_locked = false;
				m_layers[m_active_layer_index].is_visible = true;
			}
			else {
				SBX_ERROR("Can't leave the world with out any layers to work with !!!");
			}

		}

		
		ImGui::SameLine();
		if (ImGui::Button("Clear Layer")) {
			for (unsigned int i = 0; i < m_layers[m_active_layer_index].alive_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[m_layers[m_active_layer_index].alive_cluster_objects[i].first].cluster_objects[m_layers[m_active_layer_index].alive_cluster_objects[i].second];
				//actually delete these cluster Objects
				cwobj->life_span = 0;
				cwobj->is_alive = false;
				if (cwobj->physics_properties != nullptr) {
					cwobj->physics_properties->dispose();
					delete cwobj->physics_properties; //we should always free up this memory with delete
					cwobj->physics_properties = nullptr;
				}
			}
			m_layers[m_active_layer_index].alive_cluster_objects.clear();
			m_selected_cluster_objects.clear();
		}

		ImGui::SameLine();
		if (ImGui::ArrowButton("Layer_up", ImGuiDir_Up)) {
			int new_desired_layer_index = m_active_layer_index - 1;
			if (new_desired_layer_index >= 0) {
				/*
				we have to literally shift this layer and the only way to do that is to do a clean cut copy swap replace

				*/

				//the copy
				SquareBox::GWOM::Layer orignal_layer_data = m_layers[m_active_layer_index];
				SquareBox::GWOM::Layer destination_layer_data = m_layers[new_desired_layer_index];

				//the swap
				m_layers[new_desired_layer_index] = orignal_layer_data;
				m_layers[m_active_layer_index] = destination_layer_data;

				m_active_layer_index = new_desired_layer_index;

			}
			else {
				//there is no where to move up to 
			}

		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("Layer_down", ImGuiDir_Down)) {
			int new_desired_layer_index = m_active_layer_index + 1;
			if (new_desired_layer_index < static_cast<int>(m_layers.size())) {
				/*
				we have to literally shift this layer and the only way to do that is to do a clean cut copy swap replace

				*/

				//the copy
				SquareBox::GWOM::Layer orignal_layer_data = m_layers[m_active_layer_index];
				SquareBox::GWOM::Layer destination_layer_data = m_layers[new_desired_layer_index];

				//the swap
				m_layers[new_desired_layer_index] = orignal_layer_data;
				m_layers[m_active_layer_index] = destination_layer_data;

				m_active_layer_index = new_desired_layer_index;

			}
			else {
				//there is no where to move down to 
			}
		}
		}
		else if (m_side_view == RightTabDisplayEnum::LAYERPROPERTIES) {
		if (m_layers[m_active_layer_index].tile_system.getTiling() != SquareBox::LayerTilingEnum::None && m_layers[m_active_layer_index].tile_system.isInitialised()) {
			ImGui::Text("Layer Tiling  :  %s  ", ToString(m_layers[m_active_layer_index].tile_system.getTiling()));
			ImGui::Text("Layer Origin  :  %.3f  ,  %.3f", m_layers[m_active_layer_index].tile_system.getTileSystemOrigin().x, m_layers[m_active_layer_index].tile_system.getTileSystemOrigin().y);
			ImGui::Text("Layer Width   :  %.3f  ", m_layers[m_active_layer_index].tile_system.getWidth());
			ImGui::Text("Layer Height  :  %.3f  ", m_layers[m_active_layer_index].tile_system.getHeight());
			ImGui::Text("Tile size     :  %.3f", m_layers[m_active_layer_index].tile_system.getTileSize());
			auto total_cells = m_layers[m_active_layer_index].tile_system.getNumXCells() * m_layers[m_active_layer_index].tile_system.getNumYCells();
			ImGui::Text("Num Tiles     :  %d", total_cells);
		}
		else {
			ImGui::Text("Layer Tiling  :  %s  ", ToString(m_layers[m_active_layer_index].tile_system.getTiling()));
		}
		if(ImGui::Button("Update Tiling")) {
			 m_show_update_layer_tiling_dialog = true;
		}
		ImGui::Text("Layer Opacity :"); ImGui::SameLine();
		ImGui::SliderInt("###layeropacity", &m_layers[m_active_layer_index].opacity, 0, 100);
		ImGui::Separator();
		if (m_selected_cluster_objects.size()>0) {
			if (ImGui::Button("Shift Selected Objects")) {
				//we active a pop up that lets us shif all selected Objects to another layer
				m_show_shift_cobjs_to_another_layer_dialog = true;
				m_target_shift_to_layer_index = 0;//reset this
			}

		}

			ImGui::SameLine();
			if (ImGui::Button("Unhide All Objects")) {
				for (unsigned int i = 0; i < m_layers[m_active_layer_index].alive_cluster_objects.size(); i++)
				{
					SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[m_layers[m_active_layer_index].alive_cluster_objects[i].first].cluster_objects[m_layers[m_active_layer_index].alive_cluster_objects[i].second];
					//actually delete these cluster Objects
					cwobj->is_hidden = false;
				}
			}
		}
	}
	ImGui::Text("\n\n"); //Bottom padding
	ImGui::End();
	//file explore dialog
	{
		//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
		if (m_show_open_level_file_dialog)
			ImGui::OpenPopup("Open Level File");
		if (m_show_save_level_file_dialog)
			ImGui::OpenPopup("Save Level File");
		if (m_show_open_texture_file_dialog)
			ImGui::OpenPopup("Open Texture File");

		if (m_show_open_tile_sheet_file_dialog)
			ImGui::OpenPopup("Open TileSheet File");

		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */
		if (m_file_dialog.showFileDialog("Open Level File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_level_file_extension))//".txt,.dat"
		{
			std::cout << m_file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << m_file_dialog.selected_path << std::endl;    // The absolute path to the selected file
			std::cout << m_file_dialog.ext << std::endl;    // The absolute path to the selected file

			
				SBX_INFO("Loading {} Level", m_level_file_extension);
				//clean out the current world
				for (size_t i = 0; i < m_world_clusters.size(); i++)
				{
					if (m_world_clusters[i].cluster_objects.size() > 0) {
						for (size_t j = 0; j < m_world_clusters[i].cluster_objects.size(); j++)
						{
							SquareBox::GWOM::ClusterObject & ccobj = m_world_clusters[i].cluster_objects[j];
							if (m_world_clusters[i].cluster_objects[j].physics_properties != nullptr &&m_world_clusters[i].cluster_objects[j].physics_properties->isIntialised()) {
								m_world_clusters[i].cluster_objects[j].physics_properties->dispose();
								m_world_clusters[i].cluster_objects[j].physics_properties = nullptr;
							}
						}
					}
					else {
						m_world_clusters[i].cluster_objects.clear();
					}
				}

				//clean out the layers
		//clear out all layers
				for each (auto & layer in m_layers) {
					if (layer.tile_system.getTiling() == SquareBox::LayerTilingEnum::None) {
						for (unsigned int i = 0; i < layer.alive_cluster_objects.size(); i++)
						{
							SquareBox::GWOM::ClusterObject* cwobj = &m_world_clusters[layer.alive_cluster_objects[i].first].cluster_objects[layer.alive_cluster_objects[i].second];
							if (cwobj->physics_properties != nullptr) {
								cwobj->physics_properties->dispose();
								delete cwobj->physics_properties; //we should always free up this memory with delete
								cwobj->physics_properties = nullptr;
							}
						}
					}
					
				}
				m_layers.clear();
				m_world_clusters.clear();
				m_selected_cluster_objects.clear();
				m_current_active_world_cluster_index = 0;
				m_current_active_cluster_object_index = 0;
				m_auto_assign_world_cluster = true;
				m_respect_ancestor = false;
				/*
					we need a temp WorldCluster vec because create ClusterObject handles adding new cluster objects
					to the currentActiveWorld by its self , so we have to do some logic to make sure we are were are always in the cluster_object array
				*/
				
				m_level_reader_writer.loadWorldAsBinary(m_file_dialog.selected_path, m_world_clusters,m_layers,m_universal_camera_scale,m_universal_camera_position,m_active_layer_index);

				
				for (unsigned int i = 0; i < m_layers.size(); i++)
				{
					SquareBox::GWOM::Layer& layer = m_layers[i];
					//recreate all the layer cameras
					layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
					layer.camera.setScale(m_universal_camera_scale);
					layer.camera.setPosition(m_universal_camera_position);

					//load all the layers textures
					/* We need to load the textures in an order that respects the their index in their respective category*/
					//singles
					//copy the orignal list
					auto original_single_textures = layer.single_textures;
					layer.single_textures.clear();
					for (unsigned int j = 0; j < original_single_textures.size(); j++)
					{
						auto& current_single_parent_texture = original_single_textures[j];
						SquareBox::GWOM::ParentTexture tmp_single_parent_texture;
						tmp_single_parent_texture.texture_index = j;//if this causes errors then we have a bug somewhere
						tmp_single_parent_texture.texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_single_parent_texture.texture.display_name, m_window->getDDPI());
						tmp_single_parent_texture.texture.asset_file_path = current_single_parent_texture.texture.asset_file_path;
						tmp_single_parent_texture.texture.display_name = current_single_parent_texture.texture.display_name;
						layer.single_textures.push_back(tmp_single_parent_texture);
					}
				
					//tiled textures
					auto original_tiled_textures = layer.tiled_textures;
					layer.tiled_textures.clear();
					for (unsigned int j = 0; j < original_tiled_textures.size(); j++)
					{
						auto& current_tiled_parent_texture = original_tiled_textures[j];
						SquareBox::GWOM::ParentTexture tmp_tiled_parent_texture;
						tmp_tiled_parent_texture.texture_index = j;//if this causes errors then we have a bug somewhere
						tmp_tiled_parent_texture.texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_tiled_parent_texture.texture.display_name, m_window->getDDPI());
						tmp_tiled_parent_texture.texture.asset_file_path = current_tiled_parent_texture.texture.asset_file_path;
						tmp_tiled_parent_texture.texture.display_name = current_tiled_parent_texture.texture.display_name;
						tmp_tiled_parent_texture.texture.tiling = current_tiled_parent_texture.texture.tiling;
						layer.tiled_textures.push_back(tmp_tiled_parent_texture);
					}
				}

				if (m_world_clusters.size() > 0) {
					for (unsigned int i = 0; i < m_world_clusters.size(); i++)
					{
						m_current_active_world_cluster_index = i;//make this the active world Cluster
						m_current_world_cluster_ptr = &m_world_clusters[i];
						if (m_world_clusters[i].cluster_objects.size() > 0)
						{
							printf("THIS WORLD HAS %d cluster Objects \n", m_world_clusters[i].cluster_objects.size());
							unsigned int numWorldClusterObjects = m_world_clusters[i].cluster_objects.size();//so that its not baised as we add new cwcobj
							for (unsigned int j = 0; j < numWorldClusterObjects; j++)
							{
								m_current_active_cluster_object_index = j;
								bool lastClusterObjectMemberinLastWorldCluster = (i == m_world_clusters.size() - 1) && (j == numWorldClusterObjects - 1) ? true : false;
								SquareBox::GWOM::ClusterObject* cobj = &m_world_clusters[i].cluster_objects[j];
								m_utilities.setCurrentShapePointer(cobj->shape, &m_current_shape_ptr);

								m_current_cluster_object_ptr = cobj;

								if (!lastClusterObjectMemberinLastWorldCluster) {
									if (m_current_shape_ptr->is_plotted) {
										printf("Create a plotted Shape Called with %d vertices and index id %d \n", cobj->vertices.size(), cobj->index);
										SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
										if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
											if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
												m_is_all_work_saved = false;
											}
											else {
												SBX_ERROR("Failed to recreate a cluster object on loading level");
											}
										}
									}
									else
									{
										SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
									
										if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
											if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
												m_is_all_work_saved = false;
											}
											else {
												SBX_ERROR("Failed to recreate a cluster object on loading level");
											}
										}
									}
								}
								else {
									if (m_current_shape_ptr->is_plotted) {
										printf("Create a ploted Shape Called with %d vertices and index id %d \n", cobj->vertices.size(), cobj->index);
										SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(true, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, true, false);
										if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
											if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
												m_is_all_work_saved = false;
											}
											else {
												SBX_ERROR("Failed to recreate a cluster object on loading level");
											}
										}
										//get a pointer to the shell
										if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
											m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
											m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
											m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
										}
										else {
											SBX_ERROR("Missing shell!!");
											__debugbreak();
										}
									}
									else
									{
										SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, true, false);
										if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
											if (m_world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
												m_is_all_work_saved = false;
											}
											else {
												SBX_ERROR("Failed to recreate a cluster object on loading level");
											}
										}
										//get a pointer to the shell
										if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
											m_current_cluster_object_ptr = &m_world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
											m_current_active_world_cluster_index = m_current_cluster_object_ptr->cluster_index;
											m_current_active_cluster_object_index = m_current_cluster_object_ptr->index;
										}
										else {
											SBX_ERROR("Missing shell!!");
											__debugbreak();
										}
									}

									updateStates();
								}
							}
						}
					}

					SBX_INFO("Creating Joints");
					m_utilities.createWorldClusterJoints(m_world_clusters, m_physics_world);
					SBX_INFO("Finished Creating Joints");
					SBX_INFO("Loaded {} Level", m_level_file_extension);
				}
				else {
					SBX_INFO("Restoring empty world");
					//This was an empty world saved , just restore the defaults
					//first WorldCluster
					m_world_clusters.emplace_back();
					m_current_world_cluster_ptr = &m_world_clusters.back();
					m_current_world_cluster_ptr->index = m_world_clusters.size() - 1;
					m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);

					//First Cluster object
					m_current_world_cluster_ptr->cluster_objects.emplace_back();
					m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects.back();
					m_current_cluster_object_ptr->index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
					m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);

					//Create the first layer mannually
					//push back a new layer

					m_layers.emplace_back();
					//get a reference to the last placed layer
					SquareBox::GWOM::Layer& layer = m_layers.back();
					layer.alive_cluster_objects.clear();
					layer.alive_cluster_objects.reserve(60);//some cache friendlness
					layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
					layer.camera.setScale(m_universal_camera_scale);
					layer.camera.setPosition(m_universal_camera_position);
					layer.is_visible = true;
					layer.is_locked = false;
					m_active_layer_index = 0;
				}
			
		}
		if (m_file_dialog.showFileDialog("Open Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			m_texture.asset_file_path = m_file_dialog.selected_path;

			m_texture.display_name = m_file_dialog.selected_fn;
			//m_single_textures.push_back(m_texture);
			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_layers[m_active_layer_index].single_textures.size(); i++)
			{
				if (m_layers[m_active_layer_index].single_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {
				SquareBox::GWOM::ParentTexture new_parent_texture;
				new_parent_texture.texture_index = m_layers[m_active_layer_index].single_textures.size();
				new_parent_texture.texture = m_texture;
				m_layers[m_active_layer_index].single_textures.push_back(new_parent_texture);

				//add to the sub_textures_table for all layers , since a none tiled can become tiled
					SquareBox::GWOM::SubTexture new_sub_texture;
					new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
					new_sub_texture.parent_type = SquareBox::TextureEnum::SINGLE;
					new_sub_texture.sub_texture_key = m_layers[m_active_layer_index].sub_textures_table.size();
					m_layers[m_active_layer_index].sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
			}
		}
		if (m_file_dialog.showFileDialog("Open TileSheet File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			m_texture.asset_file_path = m_file_dialog.selected_path;
			m_texture.display_name = m_file_dialog.selected_fn;
			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_layers[m_active_layer_index].tiled_textures.size(); i++)
			{
				if (m_layers[m_active_layer_index].tiled_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {
				SquareBox::GWOM::ParentTexture new_parent_texture;
				new_parent_texture.texture_index = m_layers[m_active_layer_index].tiled_textures.size();
				new_parent_texture.texture = m_texture;
				new_parent_texture.texture.tiling = m_new_tiled_texture_tiling;
				m_new_tiled_texture_tiling.x = 0;
				m_new_tiled_texture_tiling.y = 0;
				if (new_parent_texture.texture.tiling.x > 0 && new_parent_texture.texture.tiling.y > 0) {
					m_layers[m_active_layer_index].tiled_textures.push_back(new_parent_texture);

						//loop through all the tilesheet indiviaual grids adding one by one
						int num_sections = new_parent_texture.texture.tiling.x * new_parent_texture.texture.tiling.y;
						int initial_sub_texture_key = m_layers[m_active_layer_index].sub_textures_table.size();
						for (int i = 0; i < num_sections; i++)
						{
							SquareBox::GWOM::SubTexture new_sub_texture;
							new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
							new_sub_texture.parent_type = SquareBox::TextureEnum::TILESHEET;
							new_sub_texture.tiling_index = i;
							new_sub_texture.sub_texture_key = initial_sub_texture_key + i;
							m_layers[m_active_layer_index].sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
						}
				}
				else {
					SBX_ERROR("Failed to Added TileSheet {} to Editor, Invalid Tiling", new_parent_texture.texture.display_name);
				}
			}
		}

		if (m_file_dialog.showFileDialog("Save Level File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_level_file_extension))
		{
			
			std::cout << m_file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << m_file_dialog.selected_path << std::endl;    // The absolute path to the selected file
			std::cout << m_file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)

				SBX_INFO("Saving Level as {} File", m_level_file_extension);
				//eliminate all the file extensions just incase the user included an extra file extension
				//e.g level.txt.txt
				std::string ext = m_file_dialog.ext;
				std::string filePath = m_file_dialog.selected_path;
				
				while (filePath != ext &&
					filePath.size() > ext.size() &&
					filePath.substr(filePath.size() - ext.size()) == ext)
				{
					// if so then strip them off
					filePath = filePath.substr(0, filePath.size() - ext.size());
				}
				
				m_selected_cluster_objects.clear();
				m_utilities.worldIndiciesCleanUp(m_world_clusters,m_layers);
				if (m_level_reader_writer.saveWorldAsBinary(filePath + m_file_dialog.ext, m_world_clusters,m_layers,m_layers[m_active_layer_index].camera.getScale(),m_layers[m_active_layer_index].camera.getPosition(),m_active_layer_index))
				{
					SBX_INFO("Level Saved");
					m_is_all_work_saved = true;
				}
				else {
					m_show_failed_to_save_world_dialog = true;
					SBX_WARN("Failed To Save");
				}
				
				if (m_world_clusters.size() == 0) {
					//this was an empty world before the save so we should return its defaults
					//first WorldCluster
					m_world_clusters.emplace_back();
					m_current_world_cluster_ptr = &m_world_clusters.back();
					m_current_world_cluster_ptr->index = m_world_clusters.size() - 1;
					m_utilities.nameWorldClusterByIndex(*m_current_world_cluster_ptr);

					//First Cluster object
					m_current_world_cluster_ptr->cluster_objects.emplace_back();
					m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects.back();
					m_current_cluster_object_ptr->index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
					m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				}
				
				//return back , the m_currentObject
				if (m_auto_assign_world_cluster) {
					m_current_active_world_cluster_index = m_world_clusters.size() - 1;
					m_current_world_cluster_ptr = &m_world_clusters[m_current_active_world_cluster_index];
					m_current_world_cluster_ptr->cluster_objects.emplace_back();
					int	last_index = m_current_world_cluster_ptr->cluster_objects.size() - 1;
					m_current_cluster_object_ptr = &m_current_world_cluster_ptr->cluster_objects[last_index];
					updateStates();
					m_current_cluster_object_ptr->cluster_index = m_current_world_cluster_ptr->index;
					m_current_cluster_object_ptr->index = last_index;
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				}
				else {
					m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.emplace_back();
					int	last_index = m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects.size() - 1;
					m_current_cluster_object_ptr = &m_world_clusters[m_mannually_choosen_world_cluster_index].cluster_objects[last_index];
					updateStates();
					m_current_cluster_object_ptr->cluster_index = m_world_clusters[m_mannually_choosen_world_cluster_index].index;
					m_current_cluster_object_ptr->index = last_index;
					m_utilities.nameClusterObjectByIndex(*m_current_cluster_object_ptr);
				}
		}

		//reset of all dialogs
		m_show_open_level_file_dialog = false;
		m_show_save_level_file_dialog = false;
		m_show_open_texture_file_dialog = false;
		m_show_open_tile_sheet_file_dialog = false;
	}

	if (m_show_console) {
		auto console_position = ImVec2(0.0f, m_window->getScreenHeight()*0.75f);
		auto console_size = ImVec2(static_cast<float>(m_window->getScreenWidth()), m_window->getScreenHeight()*0.25f);
		ImGuiWindowFlags console_flags = 0;
		console_flags |= ImGuiWindowFlags_NoTitleBar;
		m_console.Draw("Console", &m_show_console, console_flags, console_position, console_size);

		for each (auto log_message in m_fresh_logs)
		{
			m_console.AddLog(log_message.c_str());
		}
		m_fresh_logs.clear();
	}
	else {
		if (m_fresh_logs.size() > 0) {
			m_fresh_logs.clear();
		}
	}

	// Rendering
	ImGui::EndFrame();
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SquareBoxEditor::LevelEditor_Screen::updateStates()
{
	//updates all states to show the values of the currently active/selected object so that it doesn't loss its values
	//by taking up the ones of its predessors

	//this function should be called each time we change objects if we want to use the expected new state

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

void SquareBoxEditor::LevelEditor_Screen::checkPhysicsStatus()
{
	if (m_current_cluster_object_ptr != nullptr) {
		if (m_enable_physics) {
			if (m_current_cluster_object_ptr->physics_properties == nullptr) {
				if (m_place_mode) {
					//the object had not yet been created just give it physics back
					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
				}
				else if (m_select_mode) {
					//the object had already been created !!!
					m_current_cluster_object_ptr->physics_properties = new SquareBox::PhysicsCollisionEngine::PhysicsProperties();
					//recreate it into the physics world
					if (m_current_shape_ptr->is_plotted)
					{
						m_utilities.createClusterObjectIntoWorld(true, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
					else
					{
						m_utilities.createClusterObjectIntoWorld(false, true, false, glm::vec2(0), m_current_cluster_object_ptr, m_world_clusters, m_current_cluster_object_ptr->cluster_index, &m_physics_world, false, false);
						m_is_all_work_saved = false;
					}
				}
			}
		}
		else if (!m_enable_physics) {
			//if this object has a physics object attacted to it delete it
			if (m_current_cluster_object_ptr->physics_properties != nullptr) {
				m_utilities.destroryClusterObjectJoints(m_world_clusters, *m_current_cluster_object_ptr);
				m_current_cluster_object_ptr->physics_properties->dispose();
				//free up the memory
				delete m_current_cluster_object_ptr->physics_properties;
				m_current_cluster_object_ptr->physics_properties = nullptr;
			}
		}
	}
}