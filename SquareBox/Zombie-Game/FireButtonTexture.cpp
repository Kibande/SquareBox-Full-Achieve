#include "FireButtonTexture.h"

void FireButtonTexture::init(float dpi_)
{
	m_button_gloss = SquareBox::AssetManager::TextureManager::getTextureByName("fire button accessories.png", dpi_);
	m_button_tint = SquareBox::AssetManager::TextureManager::getTextureByName("fire button gradient.png", dpi_);
	m_button_shadow_texture = SquareBox::AssetManager::TextureManager::getTextureByName("fire button shadow.png", dpi_);
	m_button_color_texture = SquareBox::AssetManager::TextureManager::getTextureByName("fire button color.png", dpi_);
}

void FireButtonTexture::draw(glm::vec2 position_, float radius_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
	const glm::vec2   boxDims = glm::vec2(radius_ * 2, radius_ * 2);
	const glm::vec2  boxOrigin = glm::vec2(position_) - boxDims * 0.5f;
	glm::vec4 uv_rect(0.0f, 0.0f, 1.0f, 1.0f);
	SquareBox::RenderEngine::ColorRGBA8 white_color(SquareBox::Color::white);
	SquareBox::RenderEngine::ColorRGBA8 button_color;
	if (m_is_firing) {
		button_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::yellow);
	}
	else {
		button_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green);
	}

	(SquareBox::Color::white);
	sprite_batch_.draw(glm::vec4(boxOrigin, radius_ * 2, radius_ * 2), uv_rect, m_button_shadow_texture.id, 0.0f, white_color);
	sprite_batch_.draw(glm::vec4(boxOrigin, radius_ * 2, radius_ * 2), uv_rect, m_button_color_texture.id, 0.0f, button_color);
	sprite_batch_.draw(glm::vec4(boxOrigin, radius_ * 2, radius_ * 2), uv_rect, m_button_tint.id, 0.0f, white_color);
	sprite_batch_.draw(glm::vec4(boxOrigin, radius_ * 2, radius_ * 2), uv_rect, m_button_gloss.id, 0.0f, white_color);
}
