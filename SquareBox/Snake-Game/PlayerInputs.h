#ifndef PLAYERINPUTS_H_INCLUDED
#define PLAYERINPUTS_H_INCLUDED
#include<SquareBox-Core/SquareBoxGameEngine.h>
enum class Move
{
    NONE = -1,
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};
class PlayerInputs
{
public:
    PlayerInputs();

    Move getMove(SquareBox::IMainGame * _game_ptr);
};


#endif // PLAYERINPUTS_H_INCLUDED
