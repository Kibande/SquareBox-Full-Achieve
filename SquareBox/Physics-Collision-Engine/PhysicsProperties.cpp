#include "PhysicsProperties.h"
namespace SquareBox {
	namespace PhysicsCollisionEngine
	{
		PhysicsProperties::PhysicsProperties()
		{
		}
		PhysicsProperties::~PhysicsProperties()
		{
		}
		void PhysicsProperties::init(int numFixtures_)
		{
			if (numFixtures_ > 0) {
				fixtures.reserve(numFixtures_);
				fixtures = std::vector<b2Fixture*>(numFixtures_);
			}
		}

		glm::vec4 PhysicsProperties::getBoxCoordinates()
		{
			glm::vec4 destRect;
			destRect.x = body->GetPosition().x - dimensions.x / 2.0f;
			destRect.y = body->GetPosition().y - dimensions.y / 2.0f;
			destRect.z = dimensions.x;
			destRect.w = dimensions.y;
			return destRect;
		}

		glm::vec2 PhysicsProperties::getCircleCoordinates()
		{
			glm::vec2 distCir;

			distCir.x = body->GetPosition().x;
			distCir.y = body->GetPosition().y;
			return distCir;
		}

		float PhysicsProperties::getAngle()
		{
			return body->GetAngle();
		}

		std::vector<glm::vec2> PhysicsProperties::getPolygonVerticesCoordinates()
		{
			std::vector<b2Vec2> vectorOfb2Vec2Vertices;
			std::vector<glm::vec2> vectorOfGlmVec2Vertices;
			//determine shape
			//polygons have only one fixture
			b2Shape::Type shapeType = fixtures[0]->GetType();
			auto shape = fixtures[0]->GetShape();
			b2Transform transformation;
			transformation.Set(body->GetPosition(), body->GetAngle());

			if (shapeType == b2Shape::e_polygon)
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)shape;
				//for our polygons

				for (size_t j = 0; j < polygonShape->m_count; ++j)
				{
					//std::cout << "j "<<j<< std::endl;
					vectorOfb2Vec2Vertices.push_back(b2Mul(body->GetTransform(), polygonShape->m_vertices[j]));
					//vectorOfb2Vec2Vertices.push_back(polygonShape->m_vertices[j]);
				}
			}

			vectorOfGlmVec2Vertices.clear();

			for (size_t i = 0; i < vectorOfb2Vec2Vertices.size(); i++)
			{
				vectorOfGlmVec2Vertices.emplace_back(vectorOfb2Vec2Vertices[i].x, vectorOfb2Vec2Vertices[i].y);
			}

