#pragma once
#include "IExample.h"
#include <cstdio>
#include <Box2D/include/box2d/box2d.h>
namespace SquareBox {
	namespace Example {
		class Box2DExample :public SquareBox::Example::IExample
		{
		public:
			// Inherited via IExample
			virtual void init() override;
			virtual void onUpdate() override;
			virtual void dispose() override;
		private:
			b2World * m_world = nullptr;
			b2Vec2 m_gravity = b2Vec2(0.0f, -10.0f);
			float m_timeStep = 1.0f / 60.0f;
			int32 m_velocityIterations = 6; int32 positionIterations = 2;
		};
	}
}
