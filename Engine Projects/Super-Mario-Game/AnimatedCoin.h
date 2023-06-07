#ifndef AnimatedCoin_H
#define AnimatedCoin_H

#include <array>
#include "TemporaryObject.h"

struct SDL_Surface;


class AnimatedCoin : public TemporaryObject 
{
private:
	static std::array<SDL_Surface*, 7> animatedCoinImages;
	int imageIndex;
	int computeImageIndex() const override;

public:
	AnimatedCoin(Position position);
	static void loadAnimatedCoinImages(SDL_Surface* display);
	bool shouldBeRemoved() const override;
	bool isAnimatedCoin() const override;
	void slide() override;
	void draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const override;
};

#endif //AnimatedCoin_H