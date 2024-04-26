#pragma once

#include <SquareBox-Core/SquareBoxGameEngine.h>

namespace Platformer {

	class Actor
	{
	public:
		Actor(SquareBox::GWOM::ClusterObject & cluster_object_, SquareBox::IMainGame* game_ptr_);
		~Actor();
		virtual void draw(SquareBox::GWOM::ClusterObject& cluster_object_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_);
		virtual bool isOverlap(SquareBox::GWOM::ClusterObject& cluster_object_1, SquareBox::GWOM::ClusterObject& cluster_object_2);
		bool containsPoint(SquareBox::GWOM::ClusterObject& cluster_object_, int pointX, int pointY);
		virtual bool gravity();
		bool collisionTop;
		bool collisionDown;
		bool collisionLeft;
		bool collisionRight;
		bool outRight;
		bool outLeft;
		SquareBox::IMainGame* m_game_ptr = nullptr;
	};

}

