#ifndef CrushedCreature_H
#define CrushedCreature_H

#include <array>
#include <chrono>
#include "TemporaryObject.h"

struct SDL_Surface;


class CrushedCreature : public TemporaryObject
{
private:
	static std::array<SDL_Surface*, 4> crushedCreatureImages;
	std::chrono::steady_clock::time_point creationTime;
	int computeImageIndex() const override;

public:
	CrushedCreature(Position position);
	static void loadCrushedCreatureImages(SDL_Surface* display);
	bool shouldBeRemoved() const override;
	void draw(SDL_Surface* display, int beginningOfCamera, int endOfCamera) const override;
};

#endif //CrushedCreature_H