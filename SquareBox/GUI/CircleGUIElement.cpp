#include "CircleGUIElement.h"

void SquareBox::GUI::CircleGUIElement::drawTexture(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, glm::vec4& visiable_dest_rect_)
{
	if (!gui_element_.is_hidden && gui_element_.is_alive) {
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		float radius =  gui_element_.radius_ratio*std::sqrt((visiable_dest_rect_.z* visiable_dest_rect_.z) + (visiable_dest_rect_.w* visiable_dest_rect_.w));
		const glm::vec2   boxDims = glm::vec2(radius * 2, radius * 2);
		const glm::vec2  boxOrigin = glm::vec2(position) - boxDims * 0.5f;
		const SquareBox::GWOM::TextureInfo& texture_info = gui_element_.textures[static_cast<int>(gui_element_.state)];
		sprite_batch_.draw(glm::vec4(boxOrigin, radius * 2, radius * 2), texture_info.uv_rect, texture_info.texture_id, 0.0f, SquareBox::RenderEngine::ColorRGBA8((texture_info.color.x), (texture_info.color.y), (texture_info.color.z), (texture_info.color.w) * (0.01 * texture_info.opacity)), gui_element_.angle);
	}
}

void SquareBox::GUI::CircleGUIElement::drawText(const SquareBox::GWOM::GUIElement& gui_element_,SquareBox::RenderEngine::SpriteFont & sprite_font_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, float camera_scale_, glm::vec4& visiable_dest_rect_)
{

}

bool SquareBox::GUI::CircleGUIElement::containtsPoint(const SquareBox::GWOM::GUIElement& gui_element_, const glm::vec2& coordinates_, glm::vec4& visiable_dest_rect_)
{
	if (gui_element_.is_alive) {
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		float radius =  gui_element_.radius_ratio*std::sqrt((visiable_dest_rect_.z* visiable_dest_rect_.z) + (visiable_dest_rect_.w* visiable_dest_rect_.w));
		if (gui_element_.is_hidden) { return false; }
		return ((glm::length(glm::vec2(position) - coordinates_) <= radius));
	}
	return false;
}

void SquareBox::GUI::CircleGUIElement::debugDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_, glm::vec4& visiable_dest_rect_)
{
	if (gui_element_.is_alive) {
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		float radius =  gui_element_.radius_ratio*std::sqrt((visiable_dest_rect_.z* visiable_dest_rect_.z) + (visiable_dest_rect_.w* visiable_dest_rect_.w));
		debug_render_.drawCircle(position, borderColor_, radius);
	}
}

void SquareBox::GUI::CircleGUIElement::traceDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_,const float cameraScale_, const SquareBox::RenderEngine::ColorRGBA8& borderColor_, glm::vec4& visiable_dest_rect_)
{
	if (!gui_element_.is_alive) {
		float radius =  gui_element_.radius_ratio*std::sqrt((visiable_dest_rect_.z* visiable_dest_rect_.z) + (visiable_dest_rect_.w* visiable_dest_rect_.w));
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y)+ glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		debug_render_.drawCircle(position, borderColor_, radius);
	}
}
