#include "PointLightShape.h"
#include<Input-Manager/IInputDevice.h>
void SquareBox::Shapes::PointLightShape::draw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_, int opacity_)
{
	if (!clusterObject_.is_hidden) {
		const glm::vec2 & CenterCoordinates = clusterObject_.position;
		const float & magnifier = 5;
		const float & radius = clusterObject_.radius *magnifier;
		const glm::vec4 & destRect = glm::vec4(CenterCoordinates.x - radius * 0.5f, CenterCoordinates.y - radius * 0.5f, radius, radius);
		//this will give us a box grid with the source of our light
		// as the center and the brightest also,
		//the further away from the center the less bright a light is
		const glm::vec4 & uvRect = glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f);

		spriteBatch_.draw(destRect, uvRect, 0, 0.0f, SquareBox::RenderEngine::ColorRGBA8(( clusterObject_.color.x), ( clusterObject_.color.y), ( clusterObject_.color.z), ( clusterObject_.color.w)), 0);
	}
}

void SquareBox::Shapes::PointLightShape::keepTrackOfPosition(SquareBox::GWOM::ClusterObject & clusterObject_)
{
}

bool SquareBox::Shapes::PointLightShape::containtsPoint(SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & coordinates_)
{
	if (clusterObject_.is_hidden) { return false; }
	return ((glm::length(glm::vec2(clusterObject_.position) - coordinates_) <= clusterObject_.radius*0.3));
}

void SquareBox::Shapes::PointLightShape::debugDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  SquareBox::RenderEngine::ColorRGBA8 borderColor_)
{
	debugRender_.drawCircle(clusterObject_.position, borderColor_, clusterObject_.radius*0.3);
	debugRender_.drawCircle(clusterObject_.position, borderColor_, clusterObject_.radius);
}

void SquareBox::Shapes::PointLightShape::traceDraw(const SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const  glm::vec2 & mouseinworld, const  SquareBox::RenderEngine::ColorRGBA8 & borderColor_, const  float cameraScale_)
{
	debugRender_.drawCircle(mouseinworld, borderColor_, clusterObject_.radius);
}

void SquareBox::Shapes::PointLightShape::scalingDraw(SquareBox::GWOM::ClusterObject & clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2 & mouseinworld_, SquareBox::InputManager::IInputDevice * input_)
{
	//we are relying on the results of keepTrackOfPosition();
	glm::vec2 center = clusterObject_.position;
	float angle = 0.0f;

	//setting up our dimensions
	const float & scaler_length = clusterObject_.radius*2.2;
	rotation_radius = clusterObject_.radius*0.3;

	//i have rotation all wrong , so angle is const 0.0f for now
	const glm::vec2	vertical_dest_point = glm::rotate(glm::vec2(center.x, center.y + scaler_length), angle);
	const glm::vec2	horizontal_dest_point = glm::rotate(glm::vec2(center.x + scaler_length, center.y), angle);
	const glm::vec2	general_dest_point = glm::rotate(glm::vec2(center.x + scaler_length, center.y + scaler_length), angle);

	//Vertical line
	debugRender_.drawLine(center, vertical_dest_point, vertical_scaling_color);
	//the selection circle
	debugRender_.drawCircle(vertical_dest_point, vertical_scaling_color, scaler_length*selection_circle_percentage);
	if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
		if ((glm::length(vertical_dest_point - mouseinworld_) <= scaler_length * selection_circle_percentage)) {
			m_horizontal_scaling_selected = false;
			m_vertical_scaling_selected = true;
			m_general_scaling_selected = false;
		}
	}

	if (m_vertical_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
		float difference = mouseinworld_.y - vertical_dest_point.y;
		clusterObject_.radius += difference;
		if (clusterObject_.radius < 0.0f) {
			clusterObject_.radius = 0.0f;
		}
		if (clusterObject_.physics_properties != nullptr) {
			clusterObject_.requesting_recreation = true;
		}
	}

	//Horizontal line
	debugRender_.drawLine(center, horizontal_dest_point, horizontal_scaling_color);
	//the selection circle
	debugRender_.drawCircle(horizontal_dest_point, horizontal_scaling_color, scaler_length*selection_circle_percentage);
	//if the circle is clicked scale according to where the mouse is
		//to select a direction for scaling , ctrl must be pressed
	if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
		if ((glm::length(horizontal_dest_point - mouseinworld_) <= scaler_length * selection_circle_percentage)) {
			m_horizontal_scaling_selected = true;
			m_vertical_scaling_selected = false;
			m_general_scaling_selected = false;
		}
	}

	if (m_horizontal_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
		float difference = mouseinworld_.x - horizontal_dest_point.x;
		clusterObject_.radius += difference;
		if (clusterObject_.radius < 0.0f) {
			clusterObject_.radius = 0.0f;
		}
		if (clusterObject_.physics_properties != nullptr) {
			clusterObject_.requesting_recreation = true;
		}
	}

	//General line
	debugRender_.drawLine(center, general_dest_point, general_scaling_color);
	//the selection circle
	debugRender_.drawCircle(general_dest_point, general_scaling_color, scaler_length*selection_circle_percentage);
	if (input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_CTRL))) {
		if ((glm::length(general_dest_point - mouseinworld_) <= scaler_length * selection_circle_percentage)) {
			m_horizontal_scaling_selected = false;
			m_vertical_scaling_selected = false;
			m_general_scaling_selected = true;
		}
	}

	if (m_general_scaling_selected && input_->isInputIdBeingReceived(static_cast<int>(SquareBox::KeyBoardEnum::LEFT_SHIFT))) {
		float difference = glm::length(general_dest_point - mouseinworld_);
		clusterObject_.radius += difference;
		if (clusterObject_.radius < 0.0f) {
			clusterObject_.radius = 0.0f;
		}
		if (clusterObject_.physics_properties != nullptr) {
			clusterObject_.requesting_recreation = true;
		}
	}
	//rotation Circle
	debugRender_.drawCircle(center, rotation_color, rotation_radius);
}

glm::vec2 SquareBox::Shapes::PointLightShape::localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const  glm::vec2 & localCoordinates_)
{
	const glm::vec2 & CenterCoordinates = clusterObject_.position;

	return glm::vec2(CenterCoordinates.x + localCoordinates_.x, CenterCoordinates.y + localCoordinates_.y);
}