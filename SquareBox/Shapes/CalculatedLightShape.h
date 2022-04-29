#pragma once
#include <SquareBox-Core/enums.h>
#include <SquareBox-Core/IShape.h>
#include <Physics-Collision-Engine/PhysicsCollisionEngine.h>
#include <Render-Engine/RenderingEngine.h>
#include <glm/glm.hpp>
#include <GWOM/Entities.h>

namespace SquareBox {
	namespace Shapes {
		class CalculatedLightShape :public SquareBox::IShape
		{
		public:
			CalculatedLightShape() {
				body_shape = SquareBox::BodyShapeEnum::CalculatedLightShape;
				needs_height_width = false;
				needs_radius = false;
				is_calculated_light = true;
				can_be_joint_memeber = false;
				is_plotted = false;
				displacement_based_on_centroid = false;
				is_selected_through_ends = false;
				min_plotting_vertices = 2;
			};
			~CalculatedLightShape() {};
			// Inherited via Shape
			virtual void draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_ = 100) override;
			virtual void keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_) override;
			virtual void debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_) override;
			virtual void traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_) override;
			virtual void scalingDraw(SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2& mouseinworld_, SquareBox::InputManager::IInputDevice * input_)override {};
			// Inherited via Shape
			virtual bool containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_) override;
			// Inherited via Shape
			virtual glm::vec2 localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_) override;
		private:
			void calculateVisiblityPolygon(const glm::vec2  &orign, const float radius, const std::vector<std::pair<glm::vec2, glm::vec2>> &vecEdges, const SquareBox::RenderEngine::ColorRGBA8 &  color);
			void drawTriangleLight(const glm::vec2& origin_point, const glm::vec2 & pointA, const glm::vec2 & pointB, SquareBox::RenderEngine::SpriteBatch & SpriteBatch_, const SquareBox::RenderEngine::ColorRGBA8 & color, bool maxLengthSet = false, float maxLength = 0);
			std::vector<std::tuple<float, float, float >> vecVisibilityPolygonPoints;
			void drawRay(const glm::vec2& a, const glm::vec2& b, SquareBox::RenderEngine::SpriteBatch & SpriteBatch_, const SquareBox::RenderEngine::ColorRGBA8& color);
			bool isInsideTriangle(const glm::vec2 & origin_, const  glm::vec2 & pointA_, const glm::vec2 & pointB_, const glm::vec2 & point_);
			float area(float x1, float y1, float x2, float y2, float x3, float y3);
		};
	};
};
