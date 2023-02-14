#include "Snake.h"

#include <iostream>

Snake::Snake(int arg_x, int arg_y)
{
	addNode(arg_x, arg_y);
	_dead = false;
	_life = 100.0; 
}

void Snake::addLife(double life)
{
	_life += life;
	if (_life < 0)
		_life = 0;
	else if (_life > 100)
		_life = 100;
}

void Snake::update()
{
	if (_life <= 0) _dead = true;

	// appearance of the head
	for (SnakeNode& sn : _snake) { if (sn.c < sn.maxC) sn.c += 0.5; }

	// dead nodes being deleted
	for (SnakeNode& sn : _deadNodes) { if (sn.c > 0) sn.c -= 0.5; }
	_deadNodes.remove_if([](SnakeNode& sn) { return sn.c <= 0; });

}

void Snake::progress(const int world_width,const int world_height)
{
	SnakeNode head = _snake.front();
	_deadNodes.push_front(_snake.back());

	head.x += _vx;
	head.y += _vy;
	head.x %= world_width;
	head.y %= world_height;

	if (head.x < 0) {
		head.x += world_width;
	}

	if (head.y < 0) { 
		head.y += world_height; 
	}

	checkAutoCollision();

	_snake.push_front(head);
	_snake.pop_back();
}

void Snake::addNode(int x, int y)
{
	SnakeNode newNode = SnakeNode(x, y, 10);
	newNode.c = 10;
	_snake.push_front(newNode);
}

void Snake::checkAutoCollision()
{
	// life colliding with body kill the snake 
	SnakeNode& head = _snake.front();
	int n_collision = 0;
	for (SnakeNode& sn : _snake)
	{
		if (head.x == sn.x && head.y == sn.y) { n_collision++; }
	}

	if (n_collision > 1)
	{
		_dead = true;
		_life = 0;
	}
}

void Snake::draw(SquareBox::RenderEngine::SpriteBatch& renderer,float heightScore,int texture_id_)
{
	double time = _timer.Elapsed();
	double lifeRatio = 1.0 - _life / 100.0;

	int i = 0;
	bool endLight = true;
	for (const SnakeNode& sn : _snake)
	{
		double phi = time * lifeRatio * 20 - i / 10.0;
		double c = 0;
		if (phi >= 0 && phi < 3.14159)
		{
			c = 150 * sin(phi);
			endLight = false;
		}

		double r = lifeRatio * 255 + c; if (r > 255) r = 255;
		double g = 255 - lifeRatio * 255 + c; if (g > 255) g = 255;
		double b = c; if (b > 255) b = 255;

		int x = sn.x * 10 + 5;
		int y = sn.y * 10 + heightScore + 5;
		double s = sn.c / 2.0f;
		renderer.draw(glm::vec4(glm::vec2(x, y), glm::vec2(s)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture_id_, 1.0f, SquareBox::RenderEngine::ColorRGBA8(r, g, b, 255));
		i++;
	}

	int j = i;
	i = 0;
	for (const SnakeNode& sn : _deadNodes)
	{
		double phi = time * lifeRatio * 20 - j / 10.0;
		double c = 0;
		if (phi >= 0 && phi < 3.14159)
		{
			c = 255 * sin(phi);
			endLight = false;
		}

		double r = lifeRatio * 255 + c; if (r > 255) r = 255;
		double g = 255 - lifeRatio * 255 + c; if (g > 255) g = 255;
		double b = c; if (b > 255) b = 255;

		int x = sn.x * 10 + 5;
		int y = sn.y * 10 + heightScore + 5;
		double s = sn.c / 2.0;
		renderer.draw(glm::vec4(glm::vec2(x, y), glm::vec2(s)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture_id_, 1.0f, SquareBox::RenderEngine::ColorRGBA8(r, g, b, 255));
		i++;
	}

	if (endLight)
		_timer.Reset();

}

const SnakeNode& Snake::getHead() const
{
	return _snake.front();
}
