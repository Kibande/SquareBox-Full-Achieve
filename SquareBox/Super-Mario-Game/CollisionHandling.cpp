#include "CollisionHandling.h"

#include "SoundController.h"
#include "BonusObject.h"
#include "Player.h"
#include "World.h"
#include "Shell.h"
#include "FireMissle.h"
#include "Plant.h"
#include "UtilityFunctions.h"
#include "WorldInteractionFunctions.h"


bool isCharacterHittingObject(const WorldObject &object, const WorldObject &block, Direction direction, int distance)
{
	if (direction == Direction::Right && object.getX() < block.getX()
		&& object.getX() + distance + object.getWidth() / 2 >= block.getX() - block.getWidth() / 2) {

		return true;
	}
	else if (direction == Direction::Left && object.getX() > block.getX()
		&& object.getX() - distance - object.getWidth() / 2 <= block.getX() + block.getWidth() / 2) {

		return true;
	}
	else if (direction == Direction::Up && object.getY() > block.getY()
		&& object.getY() - distance - object.getHeight() / 2 <= block.getY() + block.getHeight() / 2) {

		return true;
	}
	else if (direction == Direction::Down && object.getY() < block.getY()
		&& object.getY() + distance + object.getHeight() / 2 >= block.getY() - block.getHeight() / 2) {

		return true;
	}

	return false;
}

bool isCharacterStandingOnSomething(const WorldObject &object, const World &world)
{
	std::vector<Block> blocks = world.getBlocks();
	for (auto &block : blocks) {
		if (isElementDirectlyAboveObject(object, block) && areAtTheSameWidth(object, block) 
			&& !block.isInvisible()) {

			return true;
		}
	}

	std::vector<Platform> platforms = world.getPlatforms();
	for (auto &platform : platforms) {
		if (isElementDirectlyAboveObject(object, platform) && areAtTheSameWidth(object, platform)) {
			return true;
		}
	}

	return false;
}

bool isNpcStandingOnBlock(const IndependentMovingObject &npc, const Block &block)
{
	if (isNpcCloseAboveBlock(npc, block) && areAtTheSameWidth(npc, block)) {
		return true;
	}

	return false;
}

bool isBonusStandingOnBlock(const BonusObject &bonus, const Block &block)
{
	return (isElementDirectlyAboveObject(bonus, block) && areAtTheSameWidth(bonus, block));
}

bool isBlockBlockedByAnother(const Block &block, const World &world)
{
	std::vector<Block> blocks = world.getBlocks();
	for (auto &element : blocks) {
		if (block.getX() == element.getX() && block.getY() == (element.getY() + block.getHeight())) {
			return true;
		}
	}

	return false;
}

bool isPlayerCloseToFireRocket(const FireMissle &fireRocket, const World &world)
{
	const Player& player = world.getPlayer();

	if (abs(player.getX() - fireRocket.getX()) < 350) {
		return true;
	}

	return false;
}

bool isPlayerCloseToPlant(const Plant &plant, const World &world)
{
	const Player& player = world.getPlayer();
	int yDifference = plant.getY() - player.getY();

	if (abs(player.getX() - plant.getX()) < 40 && (yDifference > 30 && yDifference < 60)) {
		return true;
	}

	return false;
}

bool isPlayerAheadOfNpc(const IndependentMovingObject &npc, const World &world)
{
	const Player& player = world.getPlayer();

	if (player.getX() > npc.getX()) {
		return true;
	}

	return false;
}

bool isPlayerStandingOnThisPlatform(const Player &player, const Platform &platform)
{
	if (isElementDirectlyAboveObject(player, platform) && areAtTheSameWidth(player, platform)) {
		return true;
	}

	return false;
}

bool isPlayerJumpingOnNpc(const Player &player, const IndependentMovingObject &npc)
{
	return ((npc.getY() - player.getY() > 25) && player.isNotJumpingUp());
}

void handleNpcDestroying(IndependentMovingObject &npc, World &world, Player &player, Direction direction)
{
	npc.destroy(world, direction);
	addTextAndPoints(player, world, npc.getPointsForDestroying(), Position(npc.getX(), npc.getY() - 15));
}

void handleNpcDeleting(World &world, int index, bool bossFlag)
{
	world.deleteNpc(index);
	SoundController::playEnemyDestroyedEffect(bossFlag);
}

void handleNpcHpReducing(IndependentMovingObject &npc, World &world, Player &player, Direction direction, int index)
{
	npc.decrementHealthPoints();
	if (npc.getHealthPoints() == 0) {
		handleNpcDestroying(npc, world, player, direction);
		handleNpcDeleting(world, index, (npc.getPointsForDestroying() == 5000));
	}
}

void handleFireBallDeleting(const FireBall &fireball, World &world, int index)
{
	int alignment = determineShift(fireball.getDirection(), 5);

	world.deleteFireBall(index);
	world.addExplosion(Position(fireball.getX() + alignment, fireball.getY()));
}

