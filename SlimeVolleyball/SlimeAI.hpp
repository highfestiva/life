#pragma once
#include "Slime.hpp"



namespace Slime
{



class SlimeVolleyball;



class SlimeAI
{
	protected: int ballX;
	protected: int ballY;
	protected: int ballVX;
	protected: int ballVY;
	protected: int p1X;
	protected: int p1Y;
	protected: int p1VX;
	protected: int p1VY;
	protected: int p2X;
	protected: int p2Y;
	protected: int p2VX;
	protected: int p2VY;
	protected: bool p1Fire;
	protected: bool p2Fire;
	protected: int team;
	protected: int mPercent;
	private: int player;
	private: SlimeVolleyball* app;
	public: static const int LEFT = 0;
	public: static const int RIGHT = 1;
	public: static const int JUMP = 2;
	public: static const int STOP = 3;

	public: SlimeAI();

	public: virtual int init(SlimeVolleyball* paramSlime1P, int paramInt);

	public: virtual void saveVars(int paramArrayOfInt[], bool paramBoolean1, bool paramBoolean2);

	public: virtual void moveSlime() = 0;

	protected: virtual void move(int paramInt);

	protected: int square(int paramInt) {
		return paramInt * paramInt;
	}
};



}
