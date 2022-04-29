#pragma once
#pragma once
#include <vector>
#include <sstream> //stringstream
#include <cstring>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <utility>
#include <GWOM/Entities.h>
#include <Render-Engine/DebugRenderer.h>
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
#include <Shapes/Shapes.h>
namespace SquareBox {
	class Utilities
	{
	public:
		struct creationDetails {
			std::pair<int, int> settlementCoordinates; // where the cluster objects has been placed
			std::pair<int, int> shellCoordinates; // where the next slot for a cluster object is
		};
		Utilities();
		~Utilities();
		void init();
		glm::vec2 compute2DPolygonCentroid(const std::vector<glm::vec2>&  vertices_);
		void addIntToVector(std::vector<int> &vector_, int num_, bool allowdups_ = false);
		void removeIntFromVector(std::vector<int> &vector_, int num_, bool removedups_ = true);
		void removePairFromVector(std::vector<std::pair<int,int>> &vector_, std::pair<int,int> target_, bool removedups_ = true);
		bool isIntVectorMember(std::vector<int> &vector_, int num_);
		
		void rotatePoint(glm::vec2 &position, float angle);
		//pairs vector
		void addPairToVector(std::vector<std::pair<int, int>> &vector_, std::pair<int, int> pair_, bool allowdups_ = false);
		void movePairInVector(std::vector<std::pair<int, int>> &vector_, std::pair<int, int> pair_, int movementDisplacement);
		bool isPairVectorMember(std::vector<std::pair<int, int>> &vector_, std::pair<int, int> pair_);

		void nameWorldClusterByIndex(SquareBox::GWOM::WorldCluster & world_cluster);
		void nameWorldClusterByGivenName(SquareBox::GWOM::WorldCluster & world_cluster_, const std::string new_name_);

		void nameClusterObjectByIndex(SquareBox::GWOM::ClusterObject & cluster_object);

		void nameClusterObjectByGivenName(SquareBox::GWOM::ClusterObject & cluster_object_, const std::string new_name_);

		void worldIndiciesCleanUp(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, std::vector<SquareBox::GWOM::Layer>& layers_);
		void showPlottedVertices(bool onlyshowStartAndEnd_, SquareBox::RenderEngine::DebugRenderer& debugRenderer_, const SquareBox::GWOM::ClusterObject& clusterObject_, float pointRadius_, SquareBox::RenderEngine::ColorRGBA8 color_ = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
		void deactivateClusterObjectJoints(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, SquareBox::GWOM::ClusterObject & clusterObject_);
		void destroryClusterObjectJoints(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, SquareBox::GWOM::ClusterObject & clusterObject_);
		void reactivateClusterObjectJoints(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::PhysicsCollisionEngine::PhysicsWorld& physics_world_);
		void destroyClusterObjectJoint(SquareBox::GWOM::ClusterObject & clusterObjectA_, SquareBox::GWOM::ClusterObject & clusterObjectB_);
		void createWorldClusterJoints(std::vector<SquareBox::GWOM::WorldCluster>& world_clusters_, SquareBox::PhysicsCollisionEngine::PhysicsWorld& physics_world_);
		bool isInBox(const  glm::vec2& testCoordinates_, const glm::vec4& testBoxDestRect_);
		//create world objects
		creationDetails createClusterObjectIntoWorld(
			bool drawingPlotedCords,/*
									this is set to true only when we are now actually submiting the plotted coordinates
									that we have been accumulating
									*/
			bool rebirth,   // whether we are recreating object , most likey with edited attributes
			bool editPosition,/*
								do we want to change the clusterObjects position to the one supplied?
								this is usually only true for dragging objects,and none plotted shapes
							  */
			glm::vec2 newPosition_,

			SquareBox::GWOM::ClusterObject * ClusterObject_,/*
															 -----old comment-----
															 the cluster Object we are acting upon , if autoNewCluster Object is enabled
															 this clusterObject pointer will then point to a new clusterObject*/
															/* 
															---new comment----
															the old comment is not clear but this is what i was
															able to pick from it.
															this points to the cluster object that we are acting upon , either adding to worldClusters
															vector or rebirthing.
															*/

			std::vector<SquareBox::GWOM::WorldCluster>& worldClusters_,
			int shellWorldClusterIndex_,
			SquareBox::PhysicsCollisionEngine::PhysicsWorld* targetPhysicsWorld_,
			bool autoNewClusterObjectCreation,/*
													Controls whether if a new cluster Objects should be added to the worldCluster once
													the current clusterObject has been created , this is by default expected to be true
													Except for when we are loading in a level,this becomes false when rebirth is true
													*/
			bool respectAncestor_					/* should the new created shell copy its ancestors attributes*/
		);
		void setCurrentShapePointer(SquareBox::BodyShapeEnum enum_, SquareBox::IShape** currentShape_);//used to prevent us from calling a wrong shapes
		float scaleFloatAccordingToScreen(float original, glm::ivec2 orignalMaxWindow_,glm::ivec2 currentMaxWindow_);
		void dispose();
	private:
		std::vector<SquareBox::IShape*> m_vec_shapes_pointer;
	};
};
