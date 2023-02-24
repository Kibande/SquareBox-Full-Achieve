#include "AnimationEditor.h"


AnimationEditor::AnimationEditor()
{
}

AnimationEditor::~AnimationEditor()
{
}

int AnimationEditor::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int AnimationEditor::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void AnimationEditor::build()
{
	SBX_INFO("Animation Editor Screen loaded into memory");
}

void AnimationEditor::onEntry()
{
	//Init Shaders
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();
	//the debugProgram
	m_debug_program.compileDefaultDebugShaders();
	m_debug_program.addDefaultDebugAttributes();
	m_debug_program.linkShaders();

	m_debug_renderer.init();



	m_utilities.init();
	m_animation_creator.init();

	initGUI();

	//The shapes we can create and edit in the world
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::BoxShape());
	m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CircleShape());
	shapes_labels_ptr = new const char* [m_vec_shapes_pointer.size()];
	/*
	 we only have one layer in this would set up
	
	*/
	m_layers.emplace_back();
	m_layers.back().index = 0;
	m_layers.back().alive_cluster_objects.clear();
	m_layers.back().alive_cluster_objects.reserve(60);//some cache friendlness
	m_layers.back().layer_type = SquareBox::LayerTypeEnum::FlatLayer;

	//create the layers first world cluster and cluster object
	//set up the first world cluster in this layer
	m_layers.back().world_clusters.emplace_back();
	auto& world_cluster = m_layers.back().world_clusters.back();
	world_cluster.index = 0;
	m_utilities.nameWorldClusterByIndex(world_cluster);

	//set up the first cluster object in the world cluster
	world_cluster.cluster_objects.emplace_back();
	auto& cluster_object = world_cluster.cluster_objects.back();
	cluster_object.cluster_index = world_cluster.index;
	cluster_object.index = 0;
	cluster_object.layer_index = m_layers.back().index; //  give the cluster object its layer index
	m_utilities.nameClusterObjectByIndex(cluster_object);

	m_current_cluster_object_ptr = &cluster_object;


	//Init Cameras
	glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_layers.back().camera.init(screen_dimensions.x, screen_dimensions.y);
	m_layers.back().camera.setScale(160.0f);//The Zoom of the Camera
	m_layers.back().camera.setPosition(screen_dimensions * glm::vec2(0.5f));
}

