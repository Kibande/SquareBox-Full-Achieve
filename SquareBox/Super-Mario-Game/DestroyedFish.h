#ifndef DestroyedFish_H
#define DestroyedFish_H

#include <array>
#include "TemporaryObject.h"
#include "Direction.h"

struct SDL_Surface;


class DestroyedFish : public TemporaryObject
{
private:
	static std::array<SDL_Surface*, 2> destroyedFishImages;
	Direction direction;
	int computeImageIndex() const override;

public:
	DestroyedFish(Position position, Direction direction);
	static void loadDestroyedFishImages(SDL_Surface* display);
	void slide() override;
	void draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const override;
};

#endif //DestroyedFish_H