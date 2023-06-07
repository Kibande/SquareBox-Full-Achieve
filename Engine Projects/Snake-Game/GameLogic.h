#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "Snake.h"
#include "Wave.h"
#include "GameObjects.h"
namespace SnakeGame {
	enum class Move
	{
		NONE = -1,
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3
	};

	class GameLogic
	{
	public:

		void init(SquareBox::IMainGame* game_ptr_,int x_orign, int y_orign,int width_,int height_);
		void update(float delta_time_, bool fire_, Move snake_move_);
		void dispose();
		void restart();
		glm::vec2 getWorldDimensions() { return glm::vec2(world_width,world_height); }
		int getScore() { return score; }
		bool isGameOver() { return gameOver; }

		const std::vector<WavePoint> & getWorldMap() const { return map; }
		const std::list<Bomb>& getWorldBooms() const { return bombs; }
		const std::list<Bonus> & getWorldDeadBonus() const { return deadBonus; }
		Snake & getSnake() { return snake; };
		const Bonus & getActiveBouns() { return bonus; }
		bool isDanger() { return danger; };
		bool isWaiting() { return waiting; };
	private:

		Snake snake;
		std::list<Bonus> deadBonus;
		std::list<Wave> waves;
		int world_width;
		int world_height;
		int orign_x;
		int orign_y;
		SquareBox::AudioSystem::AudioEngine m_audio_engine;
		SquareBox::Utilities m_utilities;

		long tick = 0;
		int bouns_shock_wave_rate = 2000; // in milliseconds
		int score = 0;

		int choosingVx = 1;
		int choosingVy = 0;
		std::list<Bomb> bombs;

		bool danger = false, waiting = false;
		int bombCount = 0;
		int dangerCount = 0;
		double yScore = 0;

		bool gameOver = false;

		SquareBox::Timer speedClock, bonusClock, dangerClock;

		Bonus bonus;
		SquareBox::AudioSystem::Music music;
		SquareBox::AudioSystem::SoundBank bip_sound_bank;
		SquareBox::AudioSystem::SoundBank boom_sound_bank;
		SquareBox::AudioSystem::SoundBank bouns_sound_bank;
		SquareBox::AudioSystem::SoundBank lose_sound_bank;
		std::vector<WavePoint> map;

		SquareBox::IMainGame* m_game_ptr;

	};

}

