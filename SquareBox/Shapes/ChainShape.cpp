#include "ChainShape.h"
#include<Input-Manager/IInputDevice.h>

namespace SquareBox {
	namespace Shapes {
		void ChainShape::draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_)
		{
			//not drawn by the spriteBatch
		}

		void ChainShape::keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_)
		{
			if (clusterObject_.physics_properties != nullptr && clusterObject_.physics_properties->isIntialised()) {
				clusterObject_.vertices = clusterObject_.physics_properties->getChainVerticesCoordinates();
			}
		}

		void ChainShape::debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  SquareBox::RenderEngine::ColorRGBA8 borderColor_)
		{
			debugRender_.drawPolygon(clusterObject_.vertices, borderColor_, clusterObject_.angle);
		}

		void ChainShape::traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & coordinates_, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_)
		{
			debugRender_.drawPolygon(clusterObject_.vertices, borderColor_, clusterObject_.angle);
			for (size_t i = 0; i < clusterObject_.vertices.size(); i++)
			{
				debugRender_.drawCircle(clusterObject_.vertices[i], borderColor_, 1.0f / cameraScale_);
			}
		}

		void ChainShape::scalingDraw(SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2 & mouseinworld_, SquareBox::InputManager::IInputDevice * input_)
		{
			//this is so far only implemented for Shapes with properly difined dimensions

			//we are relying on the results of keepTrackOfPosition();
			glm::vec2 center = clusterObject_.position;
			float angle = 0.0f;
			if (clusterObject_.is_first_selected)
			{
				center = clusterObject_.vertices[0];
			}
			else if (clusterObject_.is_last_selected)
			{
				center = clusterObject_.vertices[clusterObject_.vertices.size() - 1];
			}
			const float & scaler_length = 0.1;
			rotation_radius = 0.075;
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

		bool ChainShape::containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_)
		{
			if (clusterObject_.is_hidden) { return false; }

			//selectable through first and last points
			const int & numVertices = clusterObject_.vertices.size();
			if ((glm::length(clusterObject_.vertices[0] - coordinates_) <= 0.03)) {
				clusterObject_.is_first_hovered = true;
				clusterObject_.is_last_hovered = false;
				return true;
			}
			else if ((glm::length(clusterObject_.vertices[numVertices - 1] - coordinates_) <= 0.03)) {
				clusterObject_.is_first_hovered = false;
				clusterObject_.is_last_hovered = true;
				return true;
			}
			else {
				return false;
			}
		}
		glm::vec2 ChainShape::localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_)
		{
			return glm::vec2(0, 0);
		}
	};
};