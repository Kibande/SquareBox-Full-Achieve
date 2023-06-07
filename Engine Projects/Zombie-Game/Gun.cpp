#include "Gun.h"

void Gun::init(std::string name_, int fire_rate_, int bullets_per_shoot_, float spread_, SquareBox::AudioSystem::SoundBank sound_bank_,Bullet* bullet_)
{
	m_name = name_;
    m_fire_rate = fire_rate_;
    m_bullet_ptr = bullet_;
	m_bullets_per_shoot = bullets_per_shoot_;
	m_spread = spread_;
	m_sound_bank = sound_bank_;
    m_utilities.init();
}

void Gun::update(float delta_time_, bool should_fire_, const glm::vec2& position_, const glm::vec2& direction_, std::vector<SquareBox::GWOM::Layer> & layers_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
    m_frame_counter += 1.0f * delta_time_;
 
    // After a certain number of frames has passed we fire our gun
    if (m_frame_counter >= m_fire_rate && should_fire_) {
        fire(position_, direction_, layers_,collision_grid_ptr_);
        m_frame_counter = 0;
    }
}

void Gun::fire(const glm::vec2& position_, const glm::vec2& direction_, std::vector<SquareBox::GWOM::Layer> & layers_, SquareBox::GWOM::Grid* collision_grid_ptr_)
{
    static std::mt19937 randomEngine(time(nullptr));
    // For offsetting the accuracy
    std::uniform_real_distribution<float> randRotate(-m_spread, m_spread);

   // m_sound_bank.play();

    for (int i = 0; i < m_bullets_per_shoot; i++) {
        
        //create a new bullet
        // Add a new bullet
        layers_[LayerIndicies::ammunition_layer_index].world_clusters.back().cluster_objects.emplace_back();
      
        auto& new_bullet = layers_[LayerIndicies::ammunition_layer_index].world_clusters.back().cluster_objects.back();
        new_bullet.layer_index = LayerIndicies::ammunition_layer_index;
        new_bullet.cluster_index = layers_[LayerIndicies::ammunition_layer_index].world_clusters.back().index;
        new_bullet.index = layers_[LayerIndicies::ammunition_layer_index].world_clusters.back().cluster_objects.size()-1;
        
        new_bullet.is_alive = true;

        new_bullet.position = position_;
        new_bullet.direction = glm::rotate(direction_, randRotate(randomEngine));
        m_bullet_ptr->setUp(new_bullet);

        //add to the grid
        collision_grid_ptr_->addObject(new_bullet);
        m_utilities.addPairToVector(layers_[new_bullet.layer_index].alive_cluster_objects, std::pair<int, int>(new_bullet.cluster_index, new_bullet.index));
    }
}

void Gun::dispose()
{
    m_utilities.dispose();
}
