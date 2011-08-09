#pragma once
#include "SlimeAI.hpp"



namespace Slime
{



class HighfestivaAI: public SlimeAI
{
	private: int serveType;

	public: HighfestivaAI()
	{
		this->serveType = -1;
	}

	public: void moveSlime()
	{
		if ((this->serveType != -1) || ((this->ballX == 800) && (this->ballVX == 0)))
		{
			serve();
			return;
		}

		int j = xAtY(this->p2Y + this->p2VY + 30);
		int i;
		if (ballVX == 0 && Math::abs(p2X-j) <= 10)
		{
			i = (int)(Math::random()*31) - 15;
		}
		else if (Math::abs(ballVX) < 5)
		{
			i = (int)(Math::random()*11) - 5;
		}
		else if (ballVX < 0)
		{
			if (j > 700)
			{
				i = 45 + (int)(Math::random()*11);
			}
			else
			{
				i = 10 + (int)(Math::random()*11);
			}
		}
		else
		{
			i = -45 - (int)(Math::random()*11);
		}
		/*if (j < 600) i = 0;
		else if (j < 700) i = 5; else {
			i = 10;
		}*/
		if (j < 450)
		{
			if (Math::abs(this->p2X - 700) < 10) move(3);
			else if (this->p2X > 700) move(0);
			else if (this->p2X < 700) move(1);
		}
		else if (Math::abs(this->p2X - j - i) < 10) move(3);
		else if (j + i < this->p2X) move(0);
		else if (j + i > this->p2X) move(1);

		if (((this->p2X <= 900) || (Math::random() >= 0.4)) && (j >= 620) && 
			((this->ballY >= 130) || (this->ballVY >= 0)) && (
			(!this->p2Fire) || (Math::random() >= 0.6))) {
			if (((this->p2X >= 900) && (this->ballX > 830)) || ((this->p2X <= 580) && (this->ballX < 530) && (Math::abs(this->ballX - this->p2X) < 100))) {
				move(2);

				return;
			}
			if ((square(this->ballX - this->p2X) * 2 + square(this->ballY - this->p2Y) < square(185)) && (this->ballX != this->p2X)) {
				move(2);

				return;
			}
			if ((this->ballVX * this->ballVX + this->ballVY * this->ballVY < 20) && (this->ballX - this->p2X < 30) && (this->ballX != this->p2X)) {
				move(2);

				return;
			}
			const int lDistX = Math::abs(this->ballX - this->p2X);
			if (lDistX < (this->p2Fire ? 135 : 150) && (this->ballY > 50) && (this->ballY < 250))
			{
				if (lDistX < 75 ||
					lDistX < (ballY-50)/2)
				{
					move(2);
				}
			}
		}
	}

	private: int xAtY(int paramInt)
	{
		int i = this->ballX; int j = this->ballY; int k = this->ballVY;
		do
		{
			i += this->ballVX;

			if (i <= 0)
			{
				i = 0;
				this->ballVX = (-this->ballVX);
			} else if (i >= 1000)
			{
				i = 1000;
				this->ballVX = (-this->ballVX);
			}
			k--; } while ((j += k) > paramInt);

		return i;
	}

	private: void serve()
	{
		if (this->serveType == -1)
		{
			serveType = (int)(Math::random()*7.0) + 5;
			if (Math::random() < 0.1)
			{
				if ((this->p1X < 300) && (!this->p2Fire))
					this->serveType = 0;
				else if (this->p1X > 200)
					this->serveType = 1;
				else this->serveType = (int)(Math::random()*3.0);
			}
			if ((this->p2Fire) && (this->serveType == 0))
				this->serveType = (1 + (int)(Math::random() * 2.0));
		}
		int i;
		switch (this->serveType) {
		case 0:	// Go right, jump, hit falling ball. Results in smash.
		case 1:	// Go right, jump, hit falling ball. Results in long serve.
		{
			int j = this->serveType == 0 ? 860 : 840;
			if ((this->ballVY > 12) && (this->p2X < j))
				move(1);
			if (this->p2X >= j)
				move(3);
			if ((this->ballVY == -3) && (this->p2X != 800))
				move(2);
			if ((this->ballVY < -12) && (this->p2Y != 0) && (this->p2X >= j - 15) && (this->serveType == 0))
				move(0);
			if (this->ballX >= 700) break;
			this->serveType = -1;
		}
		return;
		case 2:	// Tricky serve: hit it backwards, continue with long double-hit or fake it and make it drop just over the net.
			i = 770;
			if ((this->ballVY > 12) && (this->p2X > i))
				move(0);
			if (this->p2X <= i)
				move(3);
			if ((this->ballVY == -2) && (this->p2X != 800))
				move(2);
			if ((this->p2Y == 0) || (this->ballX <= 800)) break;
			this->serveType = (3 + fakeJump());
			return;
		case 3:	// Continuation: double-hit serve to the back.
			i = !this->p2Fire ? 585 : 555;
			if (this->p2X > i)
				move(0);
			if (this->p2X <= i)
				move(3);
			if (this->ballX <= (!this->p2Fire ? 730 : 700))
				move(2);
			if (this->ballX >= 540) break;
			this->serveType = -1;
			return;
		case 4:	// Continuation: faking double-hit serve, but actually only hitting once, landing it near the net.
			i = !this->p2Fire ? 585 : 555;
			if (this->p2X > i)
				move(0);
			if (this->p2X <= i)
				move(3);
			if (this->ballX <= (!this->p2Fire ? 730 : 700))
				move(2);
			if (this->ballX < 600)
				move(1);
			if (this->ballX < 580)
				move(3);
			if (this->ballX >= 540) break;
			this->serveType = -1;
			return;
		case 5:
			if (ballVY <= 0) break;
		case 6:
			i = 780;
			if (this->p2X > i)
				move(0);
			if (this->p2X <= i)
				move(3);
			if (this->ballX >= 640) break;
			this->serveType = -1;
			return;
		case 7:
			if (ballVY <= 0) break;
		case 8:
			i = 820;
			if (this->p2X < i)
				move(1);
			if (this->p2X >= i)
				move(3);
			if (this->ballX >= 640) break;
			this->serveType = -1;
			return;
		case 9:
		default:
			if (ballY >= 150) break;
			serveType = (Math::random() < 0.5)? 100 : 101;
			break;
		case 100:	// Continuation: jumping right/up on a ball falling down, resulting in long serve.
			if (ballY < 150 || ballVY >= -5) break;
			// TRICKY: falls through.
		case 101:	// Continuation: (if not fallen through) jumping right/up on a ball just about to bounce, resulting in backwards serve, landing just over the net.
			i = 820;
			move(2);
			if (this->p2X < i)
			{
				move(1);
			}
			else
			{
				move(3);
				this->serveType = -1;
			}
			break;
		}
	}

	private: int fakeJump()
	{
		int i = 0;
		if (this->p1X < 200) i = 1;
		else if (this->p1X > 300) i = 0;
		if (Math::random() < 0.35) i = 1 - i;
		return i;
	}
};



}
