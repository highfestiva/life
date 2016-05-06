#pragma once
#include "slime.hpp"



namespace slime {



class SlimeVolleyball;



class SlimeAI {
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
	protected: int percent_;
	private: int player;
	private: SlimeVolleyball* app;
	public: static const int LEFT = 0;
	public: static const int RIGHT = 1;
	public: static const int JUMP = 2;
	public: static const int STOP = 3;

	public: SlimeAI();

	public: virtual int init(SlimeVolleyball* slime1_p, int i);

	public: virtual void saveVars(int array_of_int[], bool boolean1, bool boolean2);

	public: virtual void moveSlime() = 0;

	protected: virtual void move(int i);

	protected: int square(int i) {
		return i * i;
	}
};



}
