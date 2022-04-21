#include "Box2DExample.h"

namespace SquareBox {
	namespace Example {
		void Box2DExample::init()
		{
			/*
			Hello Box2D
			In the distribution of Box2D is a Hello World project.
			The program creates a large ground box and a small dynamic box.
			This code does not contain any graphics.
			All you will see is text output in the console of the box's position over time.
		*/
		//create World
			m_world = new b2World(m_gravity);
		}

		void Box2DExample::onUpdate()
		{
			//create Ground
			b2BodyDef groundBodyDef;
			groundBodyDef.position.Set(0.0f, -10.0f);
			b2Body* groundBody = m_world->CreateBody(&groundBodyDef);

			b2PolygonShape groundBox;
			groundBox.SetAsBox(50.0f, 10.0f);
			groundBody->CreateFixture(&groundBox, 0.0f);

			//Create the Dynamic Box
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(0.0f, 4.0f);
			b2Body* body = m_world->CreateBody(&bodyDef);
			b2FixtureDef fixtureDef;
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(1.0f, 1.0f);
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			body->CreateFixture(&fixtureDef);

			//updating our Simulation
			for (int32 i = 0; i < 60; ++i) {
				m_world->Step(m_timeStep, m_velocityIterations, positionIterations);
				b2Vec2 position = body->GetPosition();
				float angle = body->GetAngle();
				printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
			}
		}

		void Box2DExample::dispose()
		{
			delete m_world;
		}
	}
}