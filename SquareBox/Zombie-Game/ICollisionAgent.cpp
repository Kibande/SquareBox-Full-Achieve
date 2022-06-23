#include "ICollisionAgent.h"
ICollisionAgent::ICollisionAgent()
{
	m_utilities.init();
}

ICollisionAgent::~ICollisionAgent()
{
	m_utilities.dispose();
}

bool ICollisionAgent::collideAgent(const float delta_time_, SquareBox::GWOM::ClusterObject& cluster_object_a_, SquareBox::GWOM::ClusterObject& cluster_object_b_)
{
    // make sure these are valide colliisions

    if (!cluster_object_a_.is_alive || !cluster_object_b_.is_alive) {
        return false;
    }

    // do not collide the same object

    if (cluster_object_a_.layer_index == cluster_object_b_.layer_index && cluster_object_a_.cluster_index == cluster_object_b_.cluster_index && cluster_object_a_.index == cluster_object_b_.index) {
        return false;
    }

    // Minimum distance before there is a collision
    const float MIN_DISTANCE = cluster_object_a_.radius + cluster_object_b_.radius;

    // Center position of this agent
    glm::vec2 centerPosA = cluster_object_a_.position;
        // Center position of the parameter agent
    glm::vec2 centerPosB = cluster_object_b_.position;

    // Distance vector between the two agents
    glm::vec2 distVec = centerPosA - centerPosB;

    // Length of the distance vector
    float distance = glm::length(distVec);

    // Depth of the collision
    float collisionDepth = MIN_DISTANCE - distance;

    // If collision depth > 0 then we did collide
    if (collisionDepth > 0) {

        // Get the direction times the collision depth so we can push them away from each other
        glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

        // Push them in opposite directions
        cluster_object_a_.position += collisionDepthVec / 2.0f;
        cluster_object_b_.position -= collisionDepthVec / 2.0f;
        return true;
    }
    return false;
}

bool ICollisionAgent::collideWithTiles(const float delta_time_, SquareBox::GWOM::ClusterObject& cluster_object_, int target_tiled_layer_, std::vector<SquareBox::GWOM::Layer>& layers_)
{
    std::vector<glm::vec2> collideTilePositions;

    // Check the four corners
	glm::vec2 cluster_object_dimensions(cluster_object_.radius*2.0f);
	glm::vec4 new_cluster_object_dest_rect = glm::vec4(cluster_object_.position - cluster_object_dimensions * 0.5f, cluster_object_dimensions);
    collided_tiles.clear();

	glm::vec2 bottom_left_corner = glm::vec2(new_cluster_object_dest_rect.x, new_cluster_object_dest_rect.y);
	glm::vec2 bottom_right_corner = glm::vec2(new_cluster_object_dest_rect.x + new_cluster_object_dest_rect.z, new_cluster_object_dest_rect.y);
	glm::vec2 top_right_corner = glm::vec2(new_cluster_object_dest_rect.x + new_cluster_object_dest_rect.z, new_cluster_object_dest_rect.y + new_cluster_object_dest_rect.w);
	glm::vec2 top_left_corner = glm::vec2(new_cluster_object_dest_rect.x, new_cluster_object_dest_rect.y + new_cluster_object_dest_rect.w);

	// check if this will result into a collision
	auto destination_tile = layers_[target_tiled_layer_].tile_system.getTile(bottom_left_corner);
	if (destination_tile->key != -1) {
		collided_tiles.push_back(destination_tile);
	}

	destination_tile = layers_[target_tiled_layer_].tile_system.getTile(bottom_right_corner);
	if (destination_tile->key != -1) {
		collided_tiles.push_back(destination_tile);
	}

	destination_tile = layers_[target_tiled_layer_].tile_system.getTile(top_right_corner);
	if (destination_tile->key != -1) {
		collided_tiles.push_back(destination_tile);
	}

	destination_tile = layers_[target_tiled_layer_].tile_system.getTile(top_left_corner);
	if (destination_tile->key != -1) {
		collided_tiles.push_back(destination_tile);
	}

	if (collided_tiles.size() == 0)
		return false;

    // Do the collision
    for (int i = 0; i < collided_tiles.size(); i++) {
        collideWithTile(delta_time_, cluster_object_, layers_[target_tiled_layer_].tile_system.getTileSize(),collided_tiles[i]);
    }
    return true;
}

void ICollisionAgent::collideWithTile(const float delta_time_, SquareBox::GWOM::ClusterObject& cluster_object_, float tile_width_, SquareBox::GWOM::Tile* tile_)
{
    /*
        the value of delta time is not being considered because we want collisions to be immdieatly resolved
        considering delta timw during collisions just gave us problems
    
    */
    const float TILE_RADIUS = (float)tile_width_ / 2.0f;
    // The minimum distance before a collision occurs
    const float MIN_DISTANCE = cluster_object_.radius + TILE_RADIUS;

    // Center position of the agent
    glm::vec2 centerAgentPos = cluster_object_.position;
    // Vector from the agent to the tile
    glm::vec2 distVec = centerAgentPos - tile_->position;

    // Get the depth of the collision
    float xDepth = MIN_DISTANCE - abs(distVec.x);
    float yDepth = MIN_DISTANCE - abs(distVec.y);

    // If both the depths are > 0, then we collided
    if (xDepth > 0 && yDepth > 0) {

        // Check which collision depth is less
        if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f)) {
            // X collsion depth is smaller so we push in X direction
            if (distVec.x < 0) {
                cluster_object_.position.x -= xDepth;
            }
            else {
                cluster_object_.position.x += xDepth;
            }
        }
        else {
            // Y collsion depth is smaller so we push in X direction
            if (distVec.y < 0) {
                cluster_object_.position.y -= yDepth;
            }
            else {
                cluster_object_.position.y += yDepth;
            }
        }
    }
}

void ICollisionAgent::turnHumanIntoZombie(SquareBox::GWOM::ClusterObject & human_, SquareBox::GWOM::ClusterObject & zombie_, std::vector<SquareBox::GWOM::Layer> & layers_, SquareBox::GWOM::Grid * collision_grid_ptr_)
{
	//kill cluster object and  eliminate from the system
	human_.is_alive = false;
	collision_grid_ptr_->removeObject(human_);
	m_utilities.removePairFromVector(layers_[human_.layer_index].alive_cluster_objects, std::pair<int, int>(human_.cluster_index, human_.index));

	int source_zombie_layer_index = zombie_.layer_index;
	//create zombie
	// all the previous zombies attributes
	layers_[source_zombie_layer_index].world_clusters.back().cluster_objects.push_back(zombie_);
	SquareBox::GWOM::ClusterObject & new_zombie = layers_[source_zombie_layer_index].world_clusters.back().cluster_objects.back();
	//restore unique attributes like 
	new_zombie.position = human_.position;
	new_zombie.life_span = 10;
	new_zombie.cluster_index = layers_[source_zombie_layer_index].world_clusters.back().index;
	new_zombie.index = layers_[source_zombie_layer_index].world_clusters.back().cluster_objects.size() - 1;
	//register with the system
	m_utilities.addPairToVector(layers_[source_zombie_layer_index].alive_cluster_objects, std::pair<int, int>(new_zombie.cluster_index, new_zombie.index));
	collision_grid_ptr_->addObject(new_zombie);
}

