#include "Coin.h"

#include "SoundController.h"
#include "SDL_Utility.h"
#include "Player.h"
#include "World.h"
#include "LayoutStyle.h"


std::array<SDL_Surface*, 8> Coin::coinImages;

bool Coin::coinImage = true;

int Coin::computeBaseIndex() const
{
	if (World::LAYOUT_STYLE == LayoutStyle::OpenWorld || World::LAYOUT_STYLE == LayoutStyle::CustomSummer) {
		return 0;
	}
	else {
		return 2 * static_cast<int>(World::LAYOUT_STYLE);
	}
}

int Coin::computeImageIndex() const
{
	int baseIndex = computeBaseIndex();
	return baseIndex + coinImage;
}

Coin::Coin(Position position)
{
	this->position = position;
	size = Size(20, 28);
}

bool Coin::isCoin() const
{
	return true;
}

int Coin::getPointsForCollecting() const
{
	return 200;
}

void Coin::loadCoinImages(SDL_Surface* display)
{
	for (std::size_t i = 0; i < coinImages.size(); ++i) {
		std::string filename = "./img/other_imgs/coin";
		filename += std::to_string(i + 1);
		filename += ".png";
		coinImages[i] = loadPNG(filename, display);
	}
}

void Coin::resetCoinImage()
{
	Coin::coinImage = true;
}

void Coin::changeCoinImage()
{
	Coin::coinImage = !Coin::coinImage;
}

void Coin::giveBonus(Player &player)
{
	player.incrementCoins();
	SoundController::playCoinCollectedEffect();
}

void Coin::move(World &world) {}

void Coin::draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const
{
	if (isWithinRangeOfCamera(beginningOfCamera, endOfCamera)) {
		SDL_Surface* coinImg = coinImages[computeImageIndex()];
		drawSurface(display, coinImg, position.getX() - beginningOfCamera, position.getY());
	}
}