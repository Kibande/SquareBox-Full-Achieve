#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
class ICollisionAgent
{
public:
	ICollisionAgent();
	virtual~ICollisionAgent();
	virtual void init() = 0;
	virtual void setUp(SquareBox::GWOM::ClusterObject & cluster_object_) = 0;
	virtual void update(float delta_time_, std::pair<int,std::pair<int,int>> & cluster_object_coordinates_, std::vector<SquareBox::GWOM::Layer>& layers_,int grid_index_with_collision_members_, int start_index_, SquareBox::RenderEngine::ParticleBatch * blood_particle_batch_ptr_, SquareBox::GWOM::Grid* collision_grid_ptr_) =0 ;
	//circular collision
	bool collideAgent(const float delta_time_, SquareBox::GWOM::ClusterObject & cluster_object_a_, SquareBox::GWOM::ClusterObject & cluster_object_b_);
	bool collideWithTiles(const float delta_time_, SquareBox::GWOM::ClusterObject& cluster_object_, int target_tiled_layer_, std::vector<SquareBox::GWOM::Layer> & layers_);
	void turnHumanIntoZombie(SquareBox::GWOM::ClusterObject& human_, SquareBox::GWOM::ClusterObject & zombie_, std::vector<SquareBox::GWOM::Layer> & layers_,SquareBox::GWOM::Grid * collision_grid_ptr_);
	virtual void dispose() = 0;
protected:
	SquareBox::Utilities m_utilities;
private:
	void collideWithTile(const float delta_time_, SquareBox::GWOM::ClusterObject& cluster_object_,float tile_width_, SquareBox::GWOM::Tile* tile_);
};

