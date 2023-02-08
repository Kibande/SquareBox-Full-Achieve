#ifndef SNAKE_H_INCLUDED
#define SNAKE_H_INCLUDED
#include <list>
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <cmath>

struct SnakeNode
{
    int x, y;
    double c=0.0, maxC=0.0;
    SnakeNode(int arg_x, int arg_y, double arg_maxC=10) : x(arg_x), y(arg_y), maxC(arg_maxC) {}
};

class Snake
{
public:
    Snake() {};
    Snake(int arg_x, int arg_y);

    void update();
    void progress(const int word_width, const int world_height);
    void kill() {_snake.clear();}
    void draw(SquareBox::RenderEngine::SpriteBatch  & renderer,const float heightScore);
    void addNode(int x, int y);
    void setDirection(int vx, int vy) {_vx = vx; _vy = vy;}
    void addLife(double life);

    const SnakeNode& getHead() const;
    const std::list<SnakeNode>& getNodeList() const {return _snake;}
    bool isDead() const {return _dead;}
    double getLife() const {return _life;}

private:
    void checkAutoCollision();

    std::list<SnakeNode> _snake, _deadNodes;
    int _vx, _vy;
    double _life;
    bool _dead;

    SquareBox::Timer _timer;
};

#endif // SNAKE_H_INCLUDED
