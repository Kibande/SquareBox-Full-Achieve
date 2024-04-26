#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>

class First_Screen :
	public SquareBox::IGameScreen
{
public:
	First_Screen();
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;
private:
	//we need a camera
	SquareBox::Camera::ParallelCamera m_camera;

	// a layer object would come with the camera inbuilt


	//our texture Program
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	//we need a spriteBatch
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;


	//Sprite font 
	SquareBox::RenderEngine::SpriteFont m_sprite_font;

	SquareBox::AssetManager::GLTexture m_texture;
};
