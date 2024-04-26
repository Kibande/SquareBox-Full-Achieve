#include "Actor.h"

namespace Platformer {

	Actor::Actor(SquareBox::GWOM::ClusterObject &  cluster_object_, SquareBox::IMainGame* game_ptr_) {
		m_game_ptr = game_ptr_;
		this->collisionDown = false;
		this->collisionLeft = false;
		this->collisionRight = false;
		this->collisionTop = false;
		this->outLeft = false;
		this->outRight = false;
		cluster_object_.init_linear_velocity.x = 0;
		cluster_object_.init_linear_velocity.x = 0;
	}

	Actor::~Actor()
	{}

	void Actor::draw(SquareBox::GWOM::ClusterObject& cluster_object_,SquareBox::RenderEngine::SpriteBatch & sprite_batch_) {

		// m_current_shape_ptr->draw(focus_cluster_object, m_sprite_batch, m_layers[focus_cell_member_coordinates.first].opacity);
	}

	bool Actor::isOverlap(SquareBox::GWOM::ClusterObject& cluster_object_1, SquareBox::GWOM::ClusterObject& cluster_object_2) {
	
		return cluster_object_1.position.x - cluster_object_1.width / 2 < cluster_object_2.position.x + cluster_object_2.width / 2
			&& cluster_object_1.position.x + cluster_object_1.width / 2 > cluster_object_2.position.x - cluster_object_2.width / 2
			&& cluster_object_1.position.y + cluster_object_1.height / 2 > cluster_object_2.position.y - cluster_object_2.height / 2
			&& cluster_object_1.position.y - cluster_object_1.height / 2 < cluster_object_2.position.y + cluster_object_2.height / 2;
	}

	

	bool Actor::containsPoint(SquareBox::GWOM::ClusterObject& cluster_object_,int pointX, int pointY) {
		return pointY >= cluster_object_.position.y - cluster_object_.height / 2 &&
			pointY <= cluster_object_.position.y + cluster_object_.height / 2 &&
			pointX <= cluster_object_.position.x + cluster_object_.width / 2 &&
			pointX >= cluster_object_.position.x - cluster_object_.width / 2;
	}

	bool Actor::gravity() {
		return true;
	}

}
