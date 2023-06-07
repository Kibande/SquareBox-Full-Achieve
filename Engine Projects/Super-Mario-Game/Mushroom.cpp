#include "Mushroom.h"

#include "SoundController.h"
#include "CollisionHandling.h"
#include "SDL_Utility.h"
#include "Player.h"
#include "World.h"
#include "LayoutStyle.h"


std::array<SDL_Surface*, 3> Mushroom::mushroomImages;

void Mushroom::moveUp(World &world)
{
	int alignment = computeVerticalAlignment(Direction::Down, movement.getVerticalSpeed(), *this, world);
	int verticalDistance = movement.getVerticalSpeed() - alignment;
	position.setY(position.getY() - verticalDistance);
}

void Mushroom::moveHorizontally(World &world)
{
	int alignment = computeHorizontalAlignment(movement.getDirection(), movement.getSpeed(), *this, world);
	int distance = movement.getSpeed() - alignment;
	if (movement.getDirection() == Direction::Left) {
		distance *= -1;
	}
	position.setX(position.getX() + distance);

	if (alignment > 0) {
		movement.setDirection(movement.getDirection() == Direction::Right ? Direction::Left : Direction::Right);
	}
}

void Mushroom::moveDiagonally(World &world)
{
	int alignment = computeVerticalAlignment(Direction::Down, movement.getVerticalSpeed(), *this, world);
	int verticalDistance = movement.getVerticalSpeed() - alignment;
	position.setY(position.getY() + verticalDistance);

	alignment = computeHorizontalAlignment(movement.getDirection(), movement.getSpeed(), *this, world);
	int distance = movement.getSpeed() - alignment;
	if (movement.getDirection() == Direction::Left) {
		distance *= -1;
	}
	position.setX(position.getX() + distance);
}

int Mushroom::computeImageIndex() const
{
	if (oneUp) {
		return 1 + (World::LAYOUT_STYLE == LayoutStyle::Underground);
	}

	return 0;
}

Mushroom::Mushroom(Position position, bool oneUp)
{
	this->position = position;
	this->oneUp = oneUp;
	moveCounter = 0;
	stepsUp = 0;
	growCounter = 96;
	movement = Movement(1, 2, Direction::Right, Direction::None);
	size = Size(32, 32);
}

void Mushroom::loadMushroomImages(SDL_Surface* display)
{
	mushroomImages[0] = loadPNG("./img/bonus_imgs/mushroom_red.png", display);
	mushroomImages[1] = loadPNG("./img/bonus_imgs/mushroom_green.png", display);
	mushroomImages[2] = loadPNG("./img/bonus_imgs/mushroom_brown_blue.png", display);
}

int Mushroom::getPointsForCollecting() const
{
	return (oneUp ? 0 : 1000);
}

void Mushroom::draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const
{
	if (isWithinRangeOfCamera(beginningOfCamera, endOfCamera)) {
		SDL_Surface* mushroomImg = mushroomImages[computeImageIndex()];
		drawSurface(display, mushroomImg, position.getX() - beginningOfCamera, position.getY());
	}
}

void Mushroom::giveBonus(Player &player)
{
	if (!oneUp) {
		if (player.isSmall()) {
			player.setCurrentAnimation(PlayerAnimation::Growing);
		}
		SoundController::playBonusCollectedEffect();
	}
	else {
		player.incrementLives();
		SoundController::playNewLiveAddedEffect();
	}
}

void Mushroom::knockUp()
{
	position.setY(position.getY() - 3);
	if (stepsUp == 0) {
		stepsUp = 30;
	}
}

void Mushroom::move(World &world)
{
	if (growCounter) {
		grow();
	}
	else if (moveCounter & 1) {
		if (stepsUp > 0) {
			moveUp(world);
			--stepsUp;
			moveHorizontally(world);
		}
		else {
			if (isCharacterStandingOnSomething(*this, world)) {
				moveHorizontally(world);
			}
			else {
				moveDiagonally(world);
			}
		}
	}
	++moveCounter;
}