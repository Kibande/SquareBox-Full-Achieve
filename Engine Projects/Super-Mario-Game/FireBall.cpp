#include "FireBall.h"

#include "SoundController.h"
#include "CollisionHandling.h"
#include "SDL_Utility.h"


std::array<SDL_Surface*, 4> FireBall::fireBallImages;

void FireBall::changeModelIndex()
{
	++changeModelCounter;
	if (changeModelCounter % 8 == 0) {
		++modelIndex;
		if (modelIndex == 4) {
			modelIndex = 0;
		}
	}
}

void FireBall::moveVertically(World &world)
{
	int alignment = computeHorizontalAlignment(movement.getDirection(), movement.getSpeed(), *this, world);
	int distance = movement.getSpeed() - alignment;
	if (movement.getDirection() == Direction::Left) {
		distance *= -1;
	}
	position.setX(position.getX() + distance);

	if (alignment > 0) {
		stop = true;
		SoundController::playBlockHittedEffect();
	}
}

void FireBall::moveHorizontally(World &world)
{
	int alignment = computeVerticalAlignment(movement.getVerticalDirection(), movement.getVerticalSpeed(), 
		*this, world);
	int verticalDistance = movement.getVerticalSpeed() - alignment;
	if (movement.getVerticalDirection() == Direction::Up) {
		verticalDistance *= -1;
	}
	position.setY(position.getY() + verticalDistance);

	changeModelIndex();

	if (movement.getVerticalDirection() == Direction::Up) {
		++stepsUp;
	}

	if (alignment > 0) {
		movement.setVerticalDirection(
			movement.getVerticalDirection() == Direction::Down ? Direction::Up : Direction::Down);

		stepsUp = 0;
	}
	else if (stepsUp % 20 == 0 && movement.getVerticalDirection() == Direction::Up) {
		movement.setVerticalDirection(Direction::Down);
		stepsUp = 0;
	}
}

FireBall::FireBall(Position position, Direction direction)
{
	this->position = position;
	changeModelCounter = 0;
	stepsUp = 0;
	modelIndex = 0;
	stop = false;
	movement = Movement(3, 2, direction, Direction::Down);
	size = Size(16, 16);
}

void FireBall::loadFireBallImages(SDL_Surface* display)
{
	for (std::size_t i = 0; i < fireBallImages.size(); ++i) {
		std::string filename = "./img/other_imgs/fireball";
		filename += std::to_string(i + 1);
		filename += ".png";
		fireBallImages[i] = loadPNG(filename, display);
	}
}

bool FireBall::shouldBeRemoved() const
{
	return stop;
}

void FireBall::move(World &world)
{
	if (!stop) {
		moveVertically(world);
		moveHorizontally(world);
	}
}

void FireBall::draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const
{
	if (isWithinRangeOfCamera(beginningOfCamera, endOfCamera)) {
		SDL_Surface* fireballImg = fireBallImages[modelIndex];
		drawSurface(display, fireballImg, position.getX() - beginningOfCamera, position.getY());
	}
}