#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "Snake.h"
#include "Wave.h"
#include "GameObjects.h"
#include "PlayerInputs.h"


class GameScreen: public SquareBox::IGameScreen
{

public:
	GameScreen() {};
	~GameScreen() {};


	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
	int m_previousScreenIndex = SCREEN_INDEX_NO_SCREEN;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::Camera::ParallelCamera m_camera;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;

	Snake snake;
	std::list<Bonus> deadBonus;
	std::list<Wave> waves;
	int world_width;
	int world_height;

	SquareBox::AudioSystem::AudioEngine m_audio_engine;

	SquareBox::Utilities m_utilities;

	long tick = 0;
	int bouns_shock_wave_rate = 2000; // in milliseconds
	int score = 0;

	int choosingVx = 1;
	int choosingVy = 0;
	int vx = 1;
	int vy = 0;
	std::list<Bomb> bombs;

	bool danger=false, waiting=false;
	int bombCount = 0;
	int dangerCount = 0;
	double yScore = 0;
	bool newScore = false;

	SquareBox::GWOM::Text scoreText, scoreTransition, warning;

	int heightScore = 40;

	bool gameOver = false;

	SquareBox::Timer speedClock, bonusClock, timer,dangerClock;

	PlayerInputs player_inputs;

	Bonus bonus;
	SquareBox::AudioSystem::Music music;
	SquareBox::AudioSystem::SoundBank bip_sound_bank;
	SquareBox::AudioSystem::SoundBank boom_sound_bank;
	SquareBox::AudioSystem::SoundBank bouns_sound_bank;
	SquareBox::AudioSystem::SoundBank lose_sound_bank;
	std::vector<WavePoint> map;
	double m_time = 0.0f;
};

