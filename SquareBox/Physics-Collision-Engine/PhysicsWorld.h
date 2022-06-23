#pragma once
#include <Box2D/include/box2d/box2d.h>
#include <GWOM/GWOM.h>
namespace SquareBox {
	namespace PhysicsCollisionEngine
	{
		class PhysicsWorld
		{
			//this is instance of a single physics world,
			// the physics world is where all our physics simulations take place from
			// so the idea is that we have both a physics world and a graphics world
			// if both the  graphics and physics worlds are present then the graphics world
			//simply shows whats happening in the physics world , the physics world only manages
			// physics objects , objects with no physics are handled manually by the user of this
			// Engine
		public:
			PhysicsWorld();
			~PhysicsWorld();
			void init(); //intialize our world
			void destroy(); //destory our world
			void update(); //update our worlds' physics
			void shiftWorldOrigin(glm::vec2& newWorldOrigin_);// this is done when our world is bigger than 2 kilometers
			void setTimeStep(float _timeStep) { m_timeStep = _timeStep; };
			float getTimeStep()const { return  m_timeStep; };
			int getVelocityIterations()const { return  m_velocityIterations; };
			int getPositionIterations()const { return  m_positionIterations; };
			bool isLocked() const { return m_world->IsLocked(); }
			void setVelocityIterations(int _velocityIterations) {
				m_velocityIterations = _velocityIterations;
			}
			void setPositionIterations(int _positionIterations) {
				m_positionIterations = _positionIterations;
			}

			bool isWorldinitalised() const { return m_isWorldinitalised; }
			void createObject(const SquareBox::GWOM::ClusterObject &clusterObject_);
			void createJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);
			void destoryJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);
			bool areObjectsTouching(const SquareBox::GWOM::ClusterObject &sensorBody_, const SquareBox::GWOM::ClusterObject &bodyToTest_);

			//Joint Setters
			void enableJointLimits(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			void setJointLimits(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const float upper_, const float lower_, const SquareBox::JointTypeEnum jointType_);
			void setJointMaxMotorSpeed(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const float speed_, const SquareBox::JointTypeEnum jointType_);
			void setJointMaxMotorForce(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const float force_, const SquareBox::JointTypeEnum jointType_);
			void setJointMaxMotorTorque(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const float torque_, const SquareBox::JointTypeEnum jointType_);

			//Joint Getters
			glm::vec2 getJointsReactionForce(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_);
			float getJointsReactionTorque(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_);
			glm::vec2 getJointsAnchorPoint(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, bool getForBodyA = true);
			bool isJointLimitEnabled(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			bool isMotorLimitEnabled(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointUpperLimit(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointLowerLimit(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointMotorSpeed(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointMotorForce(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointMaxMotorForce(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointMotorTorque(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointMaxMotorTorque(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointLengthA(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);
			float getJointLengthB(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_, const SquareBox::JointTypeEnum jointType_);

		private:
			bool m_isWorldinitalised = false;
			b2World* m_world = nullptr;
			b2Vec2 m_gravity = b2Vec2(0.0f, -10.0f);
			float m_timeStep = 1.0f / 60.0f;
			int m_velocityIterations = 8;
			int m_positionIterations = 3;

			//Creating a physics object
			void createBody(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_);

			//The Shape we currently support
			void createBoxShape(const SquareBox::GWOM::ClusterObject &clusterObject_);
			void createCircleShape(const SquareBox::GWOM::ClusterObject &clusterObject_);
			void createCapsule(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_);
			void createTopCapsule(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_);
			void createCar(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_);
			void createBicycle(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_);
			void createPolygonShape(const SquareBox::GWOM::ClusterObject &clusterObject_);
			void createChainShape(const SquareBox::GWOM::ClusterObject &clusterObject_);
			void createEdgeShape(const SquareBox::GWOM::ClusterObject &clusterObject_);

			//Create Revolute Joint
			b2Joint* createRevoluteJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);
			b2Joint* createDistanceJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);
			b2Joint* createPrismaticJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);
			b2Joint* createPulleyJoint(SquareBox::GWOM::ClusterObject &clusterObjectA_, SquareBox::GWOM::ClusterObject &clusterObjectB_, SquareBox::GWOM::Joint& joint_);

			//The Fixture
			void createFixtureForBody(const SquareBox::GWOM::ClusterObject &clusterObject_);

			//joints
			b2Joint* getSharedJoint(const SquareBox::GWOM::ClusterObject &clusterObjectA_, const SquareBox::GWOM::ClusterObject &clusterObjectB_);
		};
	}
}
