#pragma once
#include <SquareBox-Core/Essentials.h>
#include<SquareBox-Logger/Logger.h>
#include<vector>
#include <Box2D/include/box2d/box2d.h>
#include<glm/glm.hpp>

namespace SquareBox
{
	namespace PhysicsCollisionEngine
	{
		class PhysicsWorld;
	};
};

namespace SquareBox {
	namespace PhysicsCollisionEngine
	{
		class PhysicsProperties
		{
		public:
			friend class PhysicsWorld;
			PhysicsProperties();
			~PhysicsProperties();
			void init(int numFixtures_);
			glm::vec4 getBoxCoordinates();
			glm::vec2 getCircleCoordinates();
			float getAngle();
			std::vector<glm::vec2> getPolygonVerticesCoordinates();
			std::vector<glm::vec2> getEdgeVerticesCoordinates();
			std::vector<glm::vec2> getChainVerticesCoordinates();
			glm::vec4 getCapsuleCoordinates(bool fullCapsule = true, bool topCapsule = true);
			glm::vec2 getLinearVelocity();
			bool isIntialised() { return m_isIntialised; }
			glm::vec2 getXYPosition() {
				return glm::vec2(body->GetPosition().x, body->GetPosition().y);
			}
			float getRotation() {
				return body->GetAngle();
			}
			void applyForceToCenter(glm::vec2 force, bool wake);
			void applyForce(glm::vec2 force, glm::vec2 position, bool wake);
			void applyTorque(float torque_, bool wake_);
			void applyLinerImpluse(const glm::vec2& impluse_, const glm::vec2 position_, bool wake_);
			void setLinerVelocity(glm::vec2 & linerVelocity_);
			void applyAngularImpluse(float impulse_, bool wake_);
			void setBodyMass(float mass_);
			glm::vec2 getLateralVelocity();
			glm::vec2 getForwardVelocity();
			float getSpeedInForwardDirection();
			void dismissLateralVelocity();
			void dismissAngularVelocity();
			//returns true if the point is inside our body or fixture
			//x<0 gives you the last elements results else x>=0 gives us the first
			bool testPoint(float x, float y, int index) const;
			float getMass()const { return body->GetMass(); }
			float getInertia() const { return body->GetInertia(); }
			bool isDynamic() const { return body->GetType() == b2_dynamicBody; }
			bool isStatic() const { return body->GetType() == b2_staticBody; }
			bool isKinematic() const { return body->GetType() == b2_kinematicBody; }
			bool isOnGround();
			void dispose();
			// a body can have  multiple fixtures and multiple joints
			std::vector<b2Fixture*> fixtures;
			glm::vec2 dimensions; //width,height
		private:
			b2World* world = nullptr;
			b2Body* body = nullptr;
			b2Shape* shape = nullptr;
			bool m_isIntialised = false;
			float m_z;//width for the box
			float m_w;//height
			//for the capsule
		};
	};
}