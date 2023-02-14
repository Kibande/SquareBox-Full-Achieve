#include "PlayerInputs.h"

PlayerInputs::PlayerInputs()
{

}

Move PlayerInputs::getMove(SquareBox::IMainGame* _game_ptr)
{
    if (_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_a)))
    {
        return Move::LEFT;
    }
    else if (_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_d)))
    {
        return Move::RIGHT;
    }
    else if (_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_w)))
    {
        return Move::UP;
    }
    else if (_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_s)))
    {
        return Move::DOWN;
    }

    return Move::NONE;
}
