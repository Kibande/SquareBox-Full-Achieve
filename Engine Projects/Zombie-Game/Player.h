#pragma once
#include "ICollisionAgent.h"
#include "LayerIndicies.h"
class Player : public ICollisionAgent
{
public:
	// Inherited via ICollisionAgent
	virtual void init() override;
	virtual void setUp(SquareBox::GWOM::ClusterObject & cluster_object_) override;
	virtual void update(float delta_time_, std::pair<int, std::pair<int, int>>  &  cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_,int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_) override;
	virtual void dispose() override;
private:
	//The players speed will be controlled from here
	float m_speed = 1.5f;
};

