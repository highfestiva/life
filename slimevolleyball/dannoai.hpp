#pragma once
#include "slimeai.hpp"



namespace slime {



class DannoAI: public SlimeAI {
	private: int type_;

	public: DannoAI() {
		this->team = 4;
		this->type_ = -1;
	}

	private: int howManyFrames(int _i) {
		int i = 0; int j = this->ballY; int k = this->ballVY;
		do { i++; k--; } while ((j += k) > _i);
		return i;
	}

	private: int whereWillBallCross(int _i) {
		int i = howManyFrames(_i);
		int j = this->ballX; int k = this->ballVX;

		for (int m = 0; m < i; m++) {
			j += k;
			if (j < 0) { j = 0; k = -k; } else {
				if (j <= 1000) continue; j = 1000; k = -k;
			}
		}
		return j;
	}

	public: void moveSlime() {
		if ((this->ballX < 500) && (this->type_ != -1)) this->type_ = -1;
		int i = whereWillBallCross(125); howManyFrames(125);
		int j;
		if ((this->p2Y != 0) && (this->p2X < 575)) j = 0; else {
			j = 25 + (int)(10.0 * Math::random());
		}
		if (((this->ballVX == 0) && (this->ballX == 800)) || (this->type_ != -1)) {
			if (this->type_ == -1) {
				if (this->p1X > 250) this->type_ = 0;
				else if (this->p1X < 200) this->type_ = 1;
				else if (this->p1X < 250) this->type_ = 2;
				if (Math::random() < 0.35)
					this->type_ = (int)(3.0 * Math::random());
			}
			switch (this->type_) {
			case 0:
				if ((this->ballY >= 300) || (this->ballVY >= -3))
					break;
				move(1);
				move(2);

				return;
			case 1:
				if ((this->ballY >= 300) || (this->ballVY >= 0))
					break;
				move(0);
				move(2);

				return;
			case 2:
				int k = 860;
				if ((this->ballVY > 12) && (this->p2X < k))
					move(1);
				if (this->p2X >= k)
					move(3);
				if ((this->ballVY == -3) && (this->p2X != 800))
					move(2);
				if ((this->ballVY >= -12) || (this->p2Y == 0) || (this->p2X < k - 15)) break;
				move(0);
				return;
			}
			return;
		}

		if (i < 500) {
			if (Math::abs(this->p2X - 666) < 20) { move(3); return; }
			if (this->p2X > 666) { move(0); return; }
			if (this->p2X < 666) move(1);
			return;
		}if (Math::abs(this->p2X - i) < j) {
			if ((this->p2Y != 0) || ((this->p2Fire) && (Math::random() < 0.3))) return;

			if (((this->p2X >= 900) && (this->ballX > 830)) || ((this->p2X <= 580) && (this->ballX < 530) && (Math::abs(this->ballX - this->p2X) < 100)))
				jump();
			else if ((square(this->ballX - this->p2X) * 2 + square(this->ballY - this->p2Y) < square(170)) && (this->ballX != this->p2X))
				jump();
			else if ((this->ballVX * this->ballVX + this->ballVY * this->ballVY < 20) && (this->ballX - this->p2X < 30) && (this->ballX != this->p2X))
				jump();
			else if ((Math::abs(this->ballX - this->p2X) < 150) && (this->ballY > 50) && (this->ballY < 400) && (Math::random() < 0.666)) {
				jump();
			}
		}
		if ((this->p2Y == 0) && (this->type_ == -1)) {
			if (Math::abs(this->p2X - i) < j) { move(3); return; }
			if (i + j < this->p2X) { move(0); return; }
			if (i + j > this->p2X) { move(1);

				return;
			}

		} else if (this->type_ == -1) {
			if (this->p2X < 575) return;

			if (this->p2X > 900) {
				move(1);
				return;
			}
			if (Math::abs(this->p2X - this->ballX) < j) { move(3); return; }
			if (this->ballX < this->p2X) { move(0); return; }
			if (this->ballX > this->p2X) move(1);
		}
	}

	private: void jump() {
		if (Math::random() < 0.85)
			move(2);
	}
};



}
