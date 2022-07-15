#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <glm/glm.hpp>
class FireButtonTexture
{

public:
	void init(float dpi_,SquareBox::RenderEngine::ColorRGBA8 default_color_,SquareBox::RenderEngine::ColorRGBA8 active_color_);
	void setIsActive(bool status_) { m_is_active = status_;};
	void draw(glm::vec2 position_, float radius_ , SquareBox::RenderEngine::SpriteBatch & sprite_batch_);
	void dispose() {};
private:
	SquareBox::AssetManager::GLTexture m_button_tint;
	SquareBox::AssetManager::GLTexture m_button_shadow_texture;
	SquareBox::AssetManager::GLTexture m_button_color_texture;
	SquareBox::AssetManager::GLTexture m_button_gloss;
	bool m_is_active=false;
	SquareBox::RenderEngine::ColorRGBA8 m_default_color;
	SquareBox::RenderEngine::ColorRGBA8 m_active_color;
};

