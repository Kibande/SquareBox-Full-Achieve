#include "BoxGUIElement.h"
#include <MathLib/MathLib.h>
void SquareBox::GUI::BoxGUIElement::drawTexture(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, glm::vec4& visiable_dest_rect_)
{

	if (!gui_element_.is_hidden && gui_element_.is_alive) {
	glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
	glm::vec2 dimensions = glm::vec2(gui_element_.width_ratio * visiable_dest_rect_.z, gui_element_.height_ratio * visiable_dest_rect_.w);
		//A box
		glm::vec2  boxOrigin = glm::vec2(position) - dimensions * 0.5f;
		/* the box padding of 0.01 has been removed from here
		 for boxes  and capsules, we shall just be submitting smaller dimensions to the physics engine
		*/
		const SquareBox::GWOM::TextureInfo& texture_info = gui_element_.textures[static_cast<int>(gui_element_.state)];

		sprite_batch_.draw(glm::vec4(boxOrigin, dimensions), texture_info.uv_rect,texture_info.texture_id, 0.0f, SquareBox::RenderEngine::ColorRGBA8((texture_info.color.x), (texture_info.color.y), (texture_info.color.z), (texture_info.color.w) * (0.01 * texture_info.opacity)), gui_element_.angle);
	}
}

void SquareBox::GUI::BoxGUIElement::drawText(const SquareBox::GWOM::GUIElement& gui_element_,SquareBox::RenderEngine::SpriteFont & sprite_font_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_, float camera_scale_, glm::vec4& visiable_dest_rect_)
{
	glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
	glm::vec2 dimensions = glm::vec2(gui_element_.width_ratio * visiable_dest_rect_.z, gui_element_.height_ratio * visiable_dest_rect_.w);
	//A box
	glm::vec2  boxOrigin = glm::vec2(position) - dimensions * 0.5f;

	glm::vec4 box_dest_rect(boxOrigin,dimensions);
	auto& font_info = gui_element_.fonts[static_cast<int>(gui_element_.state)];
	glm::vec2 text_dimentions = sprite_font_.measure(font_info.text);
	float text_height = (text_dimentions.y / camera_scale_);
	float text_width = (text_dimentions.x / camera_scale_);

	float text_to_cell_height = box_dest_rect.w / text_height;

	auto text_position = glm::vec2(box_dest_rect.x, box_dest_rect.y);



	if (font_info.justification == SquareBox::JustificationEnum::LEFT) {
		//the x is in place now the y
	}
	else if (font_info.justification == SquareBox::JustificationEnum::MIDDLE) {
		text_position.x += box_dest_rect.z * 0.5f;
	}
	else if (font_info.justification == SquareBox::JustificationEnum::RIGHT) {
		text_position.x += box_dest_rect.z;
	}

	float before_scale = (1 / camera_scale_) * text_to_cell_height;
	float after_scale = (1 / camera_scale_) * text_to_cell_height * font_info.text_to_box_height_scale;

	float difference_in_scales = before_scale - after_scale;
	if (difference_in_scales > 0 && !SquareBox::MathLib::Float::areFloatsEqual(difference_in_scales, 0)) {
		// the scale is making the text smaller the the space its supposed to occupy
		//so shit it so that it appears to be in the center but shitfting it upp by half the difference in propotional
		//cell size
		text_position.y += (difference_in_scales * box_dest_rect.w) * 0.5f;
	}
	else {
		//ignore making the text bigger that the bounding box
		after_scale = before_scale;
	}

	std::string orignal_text = std::string(font_info.text);
	std::string string_text = orignal_text;

	//handling texts that are longer than the cell
	if (text_width > box_dest_rect.z) {
		//we also accounted for the horizontal scaling too.
		float width_correction_ratio = box_dest_rect.z / text_width;

		string_text = orignal_text.substr(0, orignal_text.length() * (width_correction_ratio + (width_correction_ratio * (1 - after_scale))));
	}

	sprite_font_.draw(sprite_batch_, string_text.c_str(), text_position, glm::vec2(after_scale), 1.0f, SquareBox::RenderEngine::ColorRGBA8((font_info.color.x), (font_info.color.y), (font_info.color.z), (font_info.color.w) * (0.01 * font_info.opacity)), font_info.justification);
}

bool SquareBox::GUI::BoxGUIElement::containtsPoint(const SquareBox::GWOM::GUIElement& gui_element_, const glm::vec2& coordinates_, glm::vec4& visiable_dest_rect_)
{
	if (gui_element_.is_alive) {
			glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		glm::vec2 dimensions = glm::vec2(gui_element_.width_ratio * visiable_dest_rect_.z, gui_element_.height_ratio * visiable_dest_rect_.w);
		//A box
		const float x_position = position.x;
		const float y_position = position.y;
		const float width = dimensions.x;
		const float height = dimensions.y;

		const float x_coordinate = coordinates_.x;
		const float y_coordinate = coordinates_.y;

		const float scaled_x = width * 0.5;
		const float scaled_y = height * 0.5;


		const float orign_x = x_position - scaled_x;
		const float orign_y = y_position - scaled_y;

		//when drawing we are going to add 0.01 padding around the box to catter for physics boxes
		//this should be done for all polygons
		const float  padding = 0.01f;
		const float  doublePadding = 0.02f;

		const float  x1 = orign_x - padding;
		const float  y1 = orign_y - padding;
		const float  x2 = x1 + width + doublePadding;
		const float  y2 = y1 + +height + doublePadding;

		return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);

	}
	return false;
}

void SquareBox::GUI::BoxGUIElement::debugDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const SquareBox::RenderEngine::ColorRGBA8 borderColor_, glm::vec4& visiable_dest_rect_)
{
	if (gui_element_.is_alive) {
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);
		glm::vec2 dimensions = glm::vec2(gui_element_.width_ratio * visiable_dest_rect_.z, gui_element_.height_ratio * visiable_dest_rect_.w);
		//A box
		glm::vec2  boxOrigin = glm::vec2(position) - dimensions * 0.5f;
		debug_render_.drawBox(glm::vec4(boxOrigin, dimensions), borderColor_, gui_element_.angle);
	}
}

void SquareBox::GUI::BoxGUIElement::traceDraw(const SquareBox::GWOM::GUIElement& gui_element_, SquareBox::RenderEngine::DebugRenderer& debug_render_, const float cameraScale_, const SquareBox::RenderEngine::ColorRGBA8& borderColor_, glm::vec4& visiable_dest_rect_)
{
	if (!gui_element_.is_alive) {
		glm::vec2 dimensions = glm::vec2(gui_element_.width_ratio * visiable_dest_rect_.z, gui_element_.height_ratio * visiable_dest_rect_.w);
		glm::vec2 position = glm::vec2(visiable_dest_rect_.x, visiable_dest_rect_.y) + glm::vec2(gui_element_.location_ratio.x * visiable_dest_rect_.z, gui_element_.location_ratio.y * visiable_dest_rect_.w);

		const glm::vec2& border_corner = position - dimensions * 0.5f;
		debug_render_.drawBox(glm::vec4(border_corner, dimensions), borderColor_, gui_element_.angle);
	}
}
