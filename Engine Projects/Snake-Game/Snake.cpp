#include "Snake.h"

#include <iostream>

Snake::Snake(signed arg_x, signed arg_y)
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
	if (_life <= 0) {
		_dead = true;
	}

	// appearance of the head
	for (SnakeNode& sn : _snake) {
		if (sn.node_size < sn.max_node_size) {
			sn.node_size += 0.5;
		}
	}

	// dead nodes being deleted
	for (SnakeNode& sn : _deadNodes) { 

		if (sn.node_size > 0) { 
		sn.node_size -= 0.5;
		}
	}
	_deadNodes.remove_if([](SnakeNode& sn) { return sn.node_size <= 0; });

}

void Snake::progress(const signed orign_x, const signed orign_y,const signed world_width,const signed world_height)
{
	SnakeNode head = _snake.front();
	_deadNodes.push_front(_snake.back());

	head.x += _vx;
	head.y += _vy;

	if (head.x < orign_x) {
		head.x = orign_x + world_width;
	}

	if (head.x > orign_x + world_width) {
		head.x = orign_x;
	}

	if (head.y < orign_y) {
		head.y = orign_y + world_height;
	}

	if (head.y > orign_y + world_height) {
		head.y = orign_y;
	}

	//check for collision
	for (SnakeNode& sn : _snake)
	{
		if (head.x == sn.x && head.y == sn.y) {
			_dead = true;
			_life = 0;
			return;
		}
	}

	_snake.push_front(head);
	_snake.pop_back();
}

void Snake::addNode(int x, int y)
{
	int node_size = 10;
	SnakeNode newNode = SnakeNode(x, y, node_size);
	newNode.node_size = node_size;
	_snake.push_front(newNode);
}

void Snake::checkAutoCollision()
{
	// head colliding with body kills the snake 
	SnakeNode& head = _snake.front();

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
		double size = sn.node_size / 2.0f;
		renderer.draw(glm::vec4(glm::vec2(x, y)-glm::vec2(size)*0.5f, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture_id_, 1.0f, SquareBox::RenderEngine::ColorRGBA8(r, g, b, 255));
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
		double size = sn.node_size / 2.0;
		renderer.draw(glm::vec4(glm::vec2(x, y) - glm::vec2(size) * 0.5f, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture_id_, 1.0f, SquareBox::RenderEngine::ColorRGBA8(r, g, b, 255));
		i++;
	}

	if (endLight)
		_timer.Reset();

}

const SnakeNode& Snake::getHead() const
{
	return _snake.front();
}
