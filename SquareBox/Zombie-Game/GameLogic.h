#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "ICollisionAgent.h"
#include "Player.h"
#include "Human.h"
#include "Zombie.h"
#include "Bullet.h"
#include "Gun.h"
#include "LayerIndicies.h"

class GameLogic
{
public:

	void init(SquareBox::IMainGame* game_ptr_,SquareBox::GWOM::Grid* collision_grid_ptr_, std::vector<SquareBox::GWOM::Layer>& layers_,SquareBox::Camera::ParallelCamera & game_play_camera_,std::pair<int,std::pair<int,int>> & player_coordinates_);
	void update(float delta_time_, bool fire_,std::vector<SquareBox::GWOM::Layer>& layers_);
	void dispose();
	void restart(std::vector<SquareBox::GWOM::Layer>& layers_, std::pair<int, std::pair<int, int>>&player_coordinates_);
private:
	void disposeLayers(std::vector<SquareBox::GWOM::Layer>& layers_);
	void initLayers(std::vector<SquareBox::GWOM::Layer>& layers_,std::pair<int, std::pair<int, int>>& player_coordinates_);
	//do not pass the cluster_object_full_coordinates_ by refrenece but by value
	void checkCollisionCellCollision(float delta_time_, std::vector<SquareBox::GWOM::Layer>& layers_,std::pair<int,std::pair<int,int>> cluster_object_full_coordinates_,int grid_index_with_collision_members_,int start_index_);
	void loadLevel(const char * file_path_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::Camera::ParallelCamera & game_play_camera_, std::pair<int, std::pair<int, int>> & player_coordinates_, bool first_empty_world_ = false);
	//we need references to the audio, particle engine and camera
	SquareBox::IMainGame* m_game_ptr = nullptr;
	//We need a currentShape_ptr
	SquareBox::IShape * m_current_shape_ptr = nullptr;

	//We shall be haveing some particle e,g blood
	SquareBox::RenderEngine::ParticleBatch * m_blood_particle_batch_ptr;

	//A default helper functions
	SquareBox::Utilities m_utilities;


	//A grid for spartial partioning
	SquareBox::GWOM::Grid * m_collision_grid_ptr = nullptr;
	float m_camera_scale = 1.0f;
	glm::vec2 m_camera_postion = glm::vec2(0.0f);
	//player Coordinates
	std::pair<int, std::pair<int,int>> m_player_coordinates;
	

	int m_frameCounter = 0;//aids in controlling the rate at which bullets are fired

	const int AMO_VECTOR_DEFAULT_SIZE = 400;

	Human m_human;
	Player m_player;
	Zombie m_zombie;
	Bullet m_bullet;
	Gun m_gun;
  /* layer indicies*/
	ICollisionAgent * m_collision_agent_ptr = nullptr;
	std::vector<SquareBox::GWOM::Layer> m_layers_backup;
};
