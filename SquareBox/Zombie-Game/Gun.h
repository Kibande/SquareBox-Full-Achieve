#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <random>
#include <ctime>
#include "LayerIndicies.h"
#include "Bullet.h".h
class Gun
{
public:
	Gun() {};
	void init(std::string name_,int fire_rate_,int bullets_per_shoot_,float spread_,SquareBox::AudioSystem::SoundBank sound_bank_,Bullet * bullet_);
	void update(float delta_time_, bool should_fire_, const glm::vec2& position_, const glm::vec2& direction_, std::vector<SquareBox::GWOM::Layer> & layers_,SquareBox::GWOM::Grid * collision_grid_ptr_);
	void dispose();
	~Gun() {};
private:
	void fire(const glm::vec2& position_, const glm::vec2& direction_, std::vector<SquareBox::GWOM::Layer> & layers_,SquareBox::GWOM::Grid * collision_grid_ptr_);
	std::string m_name;
	int m_bullets_per_shoot=0;
	int m_fire_rate=0;
	float m_frame_counter=0.0f;
	float m_spread=0.0f;
	SquareBox::AudioSystem::SoundBank m_sound_bank;
	Bullet* m_bullet_ptr =nullptr;
	SquareBox::Utilities m_utilities;
};