void handleJumpingOnNpc(IndependentMovingObject &npc, World &world, Player &player, int index)
{
	player.performAdditionalJump();
	npc.crush(world, index);

	int points = npc.getPointsForCrushing();
	if (points) {
		addTextAndPoints(player, world, points, Position(npc.getX(), npc.getY() - 15));
	}
}

void handlePlayerAndNpcCollision(IndependentMovingObject &npc, World &world, Player &player, int index)
{
	if (player.isImmortal()) {
		if (!npc.isResistantToImmortalPlayer()) {
			Direction direction = determineDirection(player, npc);
			handleNpcDestroying(npc, world, player, direction);
			handleNpcDeleting(world, index);
		}
		else if (isInactiveShell(npc)) {
			Direction direction = determineDirection(player, npc);
			dynamic_cast<Shell*>(&npc)->changeActiveState(direction);
		}
	}
	else if (isInactiveShell(npc)) {
		Direction direction = determineDirection(player, npc);
		dynamic_cast<Shell*>(&npc)->changeActiveState(direction);
	}
	else if (!player.isInsensitive()) {
		player.loseBonusOrLife();
	}
}

void handleCollisionsWithNpcs(Player &player, World &world)
{
	std::vector<std::shared_ptr<IndependentMovingObject>> npcs = world.getNpcs();
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (areColliding(player, **it)) {
			if (isPlayerJumpingOnNpc(player, **it) && !(*it)->isCrushproof()) {
				handleJumpingOnNpc(**it, world, player, it - npcs.begin());
			}
			else {
				handlePlayerAndNpcCollision(**it, world, player, it - npcs.begin());
			}
			return;
		}
	}
}

void handleCollisionsWithFireSerpents(Player &player, World &world)
{
	std::vector<FireSerpent> fireSerpents = world.getFireSerpents();
	for (const auto &fireSerpent : fireSerpents) {
		if (areColliding(player, fireSerpent) && !player.isImmortal() && !player.isInsensitive()) {
			player.loseBonusOrLife();
			return;
		}
	}
}

void handlePlayerCollisions(Player &player, World &world)
{
	handleCollisionsWithNpcs(player, world);
	handleCollisionsWithFireSerpents(player, world);
}

void handleShellsAndNpcsCollisions(World &world, Player &player)
{
	std::vector<std::shared_ptr<IndependentMovingObject>> npcs = world.getNpcs();
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if ((*it)->isActiveShell()) {
			for (auto it2 = npcs.begin(); it2 != npcs.end(); ++it2) {
				if (!(*it2)->isResistantToCollisionWithShell() && areColliding(**it, **it2)) {
					Direction direction = determineDirection(**it, **it2);
					handleNpcDestroying(**it2, world, player, direction);
					handleNpcDeleting(world, it2 - npcs.begin());	
					return;
				}
			}
		}
	}
}

void handleFireBallsAndNpcsCollisions(World &world, Player &player)
{
	std::vector<FireBall> fireballs = world.getFireBalls();
	std::vector<std::shared_ptr<IndependentMovingObject>> npcs = world.getNpcs();
	for (auto it = fireballs.begin(); it != fireballs.end(); ++it) {
		for (auto it2 = npcs.begin(); it2 != npcs.end(); ++it2) {
			if (!(*it2)->isResistantToFireBalls() && areColliding(*it, **it2)) {
				Direction direction = (*it).getDirection();
				handleNpcHpReducing(**it2, world, player, direction, it2 - npcs.begin());
				handleFireBallDeleting(*it, world, it - fireballs.begin());
				return;
			}
		}
	}
}

void handleBlockAndNpcsCollisions(World &world, const Block &block, Player &player)
{
	std::vector<std::shared_ptr<IndependentMovingObject>> npcs = world.getNpcs();
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (!(*it)->isResistantToCollisionWithBlock() && isNpcStandingOnBlock(**it, block)) {
			Direction direction = determineDirection(block, **it);
			handleNpcDestroying(**it, world, player, direction);
			handleNpcDeleting(world, it - npcs.begin());
		}
	}
}

void handleBlockAndBonusesCollisions(World &world, const Block &block, Player &player)
{
	std::vector<std::shared_ptr<BonusObject>> bonuses = world.getBonusElements();
	for (auto it = bonuses.begin(); it != bonuses.end(); ++it) {
		if (isBonusStandingOnBlock(**it, block) && block.canCollideWithBonuses()) {
			if ((*it)->isCoin()) {
				collectCoinByCollision(player, world, it - bonuses.begin());
			}
			else {
				(*it)->knockUp();
			}
			return;
		}
	}
}

void handleBlockCollisions(World &world, const Block &block, Player &player)
{
	handleBlockAndNpcsCollisions(world, block, player);
	handleBlockAndBonusesCollisions(world, block, player);
}

void handleBonusesCollecting(Player &player, World &world)
{
	std::vector<std::shared_ptr<BonusObject>> bonuses = world.getBonusElements();
	for (auto it = bonuses.begin(); it != bonuses.end(); ++it) {
		if (areAtTheSameWidth(player, **it) && areAtTheSameHeight(player, **it) && (*it)->isActive()) {
			(*it)->giveBonus(player);
			addTextAndPoints(player, world, (*it)->getPointsForCollecting());
			world.deleteBonus(it - bonuses.begin());
			return;
		}
	}
}

