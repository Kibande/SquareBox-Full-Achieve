#include "PhysicsWorld.h"
namespace SquareBox {
	namespace PhysicsCollisionEngine
	{
		PhysicsWorld::PhysicsWorld()
		{
		}
		PhysicsWorld::~PhysicsWorld()
		{
		}
		void PhysicsWorld::init()
		{
			if (m_isWorldinitalised == false)
			{
				m_world = new b2World(m_gravity);
				m_isWorldinitalised = true;
				SBX_CORE_INFO("Physics World Intiated");
			}
			else {
				SBX_CORE_INFO("Physics World Already Intiated");
			}
		}

		void PhysicsWorld::destroy()
		{
			delete m_world;
			m_world = nullptr;
		}

		void PhysicsWorld::update()
		{
			m_world->Step(m_timeStep, m_velocityIterations, m_positionIterations);
		}

		void PhysicsWorld::shiftWorldOrigin(glm::vec2& newWorldOrigin_)
		{
			m_world->ShiftOrigin(b2Vec2(newWorldOrigin_.x, newWorldOrigin_.y));
		}

		void PhysicsWorld::createObject(const SquareBox::GWOM::ClusterObject & clusterObject_)
		{
			//share our world pointer
			clusterObject_.physics_properties->world = m_world;
			b2BodyType bT = b2_dynamicBody;
			switch (clusterObject_.type)
			{
			case SquareBox::BodyTypeEnum::StaticBody:
				bT = b2_staticBody;
				break;
			case SquareBox::BodyTypeEnum::KinematicBody:
				bT = b2_kinematicBody;
				break;
			case SquareBox::BodyTypeEnum::DynamicBody:
				bT = b2_dynamicBody;
				break;
			default:
				SBX_CRITICAL("UnIndentified Body Type");
				break;
			}

			switch (clusterObject_.shape)
			{
			case SquareBox::BodyShapeEnum::Circle:
				clusterObject_.physics_properties->init(1, 0);//1 fixture and 0 joints
				createBody(clusterObject_, bT);
				createCircleShape(clusterObject_);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::Edge:
				clusterObject_.physics_properties->init(0, 0);//0 fixture and 0 joints
				//the fixtures will be created later and just pusgh back
				createBody(clusterObject_, bT);
				createEdgeShape(clusterObject_);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::Polygon:
				clusterObject_.physics_properties->init(1, 0);//1 fixture and 0 joint
				createBody(clusterObject_, bT);
				createPolygonShape(clusterObject_);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::Box:
				clusterObject_.physics_properties->init(1, 0);//1 fixture and 0 joints
				createBody(clusterObject_, bT);
				createBoxShape(clusterObject_);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::Chain:
				clusterObject_.physics_properties->init(1, 0);//1 fixture and 0 joints
				createBody(clusterObject_, bT);
				createChainShape(clusterObject_);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::Capsule:
				clusterObject_.physics_properties->init(3, 0);//3 fixture and 0 joints
				//this is a multi fixture body , so it will create its own fixtures
				//hence Do its own data Data Reset
				createCapsule(clusterObject_, bT);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::TopCapsule:
				clusterObject_.physics_properties->init(2, 0);//2 fixture and 0 joints
				createTopCapsule(clusterObject_, bT);
				clusterObject_.physics_properties->m_isIntialised = true;
				break;
			case SquareBox::BodyShapeEnum::TypeCount:
				break;
			default:
				SBX_CORE_CRITICAL("UnIndentified Body Shape");
				break;
			}
		}

		void PhysicsWorld::createJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_, SquareBox::GWOM::Joint & joint_)
		{
			//make the joint alive and add it to clusterObjectAs joints list
			if (joint_.joint_type == SquareBox::JointTypeEnum::distance_joint) {
				createDistanceJoint(clusterObjectA_, clusterObjectB_, joint_);
			}
			else if (joint_.joint_type == SquareBox::JointTypeEnum::revolute_joint) {
				createRevoluteJoint(clusterObjectA_, clusterObjectB_, joint_);
			}
			else if (joint_.joint_type == SquareBox::JointTypeEnum::prismatic_joint) {
				createPrismaticJoint(clusterObjectA_, clusterObjectB_, joint_);
			}
			else if (joint_.joint_type == SquareBox::JointTypeEnum::pulley_joint) {
				createPulleyJoint(clusterObjectA_, clusterObjectB_, joint_);
			}
		}

