#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
#include "LayerIndicies.h"
#include "FireButtonTexture.h"
class GameHUDandGUI
{
public:
	void init(SquareBox::IMainGame * game_);
	void update(SquareBox::Camera::ParallelCamera & game_camera_,glm::vec2 player_position_, bool & is_player_moving_, glm::vec2 & player_direction_, bool & is_firing_amo_);
	void draw(SquareBox::RenderEngine::SpriteBatch & sprite_batch_, SquareBox::RenderEngine::SpriteFont & sprite_font_, const  std::vector<SquareBox::GWOM::Layer> & layers_);
	void dispose();
private:
	//texture that make up the GUI

	//button for the players controls
	SquareBox::GWOM::ClusterObject inner_direction_button;
	SquareBox::GWOM::ClusterObject outer_direction_button;
	SquareBox::GWOM::ClusterObject fire_button;
	SquareBox::GWOM::ClusterObject move_button;

	SquareBox::GWOM::ClusterObject m_avaliable_gui_space_object;
	SquareBox::Utilities  m_utilities;
	SquareBox::IMainGame * m_game_ptr;
	glm::vec4 m_gui_space_dest_rect;
	SquareBox::IShape* m_current_shape_ptr = nullptr;// a pointer to the current world shape 
	float m_camera_scale = 0.0f;
	FireButtonTexture m_fire_button_texture;
	FireButtonTexture m_move_button_texture;

};

