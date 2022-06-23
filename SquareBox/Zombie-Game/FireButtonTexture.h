#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <glm/glm.hpp>
class FireButtonTexture
{

public:
	void init(float dpi_);
	void setIsFiring(bool status_) { m_is_firing = status_;};
	void draw(glm::vec2 position_, float radius_ , SquareBox::RenderEngine::SpriteBatch & sprite_batch_);
	void dispose() {};
private:
	SquareBox::AssetManager::GLTexture m_button_tint;
	SquareBox::AssetManager::GLTexture m_button_shadow_texture;
	SquareBox::AssetManager::GLTexture m_button_color_texture;
	SquareBox::AssetManager::GLTexture m_button_gloss;
	bool m_is_firing;

};

