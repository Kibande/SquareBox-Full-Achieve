#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include "LayerTypeInterface.h"
#include "Editor_Assistant.h"
enum SelectModeEnum
{
	WORLDCLUSTER = 0,
	CLUSTEROBJECT = 1,
	FREESELECT = 2,
	JOINTSSELECT = 3
};

//the actual cluster object selection
	/*
		WORLDCLUSTER - Cluster Select, used to select many related objects at once(a fullCluster)
		CLUSTEROBJECT - Object Select, used to select an indivdual cluster object and highlight its properties
		FREESELECT - Free Select,used to select multiple cluster objects
		JOINTSSELECT - Joints Select,used to create joints
	*/
enum ShapeTypeEnum {
	SINGLESHAPE = 0,
	COMPOUNDSHAPE = 1
};
/*
	0 -	single Shapes like box, circle, capsule
	1 - compound Shapes are shapes made from other shapes being combined , they are not really shapes , e.g
	car
	bicycle
	A boat
	A Moving machine , like the ones  that we have in super meat boy
	Compound shapes have not yet been implemented , since there has not yet been any need for them
*/

enum ObjectPropertiesEnum {
	PHYSICS = 0,
	ANIMATION = 1
};


class FlatLayer: 
	public LayerTypeInterface
{
public:
	FlatLayer();
	~FlatLayer();
	void updateStates();  //make sure every objects state is respected and not accidentally changed
	void checkPhysicsStatus(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_); // toggles the bodies physics status basing on m_enable_physics

	//Object Shape
	int m_selected_shape = 0; //in reference to m_vec_shapes_pointer (vector of pointer of all shapes avaliable to us);

	float m_free_select_rotation_padding = 0.0f;
	float m_free_select_rotation_universal_angle = 0.0f;

	//Object Type
	int m_selected_body_type = 0;
	/*
	0 - Dynamic
	1 - Static
	2 - Kinematic
	*/

	ObjectPropertiesEnum m_object_properties_enum = ObjectPropertiesEnum::PHYSICS;

	int m_selected_family = 1; //	families are used to filter out collisions

	int m_selected_single_texture_index = 0;//	the single texture index we are currently on

	int m_selected_tile_sheet_texture_index = 0;//	the tile Sheet texture index we are currently on

	//in reference to the active layer
	//	current index of the world cluster we are hovering over
	int m_current_hovered_world_cluster_index = 0;
	//	current index of the cluster object we are hovering over
	int m_current_hovered_cluster_object_index = 0;

	/*
	
	Note: A cluster Object is a shell from the point it is pushed back in the cluster object vector
		  till the point when it is set to being alive i.e is_alive = true;

		  A cluster object that was once alive and gets deleted, setting its is_alive to flase is
		  also considered to be a shell.
	*/
	//current pointer to the  WorldCluster and Clusterobjects we are working on

	//i intend to make this a smart pointer when i learn more about them 
	SquareBox::GWOM::Joint* m_current_joint_ptr = nullptr;
	SquareBox::GWOM::Joint m_joint_template; // used when createding a new joint between objects
	ShapeTypeEnum m_place_mode_shape_type = ShapeTypeEnum::SINGLESHAPE;

	SelectModeEnum m_selection_mode_index = SelectModeEnum::CLUSTEROBJECT;

	bool m_auto_assign_world_cluster = true;//if set to false a user can mannually choose the world cluster to be adding cluster objects too
	int  m_mannually_choosen_world_cluster_index = -1;//int<0 means that its not yet  assigned
	bool m_show_set_mannual_wc_index_dialog = false;
	bool m_enable_physics = true;
	bool m_show_shift_cobjs_to_another_layer_dialog = false;
	bool m_always_draw_edges = true;//helps with makinjg edges act like lines
	bool m_show_scaling_in_debug_mode = true;

	/*vectors*/
	std::vector<SquareBox::IShape*> m_vec_shapes_pointer;

	std::vector<glm::vec2> m_vector_of_joint_ploting_points;

	std::vector<std::pair<int, int>> m_selected_cluster_objects;

	SquareBox::RenderEngine::ColorRGBA8 m_debug_static_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 0, 0, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_debug_dynamic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 255, 0, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_debug_kinetic_body_color = SquareBox::RenderEngine::ColorRGBA8(0, 0, 255, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_place_mode_sketch_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::antique_white);
	SquareBox::RenderEngine::ColorRGBA8 m_plot_coordinates_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_not_physics_body_color = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255);
	SquareBox::RenderEngine::ColorRGBA8 m_unknown_body_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::violet);
	SquareBox::RenderEngine::ColorRGBA8 m_drag_selection_box_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon);
	SquareBox::RenderEngine::ColorRGBA8 m_border_color;
	SquareBox::RenderEngine::ColorRGBA8 m_selected_body_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);

	/*poiners*/
	SquareBox::IShape* m_current_shape_ptr = nullptr;// a pointer to the current world shape
	SquareBox::GWOM::ClusterObject* m_current_cluster_object_ptr = nullptr;

	//the physics simulation engine
	SquareBox::GWOM::ClusterObject m_temp_bridge_joints_object;

	//Automation
	SquareBox::AI::Automation m_automation;
	SquareBox::AnimationSystem::AnimationCreator m_animation_creator;
	std::string m_animation_script, m_automation_script;
	std::vector<std::pair<int, std::pair<int, int>>> m_cluster_objects_to_delete; //[layer_index[world_cluster_index,cluster_object_index]]

	const char** shapes_labels_ptr = nullptr;
	SquareBox::PhysicsCollisionEngine::PhysicsWorld m_physics_world;
	int m_target_shift_to_layer_index = 0;// when shifting cluster objects from one layer to 
	glm::vec2  m_drag_select_origin;
	bool m_is_drag_selecting = false;
	/* Selection Mode */
	int world_cluster_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
	int cluster_object_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
	int free_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
	int joint_selection_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);



	// free drag selecting box draging input keys combo
	// we need to draw a debug box for this !!
	int free_selection_drag_input_key_1 = static_cast<int>(SquareBox::KeyBoardEnum::SPACE);
	int free_selection_drag_input_key_2 = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);


	//	world cluster selection and free selection dragging activation input key
	int world_cluster_and_free_selection_dragging_activation_input_key = static_cast<int>(SquareBox::KeyBoardEnum::SPACE); // i want to remove this 
	//control in future
	//	world cluster selection and free selection dragging  input key
	int world_cluster_and_free_selection_dragging_input_key = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);

	// world cluster and free selection copying combo
	int left_ctrl_input_key = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL);
	int right_ctrl_input_key = static_cast<int>(SquareBox::KeyBoardEnum::RIGHT_CTRL);
	int c_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEY_c);

	//cluster object drag select mode 
	int cluster_object_drag_selection_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);

	/* Place Mode */
	int non_tiled_layer_placement_input_ley = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);

	int bridge_plotting_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);
	int pulley_plotting_input_key = static_cast<int>(SquareBox::MouseEnum::RIGHT_CLICK);

	glm::vec2 clustermousepos; // used when moving multiple cluster objects at once

	SquareBoxEditor::Editor_Assistant m_editor_assitant;

	bool m_play_simulation = false;//controls physics and animations update loops

	bool m_draw_plotted_shape = false;// determines if its time to draw the chain or polygon, or whose coordinates we have so
	bool m_show_polygon_plots = false;

	bool m_debug_show_joints = false;

	bool m_show_world_locked_dialog = false;

	bool m_respect_ancestor = false;
	bool m_plot_joint = false;
	//clearing selection modes excpet when in cluster object select
	int clear_selection_input_key_1 = static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT);
	int clear_selection_input_key_2 = static_cast<int>(SquareBox::KeyBoardEnum::KEY_c);

	//deletion key
	int delete_selection_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEYBOARD_DELETE);

	// Inherited via LayerTypeInterface
	virtual void onInit() override;
	virtual void onLayerInit(SquareBox::GWOM::Layer& layer_) override;
	virtual int onLayerInitGui(SquareBox::GWOM::Layer& layer_) override;
	virtual void onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_) override;
	virtual void onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_) override;
	virtual void onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;
	virtual void onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;

	virtual void onContentDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_) override;

	virtual void onDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_) override;
	virtual void onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_, EditorModeEnum editor_mode_) override;

	virtual void onLightDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_) override;

	virtual void onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;

	virtual void onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;
	virtual void onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;

	virtual void onDispose() override;
	virtual void onLayerDispose(SquareBox::GWOM::Layer& layer_) override;


};