void AnimationEditor::update(const float& deltaTime_)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.WantCaptureMouse == 1 || io.WantCaptureKeyboard == 1)
	{
		//stop our game engine from processing input
		m_game_ptr->setProcessingInput(false);
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
		}
	}
	else {
		m_game_ptr->setProcessingInput(true);
	}

	if (m_game_ptr->isProcessingInput())
	{

		m_editor_assistant.cameraControls(m_layers.back().camera, m_game_ptr);

		const std::vector<std::pair<int, int>>& active_layer_alive_cluster_objects = m_layers.back().alive_cluster_objects;

		glm::vec2& mouse_in_world = m_layers.back().camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

		if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {




			bool hoveringOverCluster = false;
			bool hoveringOverClusterObject = false;
			for (unsigned int i = 0; i < active_layer_alive_cluster_objects.size(); i++)
			{
				SquareBox::GWOM::ClusterObject& ccobj = m_layers.back().world_clusters[active_layer_alive_cluster_objects[i].first].cluster_objects[active_layer_alive_cluster_objects[i].second];
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
				//Cluster object Selection
				//this is the only place where we actually set the active worldClusters and Objects
				//if we are hovering over a cluster object and we Left click let that object become the current active object in our world scene
				if (hoveringOverClusterObject && m_game_ptr->getInputDevice()->isInputIdReceived(cluster_object_selection_input_key)) {
					m_current_cluster_object_ptr = &m_layers.back().world_clusters[m_current_hovered_world_cluster_index].cluster_objects[m_current_hovered_cluster_object_index];
					updateStates();//this updates the imGui variables
					m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
					m_selected_cluster_objects.clear();
					m_utilities.addPairToVector(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index));
				}

				//dragging and deletion

				if (m_current_cluster_object_ptr != nullptr) {
					if (m_utilities.isPairVectorMember(m_selected_cluster_objects, std::pair(m_current_cluster_object_ptr->cluster_index, m_current_cluster_object_ptr->index))) {
						if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(cluster_object_drag_selection_input_key))
						{
							m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
							//we are trusting our program to always make the selected object the current object ,which it does perfectly
							if (m_current_shape_ptr->is_plotted || m_current_shape_ptr->is_calculated_light) {
								//need to do the logic for displacing edges,chains and ploygons and bridges since
								//we want to displace the vertices basinf on the mouses motion
								m_utilities.createClusterObjectIntoWorld(true, true, true, mouse_in_world, m_current_cluster_object_ptr, m_layers, &m_physics_world, false, false);
								m_is_all_work_saved = false;
							}
							else
							{
								m_utilities.createClusterObjectIntoWorld(false, true, true, mouse_in_world, m_current_cluster_object_ptr, m_layers, &m_physics_world, false, false);
								m_is_all_work_saved = false;
							}
						}
					}


					if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(delete_selection_input_key)) {
						//Delete selected
							for (unsigned i = 0; i < m_selected_cluster_objects.size(); i++)
							{
								auto& target_cluster_object = m_layers.back().world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
								std::pair<int, int> location_pair = std::pair<int, int>(target_cluster_object.cluster_index, target_cluster_object.index);
								std::pair<int, std::pair<int, int>> master_pair = std::pair<int, std::pair<int, int>>(target_cluster_object.layer_index, location_pair);
								m_cluster_objects_to_delete.push_back(master_pair);
							}
							m_selected_cluster_objects.clear();
					}
				}
		}
		else if (m_animation_editor_mode == AnimationEditorModeEnum::PLACE) {
			if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
				//place plotting points for the ploted shapes , and draw the other objects
				SquareBox::Utilities::creationDetails details = m_utilities.createClusterObjectIntoWorld(false, false, true, mouse_in_world, m_current_cluster_object_ptr, m_layers, &m_physics_world, true, m_respect_ancestor);
				//add to m_alive_cluster_objects Objects if its alive
				if (details.settlementCoordinates.first >= 0 && details.settlementCoordinates.second >= 0) {
					if (m_layers.back().world_clusters[details.settlementCoordinates.first].cluster_objects[details.settlementCoordinates.second].is_alive) {
						m_utilities.addPairToVector(m_layers.back().alive_cluster_objects, details.settlementCoordinates);
						m_is_all_work_saved = false;
					}
					else {
						SBX_ERROR("Tried Adding a Dead Object to m_aliveClusterObjects");
						__debugbreak();
					}
				}
				//get a pointer to the shell
				if (details.shellCoordinates.first >= 0 && details.shellCoordinates.second >= 0) {
					m_current_cluster_object_ptr = &m_layers.back().world_clusters[details.shellCoordinates.first].cluster_objects[details.shellCoordinates.second];
				
					/*
						to be uncommented when we add the physics engine here
						checkPhysicsStatus(layers_, active_layer_index_, m_animation_editor_mode);
					*/
					
					updateStates();
				}

			}
		}


		//forcing imgui to start responding
		if (m_game_ptr->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::ESCAPE))) {
			ImGui::SetNextFrameWantCaptureMouse(true);
			ImGui::SetNextFrameWantCaptureKeyboard(true);
			/* this is the hack that i have come up with to solve the issue of imgui freezing up
			and not allowing any more user input, this block of code  forces it to start
			taking user input but it disoragnises sdls input manager
			so this function only helps you be able to save your work */
		}
	}

	m_animation_creator.update(deltaTime_, m_layers.back().world_clusters, m_layers.back().alive_cluster_objects, m_game_ptr->getFps(), m_game_ptr->getGameLoopElapsedSeconds(), m_game_ptr->getInputDevice(), m_play_simulation);

	bool deleted_a_cluster_object = false;
	// deleting objects
	for (unsigned int i = 0; i < m_cluster_objects_to_delete.size(); i++)
	{
		auto& target_layer = m_layers[m_cluster_objects_to_delete[i].first];

		auto& target_cluster_object_coordinates = m_cluster_objects_to_delete[i].second;
		auto& target_cluster_object = target_layer.world_clusters[target_cluster_object_coordinates.first].cluster_objects[target_cluster_object_coordinates.second];

		m_utilities.deleteClusterObjectFromWorld(m_layers, target_cluster_object, &m_physics_world);
		deleted_a_cluster_object = true;
	}
	m_cluster_objects_to_delete.clear();
	if (deleted_a_cluster_object) {
		m_selected_cluster_objects.clear();
	}

	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	//Called once every game loop , and updates what will be drawn
	m_layers.back().camera.update(screen_dimensions.x, screen_dimensions.y);


	if (m_game_ptr->isProcessingInput()) {
		if (m_animation_editor_mode == AnimationEditorModeEnum::PLACE) {

		}
		else if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {


		}
	}

	updateStates();
}