int getAlignmentForCollisionFromRight(int dist, const WorldObject &obj, const Block &block, const World &world)
{
	int alignment = 0;

	if (block.getType() == BlockType::TubeLeftEntry) {
		if (!isCharacterStandingOnSomething(obj, world)) {
			alignment = (obj.getX() + dist + obj.getWidth() / 2) - (block.getX() - block.getWidth() / 2);
		}
	}
	else {
		alignment = (obj.getX() + dist + obj.getWidth() / 2) - (block.getX() - block.getWidth() / 2);
	}
	
	return (alignment > 0 ? alignment : 0);
}

int getHorizontalAlignmentForCollisionWithBlocks(Direction dir, int dist, const WorldObject &obj, const World &world)
{
	int alignment = 0;
	std::vector<Block> blocks = world.getBlocks();

	for (auto &block : blocks) {
		if (areAtTheSameHeight(obj, block) && isCharacterHittingObject(obj, block, dir, dist) && !block.isInvisible()) {
			if (dir == Direction::Right) {
				alignment = getAlignmentForCollisionFromRight(dist, obj, block, world);
			}
			else if (dir == Direction::Left) {
				alignment = (block.getX() + block.getWidth() / 2) - (obj.getX() - dist - obj.getWidth() / 2);
			}
			break;
		}
	}

	return (alignment > 0 ? alignment : 0);
}

int getHorizontalAlignmentForCollisionWithPlatforms(Direction dir, int dist, const WorldObject &obj, const World &world)
{
	int alignment = 0;
	std::vector<Platform> platforms = world.getPlatforms();

	for (auto &platform : platforms) {
		if (areAtTheSameHeight(obj, platform) && isCharacterHittingObject(obj, platform, dir, dist)) {
			if (dir == Direction::Right) {
				alignment = (obj.getX() + dist + obj.getWidth() / 2) - (platform.getX() - platform.getWidth() / 2);
			}
			else if (dir == Direction::Left) {
				alignment = (platform.getX() + platform.getWidth() / 2) - (obj.getX() - dist - obj.getWidth() / 2);
			}
			break;
		}
	}

	return (alignment > 0 ? alignment : 0);
}

int computeHorizontalAlignment(Direction dir, int dist, const WorldObject &obj, const World &world)
{
	int firstAlignment = getHorizontalAlignmentForCollisionWithBlocks(dir, dist, obj, world);
	int secondAlignment = getHorizontalAlignmentForCollisionWithPlatforms(dir, dist, obj, world);

	return (firstAlignment >= secondAlignment ? firstAlignment : secondAlignment);
}

// this method additionaly set the last touched block's index
int getVerticalAlignmentForCollisionWithBlocks(Direction dir, int dist, const WorldObject &obj, World &world)
{
	int alignment = 0;
	std::vector<Block> blocks = world.getBlocks();

	for (auto it = blocks.begin(); it != blocks.end(); ++it) {
		if (areAtTheSameWidth(obj, *it) && isCharacterHittingObject(obj, *it, dir, dist)) {
			if (dir == Direction::Up) {
				alignment = (it->getY() + it->getHeight() / 2) - (obj.getY() - dist - obj.getHeight() / 2);

				if (alignment > 0) {
					world.setLastTouchedBlock(it - blocks.begin());	// 
				}
			}
			else if ( dir == Direction::Down && !it->isInvisible()) {
				alignment = (obj.getY() + dist + obj.getHeight() / 2) - (it->getY() - it->getHeight() / 2);
			}
			break;
		}
	}

	return (alignment > 0 ? alignment : 0);
}

// this method additionaly set the last touched block's index to -1
int getVerticalAlignmentForCollisionWithPlatforms(Direction dir, int dist, const WorldObject &obj, World &world)
{
	int alignment = 0;
	std::vector<Platform> platforms = world.getPlatforms();

	for (auto it = platforms.begin(); it != platforms.end(); ++it) {
		if (areAtTheSameWidth(obj, *it) && isCharacterHittingObject(obj, *it, dir, dist)) {
			if (dir == Direction::Up) {
				alignment = (it->getY() + it->getHeight() / 2) - (obj.getY() - dist - obj.getHeight() / 2);

				if (alignment > 0) {
					world.setLastTouchedBlock(-1);	// 
				}
			}
			else if (dir == Direction::Down) {
				alignment = (obj.getY() + dist + obj.getHeight() / 2) - (it->getY() - it->getHeight() / 2);
			}
			break;
		}
	}

	return (alignment > 0 ? alignment : 0);
}

int computeVerticalAlignment(Direction dir, int dist, const WorldObject &obj, World &world)
{
	int firstAlignment = getVerticalAlignmentForCollisionWithBlocks(dir, dist, obj, world);
	int secondAlignment = getVerticalAlignmentForCollisionWithPlatforms(dir, dist, obj, world);

	return (firstAlignment >= secondAlignment ? firstAlignment : secondAlignment);
}