#pragma once
#include <SquareBox-Core/enums.h>
#include "IShape.h"
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
#include <Render-Engine/RenderingEngine.h>
#include <glm/glm.hpp>
#include <GWOM/Entities.h>

namespace SquareBox {
	namespace Shapes {
		class PointLightShape :public SquareBox::IShape
		{
		public:
			PointLightShape() {
				body_shape = SquareBox::BodyShapeEnum::PointLightShape;
				needs_height_width = false;
				needs_radius = true;
				can_be_joint_memeber = false;
				is_plotted = false;
				displacement_based_on_centroid = false;
				is_selected_through_ends = false;
				min_plotting_vertices = 0;
			};
			~PointLightShape() {};
			// Inherited via Shape
			virtual void draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_ = 100) override;
			virtual void keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_) override;
			virtual void debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_) override;
			virtual void traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_) override;
			virtual void scalingDraw(SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2& mouseinworld_, SquareBox::InputManager::IInputDevice * input_)override;
			// Inherited via Shape
			virtual bool containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_) override;
			// Inherited via Shape
			virtual glm::vec2 localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_) override;
		};
	};
};
