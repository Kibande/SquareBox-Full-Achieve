#pragma once
#include <GWOM/Entities.h>
#include <Render-Engine/RenderingEngine.h>
#include <Render-Engine/DebugRenderer.h>
#include <Render-Engine/SpriteBatch.h>
#include <Render-Engine/ColorRGBA8.h>
#include <SquareBox-Core/enums.h>
#include<glm/gtx/rotate_vector.hpp>

namespace SquareBox {
	namespace InputManager {
		class IInputDevice;
	}
}

namespace SquareBox {
	class IShape
	{
	public:
		virtual ~IShape() {};
		//the assistant handles determing the draw calls
		virtual void draw(const SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::SpriteBatch & spriteBatch_,int opacity_=100) = 0;
		virtual void keepTrackOfPosition(SquareBox::GWOM::ClusterObject& clusterObject_) = 0;
		/*
			we have to some time take record in the clusterObject position and angle, this info is got from the
			Physics Engine, saved and is used when recreating objects into the world
		*/
		virtual bool containtsPoint(SquareBox::GWOM::ClusterObject& clusterObject_, const glm::vec2& coordinates_) = 0;
		virtual void debugDraw(const SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_) = 0;
		virtual void traceDraw(const SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2& mouseinworld, const SquareBox::RenderEngine::ColorRGBA8& borderColor_, const float cameraScale_) = 0;
		virtual void scalingDraw(SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::RenderEngine::DebugRenderer & debugRender_, const glm::vec2& mouseinworld_, SquareBox::InputManager::IInputDevice * input_) = 0;
		virtual glm::vec2 localCoordinatesToWorldCoordinates(const SquareBox::GWOM::ClusterObject & clusterObject_, const glm::vec2 & localCoordinates_) = 0;
		SquareBox::BodyShapeEnum body_shape;
		bool needs_height_width = false;
		bool needs_radius = false;
		bool is_calculated_light = false;
		bool can_be_joint_memeber = false;
		bool is_plotted = false;
		bool is_selected_through_ends = false;
		bool displacement_based_on_centroid = false;//this is to help when dragging
		int  min_plotting_vertices = 1;

	protected:
		//transformations
		const SquareBox::RenderEngine::ColorRGBA8 horizontal_scaling_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::red);
		const SquareBox::RenderEngine::ColorRGBA8 vertical_scaling_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green);
		const SquareBox::RenderEngine::ColorRGBA8 general_scaling_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::blue);
		const SquareBox::RenderEngine::ColorRGBA8 rotation_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::purple);
		const float selection_circle_percentage = 0.1f;
		float rotation_radius = 0.0f;

		bool m_horizontal_scaling_selected = false;
		bool m_vertical_scaling_selected = false;
		bool m_general_scaling_selected = false;
	};
}
/*
	the way we select physics and none physics bodies is different
	for a physics body we can select it though any point in its boundaries ,
	by the help of our physics engine
	while  for non physics bodies we have to hustle it out here ,our aim is to make the selection
	way feel the  same and we have that for the edges,chains and circles
	boxes have a circular selectabale radius inside them , this radius orignates
	from the center
	capsule are selectable through their circles and boxes

	edges,chains are only selectable through there end points wether with or without physics
*/