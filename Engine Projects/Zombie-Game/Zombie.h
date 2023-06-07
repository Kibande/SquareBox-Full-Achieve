#pragma once
#include "ICollisionAgent.h"
#include "LayerIndicies.h"
#include <random>
#include <ctime>
class Zombie :public ICollisionAgent
{
public:
	// Inherited via ICollisionAgent
	virtual void init() override;
	virtual void setUp(SquareBox::GWOM::ClusterObject & cluster_object_) override;
	virtual void update(float delta_time_, std::pair<int, std::pair<int, int>>  & cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_,int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_) override;
	virtual void dispose() override;
private:
	bool getClosetHuman(SquareBox::GWOM::ClusterObject & zombie_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::Grid* collision_grid_ptr_);
	bool getClosetHumanInCell(SquareBox::GWOM::ClusterObject& zombie_, std::vector<SquareBox::GWOM::Layer>& layers_,SquareBox::GWOM::Cell* target_cell_, float & current_closet_distance_);
	
	//float m_speed = 0.001f;
	float m_speed = 0.18f;
	const float rotation_angle_range = 40.0f;
	const float direction_range = 1.0f;
};

