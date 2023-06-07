#include "GameLogic.h"
namespace SnakeGame {

	void GameLogic::init(SquareBox::IMainGame* game_ptr_, int x_orign, int y_orign, int width_, int height_)
	{
		m_audio_engine.init();
		m_utilities.init();
		m_game_ptr = game_ptr_;

		world_width = width_;
		world_height = height_;
		orign_x = x_orign;
		orign_y = y_orign;

		snake = Snake((x_orign + world_width) * 0.5f, (y_orign + world_height) * 0.5f);

		waves.push_front(Wave((x_orign + world_width) * 0.5f, (y_orign + world_height) * 0.5f, 30, 1));
		waves.front().ampl = 400;

		music = SquareBox::AudioSystem::Music("Assets/Audio/ambience.wav");

		m_audio_engine.loadMusic(music);
		music.play(-1);

		bip_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bip", "Assets/Audio/bip.wav"));
		m_audio_engine.loadSoundBank(bip_sound_bank);

		boom_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("boom", "Assets/Audio/boom.wav"));
		m_audio_engine.loadSoundBank(boom_sound_bank);

		bouns_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bonus", "Assets/Audio/bonus.wav"));
		m_audio_engine.loadSoundBank(bouns_sound_bank);

		lose_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("lose", "Assets/Audio/lose.wav"));
		m_audio_engine.loadSoundBank(lose_sound_bank);

		srand(time(NULL));

		for (int i = orign_x; i < orign_x+world_width; ++i)
		{
			for (int j = orign_y; j < orign_y +world_height; ++j) { map.push_back(WavePoint(i, j)); }
		}

		Wave::surround = &map;

		score = 0;
		bonus = Bonus(rand() % (int)(orign_x + world_width * 0.8), rand() % (int)(orign_y+ world_height * 0.8));
	}


	void GameLogic::update(float delta_time_, bool fire_, Move snake_move_)
	{
		tick++;

		if (snake_move_ == Move::LEFT)
		{
			if (choosingVx != 1)
			{
				choosingVx = -1;
				choosingVy = 0;
			}
		}
		else if (snake_move_ == Move::RIGHT)
		{
			if (choosingVx != -1)
			{
				choosingVx = 1;
				choosingVy = 0;
			}
		}
		else if (snake_move_ == Move::UP)
		{
			if (choosingVy != -1)
			{
				choosingVx = 0;
				choosingVy = 1;
			}
		}
		else if (snake_move_ == Move::DOWN)
		{
			if (choosingVy != 1)
			{
				choosingVx = 0;
				choosingVy = -1;
			}
		}

		if (bonusClock.ElapsedMillis() >= bouns_shock_wave_rate)
		{
			waves.push_front(Wave(bonus.x, bonus.y, 10, 2));
			waves.front().ampl = 100;

			bip_sound_bank.play();

			bonusClock.Reset();
		}

		if (bombCount == 10 && !waiting && !danger)
		{
			waiting = true;
			dangerClock.Reset();
		}

		if (dangerClock.Elapsed() > 5 && !danger && waiting)
		{
			waiting = false;

			dangerCount++;
			bombCount += dangerCount;

			int simultaneous = dangerCount;
			if (simultaneous > 3) simultaneous = 3;

			for (int i(0); i < simultaneous; ++i)
			{
				bombs.push_back(Bomb(rand() % (int)(orign_x + world_width * 0.8), rand() % (int)(orign_y + world_height * 0.8)));
			}

			danger = true;
		}

		for (Bomb& bomb : bombs)
		{
			if (bomb.ready)
			{
				waves.push_back(Wave(bomb.x, bomb.y, 10, 10)); waves.back().ampl = 250;
				waves.push_back(Wave(bomb.x, bomb.y, 40, 1)); waves.back().ampl = 200;
				boom_sound_bank.play();
				bouns_sound_bank.play();
				if (!snake.isDead()) {
					const SnakeNode& head = snake.getHead();
					int x_head_boom = head.x - bomb.x;
					int y_head_boom = head.y - bomb.y;

					double distToSnakeHead = sqrt(x_head_boom * x_head_boom + y_head_boom * y_head_boom);

					if (distToSnakeHead < 15) {
						snake.addLife(-100 / (distToSnakeHead + 1));
					}
				}

				if (bombCount > 0)
				{
					bombs.push_back(Bomb(rand() % (int)(orign_x+ world_width * 0.8), rand() % (int)(orign_y + world_height * 0.8)));
					bombCount--;
				}
			}
		}

		bombs.remove_if([](Bomb& b) { return b.ready; });

		if (bombs.size() == 0) {
			danger = false;
		}

		if (tick % 4 == 0 && !snake.isDead())
		{
			SnakeNode head = snake.getHead();

			snake.setDirection(choosingVx, choosingVy);

			if (head.x + choosingVx == bonus.x && head.y + choosingVy == bonus.y)
			{
				if (!danger)
				{
					bombCount++;
				}
				else
				{
					score += dangerCount - 1; // the longer the game has been running , the  higher your gain
				}

				bouns_sound_bank.play();
				waves.push_front(Wave(bonus.x, bonus.y, 10, 4));

				snake.addNode(bonus.x, bonus.y);
				snake.addLife(5);

				deadBonus.push_front(bonus);

				bonus = Bonus(rand() % (int)(orign_x + world_width * 0.8), rand() % (int)(orign_y + world_height * 0.8));
				score++;
			}
			else
			{
				snake.progress(orign_x,orign_y,world_width, world_height);

				if (!(tick % 6))
				{
					waves.push_front(Wave(head.x, head.y, 11, 1));
					waves.front().ampl = 20;
				}
			}
		}
		else if (snake.isDead())
		{
			if (!gameOver) {
				lose_sound_bank.play();
			}

			for (const SnakeNode& sn : snake.getNodeList())
			{
				waves.push_front(Wave(sn.x, sn.y, 25, 2));
				waves.front().ampl = 20;
			}

			snake.kill();

			music.setVolume(0.6f);
			gameOver = true;
		}

		snake.update();

		for (Bomb& bomb : bombs)
		{
			bomb.update();
		}

		float magic_8 = 8.0f;
		for (Bonus& db : deadBonus) { if (db.radius < 50) db.radius += 2.5; }
		deadBonus.remove_if([](Bonus& b) { return b.radius >= 50; });

		for (WavePoint& wp : map)
		{
			wp.color = 25;
			wp.delta = 0;
		}

		for (Wave& wave : waves) { wave.apply(); }

		waves.remove_if([](Wave& v) { return v.done; });
	}

	void GameLogic::restart() 
	{

	}

	void GameLogic::dispose() {
		m_utilities.dispose();
	}

};