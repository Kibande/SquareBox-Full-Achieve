#include "PolygonShape.h"
#include<Input-Manager/IInputDevice.h>

namespace SquareBox {
	namespace Shapes {
		void PolygonShape::draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_)
		{
			if (!clusterObject_.is_hidden) {
				//A box
				const glm::vec2 & boxDims = glm::vec2(clusterObject_.width, clusterObject_.height);
				const glm::vec2 & boxOrigin = glm::vec2(clusterObject_.position) - boxDims * 0.5f;
				spriteBatch_.draw(glm::vec4(boxOrigin, clusterObject_.width, clusterObject_.height), clusterObject_.texture_info.uv_rect, clusterObject_.texture_info.texture_id, 0.0f, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.color.x), ( clusterObject_.color.y), ( clusterObject_.color.z), ( clusterObject_.color.w)), clusterObject_.angle);
			}
		}

		void PolygonShape::keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_)
		{
			if (clusterObject_.physics_properties != nullptr && clusterObject_.physics_properties->isIntialised()) {
				clusterObject_.vertices = clusterObject_.physics_properties->getPolygonVerticesCoordinates();
				clusterObject_.angle = clusterObject_.physics_properties->getAngle();
			}
		}

		void PolygonShape::debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  SquareBox::RenderEngine::ColorRGBA8 borderColor_)
		{
			debugRender_.drawPolygon(clusterObject_.vertices, borderColor_, clusterObject_.angle);
		}

		void PolygonShape::traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_)
		{
			debugRender_.drawPolygon(clusterObject_.vertices, borderColor_, clusterObject_.angle);
			for (size_t i = 0; i < clusterObject_.vertices.size(); i++)
			{
				debugRender_.drawCircle(clusterObject_.vertices[i], borderColor_, 1.0f / cameraScale_);
			}
		}

		void PolygonShape::scalingDraw(SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2 & mouseinworld_, SquareBox::InputManager::IInputDevice * input_)

		{
			//this is so far only implemented for Shapes with properly defined dimensions

			//we are relying on the results of keepTrackOfPosition();
			float angle = 0.0f;
			const glm::vec2 & center = clusterObject_.vertices[0];

			const float scaler_length = 0.1f;
			rotation_radius = 0.075f;
			//i have rotation all wrong , so angle is const 0.0f for now
			const glm::vec2	vertical_dest_point = glm::rotate(glm::vec2(center.x, center.y + scaler_length), angle);
			const glm::vec2	horizontal_dest_point = glm::rotate(glm::vec2(center.x + scaler_length, center.y), angle);
			const glm::vec2	general_dest_point = glm::rotate(glm::vec2(center.x + scaler_length, center.y + scaler_length), angle);

			//Vertical line
			debugRender_.drawLine(center, vertical_dest_point, vertical_scaling_color);
			//the selection circle
			debugRender_.drawCircle(vertical_dest_point, vertical_scaling_color, scaler_length*selection_circle_percentage);

			//Horizontal line
			debugRender_.drawLine(center, horizontal_dest_point, horizontal_scaling_color);
			//the selection circle
			debugRender_.drawCircle(horizontal_dest_point, horizontal_scaling_color, scaler_length*selection_circle_percentage);

			//General line
			debugRender_.drawLine(center, general_dest_point, general_scaling_color);
			//the selection circle
			debugRender_.drawCircle(general_dest_point, general_scaling_color, scaler_length*selection_circle_percentage);

			//rotation Circle
			debugRender_.drawCircle(center, rotation_color, rotation_radius);
		}

		bool PolygonShape::containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_)
		{
			if (clusterObject_.is_hidden) { return false; }

			//selectable through first and last points
			const int numVertices = clusterObject_.vertices.size();
			return (glm::length(clusterObject_.vertices[0] - coordinates_) <= 0.03)
				|| (glm::length(clusterObject_.vertices[numVertices - 1] - coordinates_) <= 0.03);
		}
		glm::vec2 PolygonShape::localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_)
		{
			return glm::vec2(clusterObject_.position.x + localCoordinates_.x, clusterObject_.position.y + localCoordinates_.y);
		}
	};
};