		bool PhysicsWorld::areObjectsTouching(const SquareBox::GWOM::ClusterObject & sensorBody_, const SquareBox::GWOM::ClusterObject & bodyToTest_)
		{
			b2Vec2 sensorPosition = sensorBody_.physics_properties->body->GetPosition();
			b2Vec2 localPosition = bodyToTest_.physics_properties->body->GetLocalPoint(sensorPosition);

			for (b2Fixture *fixture = bodyToTest_.physics_properties->body->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
			{
				if (fixture->TestPoint(localPosition)) {
					return true;
				}
			}
			return false;
		}

		void PhysicsWorld::setJointMaxMotorSpeed(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const float speed_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetMotorSpeed(speed_);
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetMotorSpeed(speed_);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Motors");
			}
		}

		void PhysicsWorld::setJointMaxMotorForce(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const float force_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetMaxMotorForce(force_);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Motor Force");
			}
		}

		void PhysicsWorld::setJointMaxMotorTorque(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const float torque_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetMaxMotorTorque(torque_);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Motor Torque");
			}
		}

		void PhysicsWorld::enableJointLimits(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->EnableLimit(true);
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->EnableLimit(true);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Limits");
			}
		}

		void PhysicsWorld::setJointLimits(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const float upper_, const float lower_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetLimits(lower_, upper_);
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					joint_ptr->SetLimits(lower_, upper_);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Limits");
			}
		}

		void  PhysicsWorld::createBody(const SquareBox::GWOM::ClusterObject &clusterObject_, const b2BodyType bodyType_)
		{
			b2BodyDef bodyDef;
			bodyDef.type = bodyType_;
			bodyDef.position.Set(clusterObject_.position.x, clusterObject_.position.y);
			bodyDef.fixedRotation = clusterObject_.is_fixed_rotation;
			bodyDef.angle = clusterObject_.angle;
			bodyDef.allowSleep = clusterObject_.allow_sleep;
			bodyDef.linearDamping = clusterObject_.linear_damping;
			bodyDef.angularDamping = clusterObject_.angular_damping;
			bodyDef.gravityScale = clusterObject_.gravity_scale;
			bodyDef.enabled = true;
			bodyDef.awake = clusterObject_.awake;

			clusterObject_.physics_properties->body = m_world->CreateBody(&bodyDef);
			clusterObject_.physics_properties->body->SetAngularVelocity(clusterObject_.init_angular_velocity);
			clusterObject_.physics_properties->body->SetLinearVelocity(b2Vec2(clusterObject_.init_linear_velocity.x, clusterObject_.init_linear_velocity.y));
		}
		void PhysicsWorld::createBoxShape(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			clusterObject_.physics_properties->dimensions = glm::vec2(clusterObject_.width, clusterObject_.height);
			b2PolygonShape boxShape;
			boxShape.SetAsBox(clusterObject_.width / 2.0f, clusterObject_.height / 2.0f);
			clusterObject_.physics_properties->shape = &boxShape;
			createFixtureForBody(clusterObject_);
		}
		void  PhysicsWorld::createCircleShape(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			b2CircleShape circleShape;
			circleShape.m_radius = clusterObject_.radius;
			circleShape.m_p.Set(clusterObject_.off_set.x, clusterObject_.off_set.y);
			clusterObject_.physics_properties->shape = &circleShape;
			createFixtureForBody(clusterObject_);
		}
		void PhysicsWorld::createCapsule(const SquareBox::GWOM::ClusterObject & clusterObject_, const b2BodyType bodyType_)
		{
			const glm::vec2 & position = glm::vec2(clusterObject_.position.x, clusterObject_.position.y);
			const glm::vec2 & dimensions = glm::vec2(clusterObject_.width, clusterObject_.height);
			//this is a regular capsule
			clusterObject_.physics_properties->fixtures.resize(0);
			clusterObject_.physics_properties->fixtures.reserve(3);
			clusterObject_.physics_properties->fixtures = std::vector<b2Fixture*>(3);

			clusterObject_.physics_properties->dimensions = dimensions;

			//--------------------
			//   Body Definition
			//-------------------------
			b2BodyDef bodyDef;
			bodyDef.type = bodyType_;
			bodyDef.position.Set(position.x, position.y);
			bodyDef.fixedRotation = clusterObject_.is_fixed_rotation;
			bodyDef.angle = clusterObject_.angle;
			clusterObject_.physics_properties->body = m_world->CreateBody(&bodyDef);

			//Create the shape
			b2PolygonShape boxShape;

			//shrinck the box appropriatly, these measurements will be doubled behind the scenes
			boxShape.SetAsBox(dimensions.x / 2.0f, (dimensions.y - dimensions.x) / 2.0f);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = clusterObject_.density;
			fixtureDef.friction = clusterObject_.friction;
			clusterObject_.physics_properties->fixtures[0] = clusterObject_.physics_properties->body->CreateFixture(&fixtureDef);

			//--------------------
			//     The Circles
			//--------------------

			//the Shape
			b2CircleShape circleShape;
			circleShape.m_radius = dimensions.x / 2.0f;

			//Definition
			b2FixtureDef circleDef;
			circleDef.shape = &circleShape;
			circleDef.density = clusterObject_.density;
			circleDef.friction = clusterObject_.friction;
			circleDef.restitution = clusterObject_.restitution;

			//Bottom Circle
			circleShape.m_p.Set(0.0f, (-dimensions.y + dimensions.x) / 2.0f);
			clusterObject_.physics_properties->fixtures[1] = clusterObject_.physics_properties->body->CreateFixture(&circleDef);

			//Top Circle
			circleShape.m_p.Set(0.0f, (dimensions.y - dimensions.x) / 2.0f);
			clusterObject_.physics_properties->fixtures[2] = clusterObject_.physics_properties->body->CreateFixture(&circleDef);
			clusterObject_.physics_properties->body->ResetMassData();
		}
		void PhysicsWorld::createTopCapsule(const SquareBox::GWOM::ClusterObject & clusterObject_, const b2BodyType bodyType_)
		{
			const glm::vec2 & position = glm::vec2(clusterObject_.position.x, clusterObject_.position.y);
			const glm::vec2 & dimensions = glm::vec2(clusterObject_.width, clusterObject_.height);
			//this is a regular capsule

			clusterObject_.physics_properties->dimensions = dimensions;

			//Body Defination
			b2BodyDef bodyDef;
			bodyDef.type = bodyType_;
			bodyDef.position.Set(position.x, position.y);
			bodyDef.fixedRotation = clusterObject_.is_fixed_rotation;
			bodyDef.angle = clusterObject_.angle;
			clusterObject_.physics_properties->body = m_world->CreateBody(&bodyDef);

			//--------------------
			//   The main Rectangle
			//-------------------------
			//Create the shape
			b2PolygonShape boxShape;

			//shrinck the box appropriatly, these measurements will be doubled behind the scenes
			boxShape.SetAsBox(dimensions.x / 2.0f, (dimensions.y - dimensions.x*0.5f) / 2.0f);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = clusterObject_.density;
			fixtureDef.friction = clusterObject_.friction;
			clusterObject_.physics_properties->fixtures[0] = clusterObject_.physics_properties->body->CreateFixture(&fixtureDef);

			//--------------------
			//     The Circles
			//--------------------

			//the Shape
			b2CircleShape circleShape;
			circleShape.m_radius = dimensions.x / 2.0f;

			//Definition
			b2FixtureDef circleDef;
			circleDef.shape = &circleShape;
			circleDef.density = clusterObject_.density;
			circleDef.friction = clusterObject_.friction;
			circleDef.restitution = clusterObject_.restitution;

			//Top Circle
			circleShape.m_p.Set(0.0f, (dimensions.y - dimensions.x) / 2.0f);
			clusterObject_.physics_properties->fixtures[1] = clusterObject_.physics_properties->body->CreateFixture(&circleDef);
			clusterObject_.physics_properties->body->ResetMassData();
		}
		void PhysicsWorld::createCar(const SquareBox::GWOM::ClusterObject & clusterObject_, const b2BodyType bodyType_)
		{
		}
		void PhysicsWorld::createBicycle(const SquareBox::GWOM::ClusterObject & clusterObject_, const b2BodyType bodyType_)
		{
		}
		void PhysicsWorld::createPolygonShape(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			b2PolygonShape polygonShape;

			b2Vec2 vec2Vector[b2_maxPolygonVertices];
			//vec2Vector.reserve(vertices.size());//Resize so that all our pointers point to the right place
			for (unsigned i = 0; i < clusterObject_.vertices.size() && i < b2_maxPolygonVertices; i++)
			{
				vec2Vector[i].Set(clusterObject_.vertices[i].x, clusterObject_.vertices[i].y);
				//vec2Vector.push_back(b2Vec2(vertices[i].x, vertices[i].y));
			}
			int32 count = clusterObject_.vertices.size() <= b2_maxPolygonVertices ? clusterObject_.vertices.size() : b2_maxPolygonVertices;

			polygonShape.Set(vec2Vector, count);

			clusterObject_.physics_properties->shape = &polygonShape;
			createFixtureForBody(clusterObject_);
		}
		void PhysicsWorld::createChainShape(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			//createChainShape(vertices); //the chain refused, i dont knw why
			//when i would pass it throu this function is world throw an error instead
			 //its like the createFixtureForBody function demands to be in the same place as the shape is created
			//so i did that for aso the rest just to have a more uniform a proche
			std::vector<b2Vec2> vec2Vector;
			vec2Vector.reserve(clusterObject_.vertices.size());//Resize so that all our pointers point to the right place
			for (unsigned i = 0; i < clusterObject_.vertices.size(); i++)
			{
				vec2Vector.push_back(b2Vec2(clusterObject_.vertices[i].x, clusterObject_.vertices[i].y));
			}

			b2ChainShape chain;
			//this is not working in the new box2d version lest wait for the next in 2026
			chain.CreateLoop(&vec2Vector[0], clusterObject_.vertices.size());
			clusterObject_.physics_properties->shape = &chain;

			createFixtureForBody(clusterObject_);
		}

		void PhysicsWorld::createEdgeShape(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			std::vector<b2Vec2> vec2Vector;
			vec2Vector.reserve(clusterObject_.vertices.size());//Resize so that all our pointers point to the right place
			for (unsigned i = 0; i < clusterObject_.vertices.size(); i++)
			{
				vec2Vector.push_back(b2Vec2(clusterObject_.vertices[i].x, clusterObject_.vertices[i].y));
			}
			b2EdgeShape shape;
			for (unsigned i = 0; i < clusterObject_.vertices.size() - 1; i++)
			{
				shape.SetTwoSided(b2Vec2(clusterObject_.vertices[i].x, clusterObject_.vertices[i].y), b2Vec2(clusterObject_.vertices[i + 1].x, clusterObject_.vertices[i + 1].y));
				clusterObject_.physics_properties->fixtures.push_back(clusterObject_.physics_properties->body->CreateFixture(&shape, clusterObject_.density));
			}
		}

		void PhysicsWorld::createRevoluteJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_, SquareBox::GWOM::Joint& joint_)
		{
			b2RevoluteJointDef jointDef;
			if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectA_.cluster_index, clusterObjectA_.index)) {
				jointDef.bodyA = clusterObjectA_.physics_properties->body;
				jointDef.bodyB = clusterObjectB_.physics_properties->body;
				jointDef.localAnchorA.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorB.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectB_.cluster_index, clusterObjectB_.index)) {
				jointDef.bodyA = clusterObjectB_.physics_properties->body;
				jointDef.bodyB = clusterObjectA_.physics_properties->body;
				jointDef.localAnchorB.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorA.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else {
				SBX_CRITICAL("Invalid Joint Coordinates");
			}
			jointDef.upperAngle = joint_.upper_angle;
			jointDef.lowerAngle = joint_.lower_angle;
			jointDef.enableLimit = joint_.enable_limit;
			jointDef.enableMotor = joint_.enable_motor;
			jointDef.motorSpeed = joint_.motor_speed;
			jointDef.maxMotorTorque = joint_.max_motor_torque;
			jointDef.collideConnected = joint_.collide_connected_bodies;
			jointDef.referenceAngle = joint_.reference_angle;
			b2Joint * joint_ptr = m_world->CreateJoint(&jointDef);
			joint_.is_joint_alive = true;
			clusterObjectA_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectA_.joints.push_back(&joint_);

			clusterObjectB_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectB_.joints.push_back(&joint_);
		}

		void PhysicsWorld::createDistanceJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_, SquareBox::GWOM::Joint & joint_)
		{
			b2DistanceJointDef jointDef;
			if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectA_.cluster_index, clusterObjectA_.index)) {
				jointDef.bodyA = clusterObjectA_.physics_properties->body;
				jointDef.bodyB = clusterObjectB_.physics_properties->body;
			}
			else if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectB_.cluster_index, clusterObjectB_.index)) {
				jointDef.bodyA = clusterObjectB_.physics_properties->body;
				jointDef.bodyB = clusterObjectA_.physics_properties->body;
			}
			else {
				SBX_CRITICAL("Invalid Joint Coordinates");
			}

			jointDef.length = joint_.length;
			jointDef.stiffness = joint_.stiffness;
			jointDef.collideConnected = joint_.collide_connected_bodies;
			jointDef.maxLength = joint_.max_length;
			jointDef.minLength = joint_.min_length;
			b2LinearStiffness(jointDef.stiffness, jointDef.damping, joint_.frequency_hz, joint_.damping_ratio, jointDef.bodyA, jointDef.bodyB);
			joint_.is_joint_alive = true;
			b2Joint * joint_ptr = m_world->CreateJoint(&jointDef);

			clusterObjectA_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectA_.joints.push_back(&joint_);

			clusterObjectB_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectB_.joints.push_back(&joint_);
		}

		void PhysicsWorld::createPrismaticJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_, SquareBox::GWOM::Joint & joint_)
		{
			b2PrismaticJointDef jointDef;
			if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectA_.cluster_index, clusterObjectA_.index)) {
				jointDef.bodyA = clusterObjectA_.physics_properties->body;
				jointDef.bodyB = clusterObjectB_.physics_properties->body;
				jointDef.localAnchorA.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorB.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectB_.cluster_index, clusterObjectB_.index)) {
				jointDef.bodyA = clusterObjectB_.physics_properties->body;
				jointDef.bodyB = clusterObjectA_.physics_properties->body;
				jointDef.localAnchorB.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorA.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else {
				SBX_CRITICAL("Invalid Joint Coordinates");
			}
			jointDef.localAxisA = b2Vec2(joint_.local_axis_a.x, joint_.local_axis_a.y);
			jointDef.localAxisA.Normalize();
			jointDef.upperTranslation = joint_.upper_translation;
			jointDef.lowerTranslation = joint_.lower_translation;
			jointDef.enableLimit = joint_.enable_limit;
			jointDef.enableMotor = joint_.enable_motor;
			jointDef.motorSpeed = joint_.motor_speed;
			jointDef.maxMotorForce = joint_.max_motor_force;
			jointDef.collideConnected = joint_.collide_connected_bodies;
			jointDef.referenceAngle = joint_.reference_angle;
			b2Joint * joint_ptr = m_world->CreateJoint(&jointDef);
			joint_.is_joint_alive = true;
			clusterObjectA_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectA_.joints.push_back(&joint_);
			clusterObjectB_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectB_.joints.push_back(&joint_);
		}

		void PhysicsWorld::createPulleyJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_, SquareBox::GWOM::Joint & joint_)
		{
			b2PulleyJointDef jointDef;
			if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectA_.cluster_index, clusterObjectA_.index)) {
				jointDef.bodyA = clusterObjectA_.physics_properties->body;
				jointDef.bodyB = clusterObjectB_.physics_properties->body;
				jointDef.localAnchorA.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorB.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else if (joint_.body_a_coordinates == std::pair<int, int>(clusterObjectB_.cluster_index, clusterObjectB_.index)) {
				jointDef.bodyA = clusterObjectB_.physics_properties->body;
				jointDef.bodyB = clusterObjectA_.physics_properties->body;
				jointDef.localAnchorB.Set(joint_.local_anchor_a.x, joint_.local_anchor_a.y);
				jointDef.localAnchorA.Set(joint_.local_anchor_b.x, joint_.local_anchor_b.y);
			}
			else {
				SBX_CRITICAL("Invalid Joint Coordinates");
			}
			jointDef.groundAnchorA = b2Vec2(joint_.ground_anchor_a.x, joint_.ground_anchor_a.y);
			jointDef.groundAnchorB = b2Vec2(joint_.ground_anchor_b.x, joint_.ground_anchor_b.y);
			jointDef.lengthA = joint_.length_a;
			jointDef.lengthB = joint_.length_b;
			jointDef.ratio = joint_.pulley_ratio;

			jointDef.collideConnected = joint_.collide_connected_bodies;
			b2Joint * joint_ptr = m_world->CreateJoint(&jointDef);
			joint_.is_joint_alive = true;
			clusterObjectA_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectA_.joints.push_back(&joint_);

			clusterObjectB_.physics_properties->joints.push_back(joint_ptr);
			clusterObjectB_.joints.push_back(&joint_);
		}

		glm::vec2 PhysicsWorld::getJointsReactionForce(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_)
		{
			b2Joint* joint_ptr = getSharedJoint(clusterObjectA_, clusterObjectB_);
			if (joint_ptr != nullptr) {
				b2Vec2 reactionForce = joint_ptr->GetReactionForce(1 / m_timeStep);
				return glm::vec2(reactionForce.x, reactionForce.y);
			}
			return glm::vec2(0.0f);
		}

		float PhysicsWorld::getJointsReactionTorque(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_)
		{
			b2Joint* joint_ptr = getSharedJoint(clusterObjectA_, clusterObjectB_);
			if (joint_ptr != nullptr) {
				return joint_ptr->GetReactionTorque(1 / m_timeStep);
			}
			return 0.0f;
		}

		glm::vec2 PhysicsWorld::getJointsAnchorPoint(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, bool getForBodyA)
		{
			b2Joint* joint_ptr = getSharedJoint(clusterObjectA_, clusterObjectB_);
			if (joint_ptr != nullptr) {
				if (getForBodyA) {
					b2Vec2 anchorPoint = joint_ptr->GetAnchorA();
					return glm::vec2(anchorPoint.x, anchorPoint.y);
				}
				else {
					b2Vec2 anchorPoint = joint_ptr->GetAnchorB();
					return glm::vec2(anchorPoint.x, anchorPoint.y);
				}
			}
			SBX_INFO("No Joint Found");
			return glm::vec2(0.0f);
		}

		bool PhysicsWorld::isJointLimitEnabled(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->IsLimitEnabled();
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->IsLimitEnabled();
				}
			}
			SBX_CORE_INFO("This joint does not support Limits");
			return false;
		}

		bool PhysicsWorld::isMotorLimitEnabled(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->IsMotorEnabled();
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->IsMotorEnabled();
				}
			}
			SBX_CORE_INFO("This joint does not support Motors");
			return false;
		}

		float PhysicsWorld::getJointUpperLimit(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetUpperLimit();
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetUpperLimit();
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Limits");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointLowerLimit(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetLowerLimit();
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetLowerLimit();
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Limits");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointMotorSpeed(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMotorSpeed();
				}
			}
			else if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMotorSpeed();
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Joints");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointMotorForce(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMotorForce(1.0f);
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Motor Force");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointMaxMotorForce(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::prismatic_joint) {
				b2PrismaticJoint* joint_ptr = static_cast<b2PrismaticJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMaxMotorForce();
				}
			}
			else {
				SBX_CORE_INFO("This joint does not support Motor Force");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointMotorTorque(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMotorTorque(1.0f);
				}
			}

			else {
				SBX_CORE_INFO("This joint does not support Motor Torque");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointMaxMotorTorque(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::revolute_joint) {
				b2RevoluteJoint* joint_ptr = static_cast<b2RevoluteJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetMaxMotorTorque();
				}
			}

			else {
				SBX_CORE_INFO("This joint does not support Motor Torque");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointLengthA(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::pulley_joint) {
				b2PulleyJoint* joint_ptr = static_cast<b2PulleyJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetLengthA();
				}
			}

			else {
				SBX_CORE_INFO("This joint is not a pulley joint");
			}
			return 0.0f;
		}

		float PhysicsWorld::getJointLengthB(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_, const SquareBox::JointTypeEnum jointType_)
		{
			if (jointType_ == SquareBox::JointTypeEnum::pulley_joint) {
				b2PulleyJoint* joint_ptr = static_cast<b2PulleyJoint*>(getSharedJoint(clusterObjectA_, clusterObjectB_));
				if (joint_ptr != nullptr) {
					return joint_ptr->GetLengthB();
				}
			}

			else {
				SBX_CORE_INFO("This joint is not a pulley joint");
			}
			return 0.0f;
		}

		void PhysicsWorld::createFixtureForBody(const SquareBox::GWOM::ClusterObject &clusterObject_)
		{
			b2FixtureDef fixtureDef;
			fixtureDef.shape = clusterObject_.physics_properties->shape;
			fixtureDef.density = clusterObject_.density;
			fixtureDef.friction = clusterObject_.friction;
			fixtureDef.restitution = clusterObject_.restitution;
			fixtureDef.filter.groupIndex = clusterObject_.group_index;
			fixtureDef.filter.categoryBits = static_cast<unsigned int>(clusterObject_.family);

			uint16 tempcategoryBits = 0;
			for (unsigned int i = 0; i < clusterObject_.vec_of_mask_bits.size(); i++)
			{
				tempcategoryBits |= clusterObject_.vec_of_mask_bits[i];
			}

			fixtureDef.filter.maskBits = tempcategoryBits;

			clusterObject_.physics_properties->fixtures[0] = clusterObject_.physics_properties->body->CreateFixture(&fixtureDef);
			clusterObject_.physics_properties->body->ResetMassData();
		}
		b2Joint * PhysicsWorld::getSharedJoint(const SquareBox::GWOM::ClusterObject & clusterObjectA_, const SquareBox::GWOM::ClusterObject & clusterObjectB_)
		{
			//loop through bodyAs joints and find one where these two are members
			for (unsigned int i = 0; i < clusterObjectA_.physics_properties->joints.size(); i++)
			{
				if (

					(clusterObjectA_.physics_properties->joints[i]->GetBodyA() == clusterObjectA_.physics_properties->body
						&&
						clusterObjectB_.physics_properties->joints[i]->GetBodyB() == clusterObjectB_.physics_properties->body)
					||
					(clusterObjectA_.physics_properties->joints[i]->GetBodyA() == clusterObjectB_.physics_properties->body
						&&
						clusterObjectB_.physics_properties->joints[i]->GetBodyB() == clusterObjectA_.physics_properties->body)
					) {
					return clusterObjectB_.physics_properties->joints[i];
				}
			}
			return nullptr;
		}
	}
}