void AnimationEditor::draw()
{
	m_sprite_batch.begin(SquareBox::RenderEngine::NONE);

	// Render textures
	std::vector<std::pair<int, int>> vector_of_alive_cluster_objects = m_layers.back().alive_cluster_objects;

	for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
	{
		SquareBox::GWOM::ClusterObject& cwobj = m_layers.back().world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

		m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);

		if (m_current_shape_ptr->needs_height_width) {
			if (m_layers.back().camera.isBoxInView(cwobj.position, glm::vec2(cwobj.width, cwobj.height))) {
				m_current_shape_ptr->draw(cwobj, m_sprite_batch, m_layers.back().opacity);
			}
		}
		else if (m_current_shape_ptr->needs_radius) {
			if (m_layers.back().camera.isBoxInView(cwobj.position, glm::vec2(cwobj.radius * 2, cwobj.radius * 2))) {
				m_current_shape_ptr->draw(cwobj, m_sprite_batch, m_layers.back().opacity);
			}
		}

	}


	m_sprite_batch.end();
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_layers.back().camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	//debug draw

	m_debug_renderer.begin();
	if (m_debug_mode) {
	
		// +X axis
		glm::vec2 debug_center = m_layers.back().camera.getPosition();
		float camera_width = m_layers.back().camera.getCameraDimensions().x;
		float camera_height = m_layers.back().camera.getCameraDimensions().y;

		//+X
		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x + (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 200));
		// -X axis
		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x - (camera_width * 0.5), debug_center.y), SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 100));

		// +Y axis
		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y + (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 200));
		// -Y axis
		m_debug_renderer.drawLine(debug_center, glm::vec2(debug_center.x, debug_center.y - (camera_height * 0.5)), SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 100));


		for (unsigned int i = 0; i < vector_of_alive_cluster_objects.size(); i++)
		{
			SquareBox::GWOM::ClusterObject& cwobj = m_layers.back().world_clusters[vector_of_alive_cluster_objects[i].first].cluster_objects[vector_of_alive_cluster_objects[i].second];

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
		}

	}

	if (m_animation_editor_mode == AnimationEditorModeEnum::PLACE && m_current_cluster_object_ptr != nullptr) {
		/*
			m_current_cluster_object_ptr my be null if we are just coming from select mode
		*/
		glm::vec2 mouse_in_world = m_layers.back().camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

		m_utilities.setCurrentShapePointer(m_current_cluster_object_ptr->shape, &m_current_shape_ptr);
		m_current_shape_ptr->traceDraw(*m_current_cluster_object_ptr, m_debug_renderer, mouse_in_world, m_place_mode_sketch_color, m_layers.back().camera.getScale());
	}
	else if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {
		//drawing the selected objects debug lines
		//are we among the selected Objects
		for (unsigned int i = 0; i < m_selected_cluster_objects.size(); i++)
		{
			SquareBox::GWOM::ClusterObject& cwobj = m_layers.back().world_clusters[m_selected_cluster_objects[i].first].cluster_objects[m_selected_cluster_objects[i].second];
			m_utilities.setCurrentShapePointer(cwobj.shape, &m_current_shape_ptr);
			m_current_shape_ptr->debugDraw(cwobj, m_debug_renderer, m_selected_body_color);
		}

	}

	m_debug_renderer.end();
	m_debug_program.use();
	uploadCameraInfo(&m_debug_program, &m_layers.back().camera, "P");
	m_debug_renderer.render();
	m_debug_program.unuse();

	drawGUI();//this is where is_locked changes from
	//having this here avoids Imgui end frame crashes
}

