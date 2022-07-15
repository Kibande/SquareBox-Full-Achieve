#pragma once
#include "ScreenIndices.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <vector>
#include <string>
#include <sstream>

class First_Screen : public SquareBox::IGameScreen
{
public:
	First_Screen();
	~First_Screen();
private:
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;

	virtual int getPreviousScreenIndex() const override;

	virtual void build() override;

	virtual void onEntry() override;

	virtual void update(const float & deltaTime_) override;

	virtual void draw() override;

	virtual void onExit() override;

	virtual void destroy() override;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	//the layering system
	std::vector<SquareBox::GWOM::Layer> m_layers;
	std::vector<SquareBox::GWOM::WorldCluster> m_vec_of_world_clusters;
	int m_active_layer_index = 0;
	int m_next_screen_index = SECOND_SCREEN_INDEX;
	int m_previous_screen_index = SCREEN_INDEX_NO_SCREEN;
	SquareBox::AI::Automation m_automation;
	SquareBox::AnimationSystem::AnimationCreator m_animation_creator;
	std::string m_animation_script = "Assets/Animations/animations_file.lua";
	std::string m_automation_script = "Assets/Animations/automation_file.lua";
	SquareBox::Utilities m_utilities;
	SquareBox::IShape * m_shape_pointer = nullptr;
	int m_num_fingers_on_screen = 0;

};