			return vectorOfGlmVec2Vertices;
		}

		std::vector<glm::vec2> PhysicsProperties::getEdgeVerticesCoordinates()
		{
			std::vector<b2Vec2> vectorOfb2Vec2Vertices;
			std::vector<glm::vec2> vectorOfGlmVec2Vertices;
			//determine shape

			b2Transform transformation;
			transformation.Set(body->GetPosition(), body->GetAngle());

			b2EdgeShape* edgeShape;
			//Doesn't an Edge have only one fixture
			for (size_t i = 0; i < fixtures.size(); i++)
			{
				edgeShape = (b2EdgeShape*)fixtures[i]->GetShape();
				vectorOfb2Vec2Vertices.push_back(b2Mul(body->GetTransform(), edgeShape->m_vertex1));
				vectorOfb2Vec2Vertices.push_back(b2Mul(body->GetTransform(), edgeShape->m_vertex2));
			}
			vectorOfGlmVec2Vertices.clear();
			for (size_t i = 0; i < vectorOfb2Vec2Vertices.size(); i++)
			{
				vectorOfGlmVec2Vertices.emplace_back(vectorOfb2Vec2Vertices[i].x, vectorOfb2Vec2Vertices[i].y);
			}

			return vectorOfGlmVec2Vertices;
		}

		std::vector<glm::vec2> PhysicsProperties::getChainVerticesCoordinates()
		{
			std::vector<b2Vec2> vectorOfb2Vec2Vertices;
			std::vector<glm::vec2> vectorOfGlmVec2Vertices;
			//determine shape

			b2Transform transformation;
			transformation.Set(body->GetPosition(), body->GetAngle());
			//chains have one fixture , just like most objects
			auto shape = fixtures[0]->GetShape();
			b2ChainShape* chainShape = (b2ChainShape*)shape;
			for (size_t i = 0; i < chainShape->m_count; i++)
			{
				vectorOfb2Vec2Vertices.push_back(b2Mul(body->GetTransform(), chainShape->m_vertices[i]));
			}
			vectorOfGlmVec2Vertices.clear();
			for (size_t i = 0; i < vectorOfb2Vec2Vertices.size(); i++)
			{
				vectorOfGlmVec2Vertices.emplace_back(vectorOfb2Vec2Vertices[i].x, vectorOfb2Vec2Vertices[i].y);
			}

			return vectorOfGlmVec2Vertices;
		}

		glm::vec4 PhysicsProperties::getCapsuleCoordinates(bool fullCapsule, bool topCapsule)
		{
			if (fullCapsule) {
				//The Box, we shall reconstruct the capsule form this
				glm::vec4 destRect;
				destRect.x = body->GetPosition().x - dimensions.x / 2.0f;//we are pushing to the actual center of the box
				destRect.y = body->GetPosition().y - (dimensions.y - dimensions.x) / 2.0f;//we are accounting for the shricked the box's height
				destRect.z = dimensions.x;
				destRect.w = dimensions.y - dimensions.x;//Shrick

				return  destRect;
			}
			else {
				//The Box
				glm::vec4 destRect;
				destRect.x = body->GetPosition().x - dimensions.x / 2.0f;//we are pushing to the actual center of the box
				destRect.y = body->GetPosition().y - (dimensions.y - (dimensions.x * 0.5f)) / 2.0f;//we are accounting for the shricked the box's height
				destRect.z = dimensions.x;
				destRect.w = dimensions.y - (dimensions.x * 0.5f);//Shrick

				return  destRect;
			}
		}

		glm::vec2 PhysicsProperties::getLinearVelocity()
		{
			b2Vec2 templinearVelocity = body->GetLinearVelocity();

			return glm::vec2(templinearVelocity.x, templinearVelocity.y);
		}

		void PhysicsProperties::applyForceToCenter(glm::vec2 force, bool wake)
		{
			b2Vec2 forceVector;
			forceVector.x = force.x;
			forceVector.y = force.y;

			body->ApplyForceToCenter(forceVector, wake);
		}

		void PhysicsProperties::applyForce(glm::vec2 force, glm::vec2 position, bool wake)
		{
			b2Vec2 forceVector;
			forceVector.x = force.x;
			forceVector.y = force.y;
			b2Vec2 positionVector;
			positionVector.x = position.x;
			positionVector.y = position.y;
			body->ApplyForce(forceVector, positionVector, wake);
		}

		void PhysicsProperties::applyTorque(float torque_, bool wake_)
		{
			body->ApplyAngularImpulse(torque_, wake_);
		}
		bool PhysicsProperties::testPoint(float x, float y, int index) const {
			if (index < 0) {
				//negatives gives us the last element
				if (fixtures.size() > 0) {
					//only do if we have a size greater than 1
					if (fixtures[fixtures.size() - 1] != nullptr) {
						return fixtures[fixtures.size() - 1]->TestPoint(b2Vec2(x, y));
					}
					else {
						return false;
					}
				}
				else {
					return false;
				}
			}
			else {
				return fixtures[0]->TestPoint(b2Vec2(x, y));
			}
		}

		void PhysicsProperties::applyLinerImpluse(const glm::vec2 & impluse_, const glm::vec2 position_, bool wake_)
		{
			body->ApplyLinearImpulse(b2Vec2(impluse_.x, impluse_.y), b2Vec2(position_.x, position_.y), wake_);
		}

		void PhysicsProperties::setLinerVelocity(glm::vec2 & linerVelocity_)
		{
			body->SetLinearVelocity(b2Vec2(linerVelocity_.x, linerVelocity_.y));
		}

		void PhysicsProperties::applyAngularImpluse(float impulse_, bool wake_)
		{
			body->ApplyAngularImpulse(impulse_, wake_);
		}

		void PhysicsProperties::setBodyMass(float mass_)
		{
			b2MassData *massdata;
			body->GetMassData(massdata);
			massdata->mass = mass_;
			body->SetMassData(massdata);
			body->ResetMassData();
		}

		glm::vec2 PhysicsProperties::getLateralVelocity()
		{
			b2Vec2 currentRightNormal = body->GetWorldVector(b2Vec2(1, 0));
			b2Vec2 lateralVec = b2Dot(currentRightNormal, body->GetLinearVelocity()) * currentRightNormal;
			return glm::vec2(lateralVec.x, lateralVec.y);
		}

		//this returns a normalized vector
		glm::vec2 PhysicsProperties::getForwardVelocity()
		{
			b2Vec2 currentforwardNormal = body->GetWorldVector(b2Vec2(0, 1));
			b2Vec2 forwardVec = b2Dot(currentforwardNormal, body->GetLinearVelocity()) * currentforwardNormal;
			glm::vec2 result = glm::normalize(glm::vec2(forwardVec.x, forwardVec.y));
			return result;
		}

		float PhysicsProperties::getSpeedInForwardDirection()
		{
			b2Vec2 currentforwardNormal = body->GetWorldVector(b2Vec2(0, 1));
			b2Vec2 forwardVelocity = b2Vec2(getForwardVelocity().x, getForwardVelocity().y);
			float currentSpeed = b2Dot(forwardVelocity, currentforwardNormal);
			return currentSpeed;
		}

		void PhysicsProperties::dismissLateralVelocity()
		{
			b2Vec2 impluse = b2Vec2(body->GetMass()*-getLateralVelocity().x, body->GetMass()*-getLateralVelocity().y);
			body->ApplyLinearImpulse(impluse, body->GetWorldCenter(), true);
		}

		void PhysicsProperties::dismissAngularVelocity()
		{
			body->ApplyAngularImpulse(0.1f*body->GetInertia()*body->GetAngularVelocity(), true);
		}

		bool PhysicsProperties::isOnGround()
		{
			if (dimensions.x > 0 && dimensions.y > 0) {
				//loop through all the contact points that our body has and determine if we are
							//actually standing on something

				for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next)
				{
					//get the actual point
					b2Contact* contactPoint = ce->contact;
					if (contactPoint->IsTouching()) {
						b2WorldManifold manifold;//Create a manifold
						//Fill the manifold with data
						contactPoint->GetWorldManifold(&manifold);
						//Check if the points are below this object
						for (unsigned i = 0; i < b2_maxManifoldPoints; i++)
						{
							if (manifold.points[i].y < body->GetPosition().y - dimensions.y / 2.0f + dimensions.x / 2.0f + 0.01f) {
								return true;
								break;
							}
						}
					}
				}
			}
			else {
				SBX_ERROR("Cant Determine On Ground for body with no Dimensions");
				return false;
			}

			return false;
		}


		void PhysicsProperties::dispose()
		{
			m_isIntialised = false;

			//when we destory a body all its joints and fixtures are atutmatically destoryed so no need for us to
			//call destory for all

			//destory the fixtures
			for (size_t i = 0; i < fixtures.size(); i++)
			{
				fixtures[i] = nullptr;
			}

			if (body) {
				world->DestroyBody(body);
				//clean up
				body = nullptr;
			}

			fixtures.clear();
			fixtures.resize(0);
			shape = nullptr;
		}
	};
}