void AnimationEditor::onExit()
{
	//this is where all screen process are stopped
//this is where objects that the screen used are destroyed from
	m_sprite_batch.dispose();
	m_debug_renderer.dispose();

	m_texture_program.dispose();
	m_utilities.dispose();
	m_debug_program.dispose();
	m_animation_creator.dispose();

	disposeGUI();

	//need a better fix for a clean up here
	m_texture_program = SquareBox::RenderEngine::GLSLProgram();
	m_debug_program = SquareBox::RenderEngine::GLSLProgram();
}

void AnimationEditor::destroy()
{
}

void AnimationEditor::initGUI()
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
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_game_ptr->getWindow()->getWindowPointerForImGui(), m_game_ptr->getWindow()->getGLContextPointerForImGui());
	ImGui_ImplOpenGL3_Init(glsl_version);
	//Load Fonts
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Comfortaa-Medium.ttf", 16.0f);
	if (font == NULL) {
		SBX_CORE_ERROR("Failed to load font Comfortaa-Medium.ttf ");
	}

	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
}

void AnimationEditor::disposeGUI() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void AnimationEditor::drawGUI()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_game_ptr->getWindow()->getWindowPointerForImGui());
	ImGui::NewFrame();
	showMenuMain();
	ImGuiWindowFlags physics_tab_window_flags = 0;
	physics_tab_window_flags |= ImGuiWindowFlags_NoMove;
	bool* physics_tab_open = false;




	ImGui::Begin("Control Panel", physics_tab_open, physics_tab_window_flags);

	ImGui::SetWindowPos(ImVec2(m_game_ptr->getWindow()->getScreenWidth() - ImGui::GetWindowWidth() - 2, m_main_menu_bar_height));
	{
		m_controls_panel_width = ImGui::GetWindowWidth();

		ImGui::Text("FPS Counter: %i", static_cast<int>(m_game_ptr->getFps()));
		ImGui::Text("Elapsed Sec: %i", m_game_ptr->getGameLoopElapsedSeconds());
		ImGui::Separator();
		static int mode = 0;
		int beforeMode = mode;
		ImGui::RadioButton("Place Mode  ", &mode, 0); ImGui::SameLine();
		ImGui::RadioButton("Select Mode ", &mode, 1); ImGui::SameLine();

		m_animation_editor_mode = static_cast<AnimationEditorModeEnum>(mode);

		if (beforeMode != mode) {
			if (m_animation_editor_mode == AnimationEditorModeEnum::PLACE) {
				m_current_cluster_object_ptr = &m_layers.back().world_clusters.back().cluster_objects.back();
			
			}
			else if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {
				m_current_cluster_object_ptr = nullptr;
				m_selected_cluster_objects.clear();
			}
		}
		ImGui::Checkbox("Debug Mode", &m_debug_mode);

		ImGui::Spacing();
		float m_universal_camera_scale = m_layers.back().camera.getScale();
		ImGui::DragFloat("Zoom", &m_universal_camera_scale, 1, 0, 9000, "%.3f");
		m_layers.back().camera.setScale(m_universal_camera_scale);
		ImGui::Spacing();


		if (m_animation_editor_mode==AnimationEditorModeEnum::SELECT && m_selected_cluster_objects.size()>0) {
			ImGui::Spacing();
			if (ImGui::Button("Clear Selection")) {
				m_current_cluster_object_ptr = nullptr;
				m_selected_cluster_objects.clear();
			}
			ImGui::Spacing();
		}
		if (ImGui::BeginTabBar("Object Specifications"))
		{

			if (ImGui::BeginTabItem("Animations"))
			{
				
				if (m_play_simulation) {
					if (ImGui::Button("Pause Simulation")) {
						m_play_simulation = false;
					}
					
				}
				else {
					if (ImGui::Button("Play Simulation")) {
						m_play_simulation = true;
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Properties"))
			{
				if (m_current_cluster_object_ptr != nullptr) {

					ImGui::Spacing();
					ImGui::Text("Cluster");
					ImGui::InputText("###CurrentWorldClustersName", m_layers.back().world_clusters[m_current_cluster_object_ptr->cluster_index].name, 20);
					ImGui::Text("Object name");
					ImGui::InputText("###CurrentClusterObjectsName", m_current_cluster_object_ptr->name, 20);
					
					ImGui::Separator();

					ImGui::Spacing();
					//display our shapes names
			//Object Types
					ImGui::Spacing();
					//display our shapes names
					ImGui::Text("Shape     : "); ImGui::SameLine();

					for (unsigned int i = 0; i < m_vec_shapes_pointer.size(); i++)
					{
						shapes_labels_ptr[i] = ToString(m_vec_shapes_pointer[i]->body_shape);
					}
					if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {
						ImGui::BeginDisabled();
					}
					ImGui::Combo("###Shape", &m_selected_shape, shapes_labels_ptr, m_vec_shapes_pointer.size());
					if (m_animation_editor_mode == AnimationEditorModeEnum::SELECT) {
						ImGui::EndDisabled();
					}

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
							ImGui::Text("Radius    : "); ImGui::SameLine();
							ImGui::InputFloat("###c_r", &m_current_cluster_object_ptr->radius, 0.025f, 0, "%.3f");
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
						vc.reserve(m_layers.back().single_textures.size());
						std::transform(m_layers.back().single_textures.begin(), m_layers.back().single_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
						{
							//this new here is never deleted, and is called alot, that is a problem to us 
							char* pc = new char[parentTexture_.texture.display_name.size() + 1];
							std::strcpy(pc, parentTexture_.texture.display_name.c_str());
							return pc;
						});
						//compute how many single textures we want to display
						//we are only displaying those that are not empty strings
						if (m_layers.back().single_textures.size() > 0) {
							ImGui::Combo("Single", &m_selected_single_texture_index, &vc[0], m_layers.back().single_textures.size());
							//update the current objects texture info
							m_current_cluster_object_ptr->texture_info.texture_index = m_selected_single_texture_index;
							m_current_cluster_object_ptr->texture_info.texture_id = m_layers.back().single_textures[m_selected_single_texture_index].texture.id;
							//current active texture
							ImTextureID my_tex_id;
							my_tex_id = (ImTextureID)m_layers.back().single_textures[m_selected_single_texture_index].texture.id;

							float my_tex_w = 50;
							float my_tex_h = 50;
							float zoom = 4.0f;
							float texture_width = m_layers.back().single_textures[m_selected_single_texture_index].texture.width;
							float texture_height = m_layers.back().single_textures[m_selected_single_texture_index].texture.height;

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
							ImGui::Combo("Single", &m_selected_single_texture_index, "Empty", m_layers.back().single_textures.size());
						}

						if (ImGui::Button("+ Single")) {
							m_show_open_single_texture_file_dialog = true;
						}
						ImGui::NewLine();
						ImGui::NewLine();
					}
					else if (m_current_cluster_object_ptr->texture_info.texture_type == SquareBox::TextureEnum::TILESHEET) {
						if (m_layers.back().tiled_textures.size() > 0) {
							ImGui::Text("Tiling:");
							ImGui::Text("cols : "); ImGui::SameLine();
							ImGui::Text("%d", m_layers.back().tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.x);
							ImGui::Text("rows : "); ImGui::SameLine();
							ImGui::Text("%d", m_layers.back().tiled_textures[m_selected_tile_sheet_texture_index].texture.tiling.y);

							std::vector<char*>  vc;
							vc.reserve(m_layers.back().tiled_textures.size());
							std::transform(m_layers.back().tiled_textures.begin(), m_layers.back().tiled_textures.end(), std::back_inserter(vc), [](const SquareBox::GWOM::ParentTexture& parentTexture_)
							{
								//this new here is never deleted, and is called alot, that is a problem to us 
								char* pc = new char[parentTexture_.texture.display_name.size() + 1];
								std::strcpy(pc, parentTexture_.texture.display_name.c_str());
								return pc;
							});

							ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, &vc[0], m_layers.back().tiled_textures.size());
							//update the current objects texture info
							auto& selected_tile_sheet_texture = m_layers.back().tiled_textures[m_selected_tile_sheet_texture_index].texture;
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
							glm::vec4 uvRect = selected_tile_sheet_texture.getUVReactAtIndex(m_current_cluster_object_ptr->texture_info.tile_sheet_index, glm::ivec2(0));

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
							ImGui::Combo("TileSheet", &m_selected_tile_sheet_texture_index, "Empty", m_layers.back().tiled_textures.size());
						}

						ImGui::Separator();

						ImGui::Text("New  TileSheet Tiling:");
						ImGui::Text("cols : "); ImGui::SameLine();
						ImGui::InputInt("###cols", &m_new_tiled_texture_tiling.x);
						ImGui::Text("rows : "); ImGui::SameLine();
						ImGui::InputInt("###rows", &m_new_tiled_texture_tiling.y);

						if (ImGui::Button("+ TileSheet")) {
							m_show_open_tile_sheet_texture_file_dialog = true;
						}

						ImGui::NewLine();
						ImGui::NewLine();

					}
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}


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
			ImGui::EndPopup();
		}

		if (m_show_open_animation_script_file_dialog) {
			ImGui::OpenPopup("Open Animation Script File");
		}

		if (m_show_about_dialog) {
			ImGui::OpenPopup("About?");
		}

		if (m_show_save_animation_script_file_dialog) {
			ImGui::OpenPopup("Save Animation Script File");
		}

		if (m_show_open_single_texture_file_dialog) {
			ImGui::OpenPopup("Open Single Texture File");
		}

		if (m_show_open_tile_sheet_texture_file_dialog) {
			ImGui::OpenPopup("Open Tile Sheet Texture File");
		}

	
		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */

		 /*
		   this simple texture opening system gives us the ability to edit texture properties straight out of the box
		 */
		if (ImGui::BeginPopupModal("About?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("ABOUT:");
			ImGui::BulletText("This is a 2D Graphics User Interface Editing tool");
			ImGui::BulletText("Built and maintained by stande studios ");
			ImGui::BulletText("Established 2021");

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		

		if (m_file_dialog.showFileDialog("Open Single Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			auto temp_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(temp_texture.id, m_tiling);// tiling can always be overwitten

			m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			m_texture.asset_file_path = m_file_dialog.selected_path;
			m_texture.display_name = m_file_dialog.selected_fn;

			/*
				if we want to be saving the textures display names . then
				SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(m_texture.id, m_file_dialog.selected_fn);
			*/

			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_layers.back().single_textures.size(); i++)
			{
				if (m_layers.back().single_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {
				SquareBox::GWOM::ParentTexture new_parent_texture;
				new_parent_texture.texture_index = m_layers.back().single_textures.size();
				new_parent_texture.texture = m_texture;
				m_layers.back().single_textures.push_back(new_parent_texture);

				//add to the sub_textures_table for all layers , since a none tiled can become tiled layer
				SquareBox::GWOM::SubTexture new_sub_texture;
				new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
				new_sub_texture.parent_type = SquareBox::TextureEnum::SINGLE;
				new_sub_texture.sub_texture_key = m_layers.back().sub_textures_table.size();
				m_layers.back().sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
			}
		}

		if (m_file_dialog.showFileDialog("Open Tile Sheet Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_texture_file_extensions))
		{
			auto temp_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(temp_texture.id, m_tiling);// tiling can always be overwitten

			m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
			m_texture.asset_file_path = m_file_dialog.selected_path;
			m_texture.display_name = m_file_dialog.selected_fn;


			//first confirm that we don't have this texture already
			bool isAlreadyPresent = false;

			for (size_t i = 0; i < m_layers.back().tiled_textures.size(); i++)
			{
				if (m_layers.back().tiled_textures[i].texture.asset_file_path == m_texture.asset_file_path) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {
				SquareBox::GWOM::ParentTexture new_parent_texture;
				new_parent_texture.texture_index = m_layers.back().tiled_textures.size();
				new_parent_texture.texture = m_texture;
				// we do this here so that we do not accidently over write a textures properties
				new_parent_texture.texture.tiling = m_new_tiled_texture_tiling;
				m_new_tiled_texture_tiling.x = 0;
				m_new_tiled_texture_tiling.y = 0;

				SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(new_parent_texture.texture.id, new_parent_texture.texture.display_name);
				SquareBox::AssetManager::TextureManager::setTextureTilingById(new_parent_texture.texture.id, new_parent_texture.texture.tiling);

				if (new_parent_texture.texture.tiling.x > 0 && new_parent_texture.texture.tiling.y > 0) {
					m_layers.back().tiled_textures.push_back(new_parent_texture);
					//loop through all the tilesheet indiviaual grids adding one by one
					int num_sections = new_parent_texture.texture.tiling.x * new_parent_texture.texture.tiling.y;
					int initial_sub_texture_key = m_layers.back().sub_textures_table.size();
					for (int i = 0; i < num_sections; i++)
					{
						SquareBox::GWOM::SubTexture new_sub_texture;
						new_sub_texture.parent_texture_index = new_parent_texture.texture_index;
						new_sub_texture.parent_type = SquareBox::TextureEnum::TILESHEET;
						new_sub_texture.tiling_index = i;
						new_sub_texture.sub_texture_key = initial_sub_texture_key + i;
						m_layers.back().sub_textures_table[new_sub_texture.sub_texture_key] = new_sub_texture;
					}
				}
				else {
					SBX_ERROR("Failed to Added TileSheet {} to Editor, Invalid Tiling", new_parent_texture.texture.display_name);
				}
			}
		}

		if (m_file_dialog.showFileDialog("Save Animation Script File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), m_animation_script_extensions))
		{
			SBX_INFO("Saving Animation Script as {} File", m_animation_script_extensions);

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

			
		}

		if (m_file_dialog.showFileDialog("Open Animation Script File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), m_animation_script_extensions))
		{
			m_animation_creator.loadAnimationScript(m_file_dialog.selected_path);
		}
		//reset of all dialogs
		m_show_open_animation_script_file_dialog = false;
		m_show_save_animation_script_file_dialog = false;
		m_show_open_single_texture_file_dialog = false;
		m_show_open_tile_sheet_texture_file_dialog = false;
		m_show_about_dialog = false;
	}

	ImGui::End();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void AnimationEditor::showMenuMain()
{
	ImGui::BeginMainMenuBar();
	m_main_menu_bar_height = ImGui::GetWindowHeight();
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("About", NULL)) {
			m_show_about_dialog = true;
		}
		if (ImGui::MenuItem("Open", "CTRL+O")) {
			m_show_open_animation_script_file_dialog = true;
		}
		if (ImGui::MenuItem("Save", "CTRL+S")) {
			m_show_save_animation_script_file_dialog = true;
		}
		
		if (ImGui::MenuItem("Home", NULL)) {
			setScreenState(SquareBox::ScreenState::CHANGE_PREVIOUS);
		}
		if (ImGui::MenuItem("Quit", NULL)) {
			m_game_ptr->initiateExit();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void AnimationEditor::updateStates()
{
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
	}
}

void AnimationEditor::animationElementShapeShellSetter(SquareBox::GWOM::GUIElement& gui_element_)
{
}

void AnimationEditor::cleanOutAnimationLayer()
{
}




