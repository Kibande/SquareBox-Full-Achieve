//#pragma once
//#include <SquareBox-Core/SquareBoxGameEngine.h>
//#include <ImGUI/imgui.h>
//#include <ImGUI/imgui_impl_sdl.h>
//#include <ImGUI/imgui_impl_opengl3.h>
//#include <ImGui/ImGuiFileBrowser.h>
//#include <sstream> //stringstream
//#include <array>
//#include <cstring>
//#include <string>
//#include <string.h>
//#include "Editor_Assistant.h"
//#include "CustomConsole.h"
//#include "FolderChangeTracker.h"
//#include <fstream>
//namespace SquareBoxEditor {
//	//TODO
//	enum SelectModeEnum
//	{
//		WORLDCLUSTER = 0,
//		CLUSTEROBJECT = 1,
//		FREESELECT = 2,
//		JOINTSSELECT = 3
//	};
//
//	enum TiledLayerSelectModeEnum {
//		SINGLETILE = 0,		//	???
//		MULTIPLETILES = 1	//	???
//	};
//	//the actual cluster object selection
//		/*
//			WORLDCLUSTER - Cluster Select, used to select many related objects at once(a fullCluster)
//			CLUSTEROBJECT - Object Select, used to select an indivdual cluster object and highlight its properties
//			FREESELECT - Free Select,used to select multiple cluster objects
//			JOINTSSELECT - Joints Select,used to create joints
//		*/
//	enum EditorModeEnum {
//		PLACE = 0,
//		SELECT = 1
//	};
//
//
//	enum ShapeTypeEnum {
//		SINGLESHAPE = 0,
//		COMPOUNDSHAPE = 1
//	};
//	/*
//		0 -	single Shapes like box, circle, capsule
//		1 - compound Shapes are shapes made from other shapes being combined , they are not really shapes , e.g
//		car
//		bicycle
//		A boat
//		A Moving machine , like the ones  that we have in super meat boy
//		Compound shapes have not yet been implemented , since there has not yet been any need for them
//	*/
//
//	enum RightTabDisplayEnum {
//		OBJECTPROPERTIES = 0,
//		LAYERING = 1
//	};
//
//	enum ObjectPropertiesEnum {
//		PHYSICS=0,
//		ANIMATION=1
//	};
//
//	enum LayeringEnum {
//		LAYERSTACK = 0,
//		LAYERPROPERTIES = 1
//	};
//	/*
//		0-Physics , properities of the currently active object
//		1-Layers , shows the layer arragements, visibility properties e.t.c
//		2-Layer Properties for a sepecific layers , mainly the layers tiling
//	*/
//
//	enum EditorThemeEnum {
//		ClassicTheme = 0,
//		LightTheme = 1,
//		DarkTheme = 2 //default theme
//	};
//
//	class LevelEditor_Screen :public SquareBox::IGameScreen
//	{
//	public:
//
//		LevelEditor_Screen(SquareBox::RenderEngine::Window* window_ptr_);
//
//	private:
//		/*methods*/
//	// Inherited via IGameScreen
//		virtual void build() override;
//
//		virtual void onEntry() override;
//
//		virtual void update(const float & deltaTime_) override;
//
//		virtual void draw() override;
//
//		virtual void onExit() override;
//
//		virtual void destroy() override;
//
//		virtual int getNextScreenIndex() const override;
//
//		virtual int getPreviousScreenIndex() const override;
//		void createNewLayer();
//		void createNewProject();
//		void cleanOutEditor();
//		// GUI
//		void initGUI();
//		void drawGUI();
//
//		void updateStates();  //make sure every objects state is respected and not accidentally changed
//		void checkPhysicsStatus(); // toggles the bodies physics status basing on m_enable_physics
//		void showMainMenu();
//
//		//Object Shape
//		int m_selected_shape = 0; //in reference to m_vec_shapes_pointer (vector of pointer of all shapes avaliable to us);
//
//		int m_active_layer_index; //the layer on which we are doing object placements right now
//		float m_free_select_rotation_padding = 0.0f;
//		float m_free_select_rotation_universal_angle = 0.0f;
//
//		//universal camera attributes
//		/*
//
//		These are referenced when creating a new camera , or
//		when recreating the world from stored files
//
//		*/
//
//		float m_universal_camera_scale = 162.0f;
//		glm::vec2 m_universal_camera_position = { 0.0f,0.0f };
//
//		//Object Type
//		int m_selected_body_type = 0;
//		/*
//		0 - Dynamic
//		1 - Static
//		2 - Kinematic
//		*/
//
//		RightTabDisplayEnum m_side_view = RightTabDisplayEnum::OBJECTPROPERTIES;
//		ObjectPropertiesEnum m_object_properties_enum = ObjectPropertiesEnum::PHYSICS;
//		LayeringEnum m_layering_enum = LayeringEnum::LAYERSTACK;
//		int m_selected_family = 1; //	families are used to filter out collisions
//
//		int m_selected_single_texture_index = 0;//	the single texture index we are currently on
//		int m_selected_tile_sheet_texture_index = 0;//	the tile Sheet texture index we are currently on
//
//
//		//in reference to the active layer
//		//	current index of the world cluster we are hovering over
//		int m_current_hovered_world_cluster_index = 0;
//		//	current index of the cluster object we are hovering over
//		int m_current_hovered_cluster_object_index = 0;
//
//		int m_place_mode_current_active_world_cluster_index = 0;//the WorldIndex that we are auto placing in new ClusterObjects
//		/*
//		Note: A cluster Object is a shell from the point it is pushed back in the cluster object vector 
//			  till the point when it is set to being alive i.e is_alive = true;
//
//			  A cluster object that was once alive and gets deleted, setting its is_alive to flase is 
//			  also considered to be a shell.
//		*/
//			//current pointer to the  WorldCluster and Clusterobjects we are working on
//
//		EditorThemeEnum m_theme_index = EditorThemeEnum::DarkTheme;
//		SquareBox::GWOM::Joint* m_current_joint_ptr = nullptr;
//		/*floats*/
//		int m_target_shift_to_layer_index = 0;// when shifting cluster objects from one layer to 
//		// another
//		/*bools*/
//		//Imgui file handling dialogs
//		bool m_show_open_level_file_dialog = false;
//		bool m_show_open_texture_file_dialog = false;
//		bool m_show_open_tile_sheet_file_dialog = false;
//		bool m_show_save_level_file_dialog = false;
//		//Modes
//		EditorModeEnum m_editor_mode_enum = EditorModeEnum::SELECT;
//		ShapeTypeEnum m_place_mode_shape_type = ShapeTypeEnum::SINGLESHAPE;
//
//		glm::ivec2 m_new_tiled_texture_tiling{ 0,0 };
//		SelectModeEnum m_selection_mode_index = SelectModeEnum::CLUSTEROBJECT;
//		TiledLayerSelectModeEnum m_tiled_layer_selection_mode_index = TiledLayerSelectModeEnum::SINGLETILE;
//		int m_active_sub_texture_key = 0;//This is the texture that is being applied to the next tile when we click
//		// leave the default set to 0 so that we can have some black sub textures if we want to
//		bool m_debug_mode = false;
//
//		bool m_respect_ancestor = false;
//		bool m_auto_assign_world_cluster = true;//if set to false a user can mannually choose the world cluster to be adding cluster objects too
//		int  m_mannually_choosen_world_cluster_index = -1;//int<0 means that its not yet  assigned
//		bool m_show_set_mannual_wc_index_dialog = false;
//		bool m_is_all_work_saved = true;
//		bool m_allow_exit = false;//bool to see if we can exit the leveleditor
//		bool m_request_exit = false;
//		bool m_play_simulation = false;//controls physics and animations update loops
//
//		bool m_draw_plotted_shape = false;// determines if its time to draw the chain or polygon, or whose coordinates we have so
//		bool m_show_polygon_plots = false;
//
//		bool m_debug_show_joints = false;
//		bool m_enable_physics = true;
//		bool m_show_world_locked_dialog = false;
//		bool m_show_failed_to_save_world_dialog = false;
//		bool m_show_create_new_project_dialog = false;
//		bool m_show_about_dialog = false;
//		bool m_show_key_bindings_dialog = false;
//		bool m_show_shift_cobjs_to_another_layer_dialog = false;
//		bool m_show_update_layer_tiling_dialog = false;
//		bool m_always_draw_edges = true;//helps with makinjg edges act like lines
//		bool m_show_all_debug_lines = true;//alternative is showing only for the active layer
//		bool m_show_scaling_in_debug_mode = true;		
//		bool m_show_grid_lines = false;
//		bool m_show_console = false;
//
//		/*vectors*/
//		std::vector<SquareBox::IShape*> m_vec_shapes_pointer;
//
//		
//		std::vector<glm::vec2> m_vector_of_joint_ploting_points;
//		glm::vec2 clustermousepos; // used when moving multiple cluster objects at once
//		std::vector<SquareBox::GWOM::Layer> m_layers;
//		std::vector<std::pair<int, int>> m_selected_cluster_objects;
//		std::vector<std::pair<int,int>> m_selected_tiles; // Used for tiling layer layout (layer_index_tile_index)
//		/*Externals*/
//		//debug colors
//		SquareBox::RenderEngine::ColorRGBA8 m_debug_static_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_debug_dynamic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_debug_kinetic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 0, 255, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_selected_body_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_place_mode_sketch_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::antique_white);
//		SquareBox::RenderEngine::ColorRGBA8 m_plot_coordinates_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_not_physics_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255);
//		SquareBox::RenderEngine::ColorRGBA8 m_unknown_body_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::violet);
//		SquareBox::RenderEngine::ColorRGBA8 m_drag_selection_box_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon);
//		SquareBox::RenderEngine::ColorRGBA8 m_layer_grid_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white);
//		SquareBox::RenderEngine::ColorRGBA8 m_border_color;
//		SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
//		SquareBox::RenderEngine::SpriteFont m_sprite_font;
//		SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
//		SquareBox::AssetManager::GLTexture m_texture;
//		SquareBox::RenderEngine::GLSLProgram m_texture_program;
//		SquareBox::RenderEngine::GLSLProgram m_light_program;
//		SquareBox::RenderEngine::GLSLProgram m_debug_program;
//		glm::vec2  m_drag_select_origin;
//		bool m_is_drag_selecting = false;
//		SquareBox::GWOM::LevelReaderWriter m_level_reader_writer;
//		imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
//
//		/*poiners*/
//		SquareBox::IShape* m_current_shape_ptr = nullptr;// a pointer to the current world shape
//		SquareBox::GWOM::ClusterObject* m_current_cluster_object_ptr = nullptr;
//		SquareBox::RenderEngine::Window* m_window = nullptr;
//		//the physics simulation engine
//		SquareBox::PhysicsCollisionEngine::PhysicsWorld m_physics_world;
//		SquareBox::GWOM::ClusterObject m_temp_bridge_joints_object;
//		//AI
//		//Automation
//		SquareBox::AI::Automation m_automation;
//		SquareBox::AnimationSystem::AnimationCreator m_animation_creator;
//		std::string m_animation_script = "Assets/Animations/animations_file.lua";
//		std::string m_automation_script = "Assets/Animations/automation_file.lua";
//		std::vector<std::pair<int,std::pair<int,int>>> m_cluster_objects_to_delete; //[layer_index[world_cluster_index,cluster_object_index]]
//		std::vector<std::pair<int,int>> m_world_clusters_to_delete;
//		/* 
//			it would be nice to add the ability to load animation and automation scripts from any where
//		*/
//		std::string m_level_file_extension = ".dat,.sbx";
//		std::string m_texture_file_extensions = ".png,.jpg";
//
//
//		std::string m_file_path_to_track = "";
//		//Assistant
//		SquareBox::Utilities m_utilities;
//		Editor_Assistant m_editor_assitant;
//		SquareBoxEditor::CustomConsole m_console;
//		const char** shapes_labels_ptr = nullptr;
//		/*
//			We should implement a cache friendly way of allocation all our
//			PhysicsEngines
//			and Joints
//			the current scatered new and delete are slow are spread out the memory in ram , making it hard
//			for caching to help us make their accesing faster , so will shall just be allocating and array of
//			new PhysicsEngines and Joints , this will ensure that we have continous memory , hence a cache friendly
//			approach
//
//			Known Bugs
//			The deletion in tiled layers is not reliable
//
//
//			TODOs
//			Fix the Joints System
//			Add an in buit animation system
//		*/
//
//
//
//
//		//Controls
//
//			//select mode handles selecting things and dragging them
////	tiled layers
//									/* Place Mode */
//		int tiled_layer_place_mode_fill_tile = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//		/* Selection Mode */
//		int tiled_layer_select_mode_select_tile = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//
//		//	no tiled
//											/* Selection Mode */
//		int world_cluster_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//		int cluster_object_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//		int free_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//		int joint_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//
//
//
//		// free drag selecting box draging input keys combo
//		// we need to draw a debug box for this !!
//		int free_selection_drag_input_key_1 = static_cast<int>(SquareBox::KeyBoardEnum::SPACE);
//		int free_selection_drag_input_key_2 = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);
//
//
//		//	world cluster selection and free selection dragging activation input key
//		int world_cluster_and_free_selection_dragging_activation_input_key = static_cast<int>(SquareBox::KeyBoardEnum::SPACE); // i want to remove this 
//		//control in future
//		//	world cluster selection and free selection dragging  input key
//		int world_cluster_and_free_selection_dragging_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//
//		// world cluster and free selection copying combo
//		int left_ctrl_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL);
//		int right_ctrl_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_CTRL);
//		int c_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEY_c);
//
//		//cluster object drag select mode 
//		int cluster_object_drag_selection_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);
//
//		/* Place Mode */
//		int non_tiled_layer_placement_input_ley = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
//
//
//
//
//		/* Use full key board short cuts */
//
//		//clearing selection modes excpet when in cluster object select
//		int clear_selection_input_key_1 = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT);
//		int clear_selection_input_key_2 = static_cast<int>(SquareBox::KeyBoardEnum::KEY_c);
//
//		//deletion key
//		int delete_selection_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEYBOARD_DELETE);
//
//		int bridge_plotting_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);
//		int pulley_plotting_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);
//	};
//}
//
