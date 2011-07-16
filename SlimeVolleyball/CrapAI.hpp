#pragma once
#include "SlimeAI.hpp"



namespace Slime
{



class CrapAI: public SlimeAI
{
private:
	private: int mDelayFrames;
	private: int mFrame;
	private: int mLastBallVX;
	private: int mLastBallVY;
	private: int serveType;

	public: CrapAI(double pDelay, int pPercent):
		mDelayFrames((int)(pDelay*44)),
		mFrame(0),
		mLastBallVX(0),
		mLastBallVY(0)
	{
		this->team = 4;
		this->mPercent = pPercent;
		this->serveType = -1;
	}

	private: int howManyFrames(int paramInt)
	{
		int i = 0; int j = this->ballY; int k = this->ballVY;
		do { i++; k--; } while ((j += k) > paramInt);
		return i;
	}

	private: int whereWillBallCross(int paramInt)
	{
		int i = howManyFrames(paramInt);
		int j = this->ballX; int k = this->ballVX;

		for (int m = 0; m < i; m++) {
			j += k;
			if (j < 0) { j = 0; k = -k; } else {
				if (j <= 1000) continue; j = 1000; k = -k;
			}
		}
		return j;
	}

	public: void moveSlime()
	{
		++mFrame;
		if ((this->ballX < 500) && (this->serveType != -1)) this->serveType = -1;
		int i = whereWillBallCross(125); howManyFrames(125);
		int j;
		if ((this->p2Y != 0) && (this->p2X < 575)) j = 0; else {
			j = 23 + (int)(15.0 * Math::random());
		}
		if (((this->ballVX == 0) && (this->ballX == 800)) || (this->serveType != -1))
		{
			if (this->serveType == -1)
			{
				if (this->p1X > 250) this->serveType = 0; else
					this->serveType = 1;
				if (Math::random() < 0.35)
					this->serveType = (int)(2.0 * Math::random());
			}
			switch (this->serveType) {
			case 0:
				if ((this->ballY >= 300) || (this->ballVY >= -3))
					break;
				move(1);
				move(2);
				SaveBall();
				return;
			case 1:
				const int lJumpHeight = (mPercent == 100)? 300 : 350;
				if ((this->ballY >= lJumpHeight) || (this->ballVY >= 0))
					break;
				move(0);
				move(2);
				SaveBall();
				return;
			}
			SaveBall();
			return;
		}

		if (i < 500)
		{
			if (Math::abs(this->p2X - 800) < 20) { Move(3); SaveBall(); return; }
			if (this->p2X > 800) { Move(0); SaveBall(); return; }
			if (this->p2X < 800) Move(1);
			SaveBall();
			return;
		}
		if (Math::abs(this->p2X - i) < j)
		{
			if ((this->p2Y != 0) || ((this->p2Fire) && (Math::random() < 0.3))) { SaveBall(); return; }

			if (((this->p2X >= 900) && (this->ballX > 830)) || ((this->p2X <= 580) && (this->ballX < 530) && (Math::abs(this->ballX - this->p2X) < 120)))
				jump();
			else if ((square(this->ballX - this->p2X) * 2 + square(this->ballY - this->p2Y) < square(170)) && (this->ballX != this->p2X))
				jump();
			else if ((this->ballVX * this->ballVX + this->ballVY * this->ballVY < 20) && (this->ballX - this->p2X < 30) && (this->ballX != this->p2X))
				jump();
			else if ((Math::abs(this->ballX - this->p2X) < 150) && (this->ballY > 50) && (this->ballY < 500) && (Math::random() < 0.5)) {
				jump();
			}
		}
		if ((this->p2Y == 0) && (this->serveType == -1))
		{
			if (Math::abs(this->p2X - i) < j) { Move(3); SaveBall(); return; }
			if (i + j < this->p2X) { Move(0); SaveBall(); return; }
			if (i + j > this->p2X)
			{
				Move(1);
				SaveBall();
				return;
			}

		}
		else if (this->serveType == -1)
		{
			if (this->p2X < 575) { SaveBall(); return; }

			if (this->p2X > 900)
			{
				Move(1);
				SaveBall();
				return;
			}
			if (Math::abs(this->p2X - this->ballX) < j) { Move(3); SaveBall(); return; }
			if (this->ballX < this->p2X) { Move(0); SaveBall(); return; }
			if (this->ballX > this->p2X) Move(1);
		}
		SaveBall();
	}

	private: void jump()
	{
		if (Math::random() < 0.4)
			Move(2);
	}

	private: void Move(int pMovement)
	{
		if (!mDelayFrames)
		{
			move(pMovement);
		}
		if (//(mLastBallVX > 0) != (this->ballVX > 0) ||	// Changed X-direction, or...
			(mLastBallVY < 0 && this->ballVY > 0))	// ... bounced up.
		{
			mFrame = 0;
		}
		if (mFrame < mDelayFrames)
		{
			return;
		}
		move(pMovement);
	}

	private: void SaveBall()
	{
		mLastBallVX = this->ballVX;
		mLastBallVY = this->ballVY;
	}
};



}