#pragma once
#include<SquareBox-Core/SquareBoxGameEngine.h>
class HomeScreen: public SquareBox::IGameScreen
{
public:
	HomeScreen() {};
	~HomeScreen() {};
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;
};

