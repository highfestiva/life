#pragma once
#include "slimeai.hpp"



namespace slime {



class HighfestivaAI: public SlimeAI {
	private: int type_;

	public: HighfestivaAI() {
		this->type_ = -1;
	}

	public: void moveSlime() {
		if ((this->type_ != -1) || ((this->ballX == 800) && (this->ballVX == 0))) {
			serve();
			return;
		}

		int j = xAtY(this->p2Y + this->p2VY + 30);
		int i;
		if (ballVX == 0 && Math::abs(p2X-j) <= 10) {
			i = (int)(Math::random()*31) - 15;
		} else if (Math::abs(ballVX) < 5) {
			i = (int)(Math::random()*11) - 5;
		} else if (ballVX < 0) {
			if (j > 700) {
				i = 45 + (int)(Math::random()*11);
			} else {
				i = 10 + (int)(Math::random()*11);
			}
		} else {
			i = -45 - (int)(Math::random()*11);
		}
		/*if (j < 600) i = 0;
		else if (j < 700) i = 5; else {
			i = 10;
		}*/
		if (j < 450) {
			if (Math::abs(this->p2X - 700) < 10) move(3);
			else if (this->p2X > 700) move(0);
			else if (this->p2X < 700) move(1);
		} else if (Math::abs(this->p2X - j - i) < 10) move(3);
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
			const int dist_x = Math::abs(this->ballX - this->p2X);
			if (dist_x < (this->p2Fire ? 135 : 150) && (this->ballY > 50) && (this->ballY < 250)) {
				if (dist_x < 75 ||
					dist_x < (ballY-50)/2) {
					move(2);
				}
			}
		}
	}

	private: int xAtY(int _i) {
		int i = this->ballX; int j = this->ballY; int k = this->ballVY;
		do {
			i += this->ballVX;

			if (i <= 0) {
				i = 0;
				this->ballVX = (-this->ballVX);
			} else if (i >= 1000) {
				i = 1000;
				this->ballVX = (-this->ballVX);
			}
			k--; } while ((j += k) > _i);

		return i;
	}

	private: void serve() {
		if (this->type_ == -1) {
			type_ = (int)(Math::random()*7.0) + 5;
			if (Math::random() < 0.1) {
				if ((this->p1X < 300) && (!this->p2Fire))
					this->type_ = 0;
				else if (this->p1X > 200)
					this->type_ = 1;
				else this->type_ = (int)(Math::random()*3.0);
			}
			if ((this->p2Fire) && (this->type_ == 0))
				this->type_ = (1 + (int)(Math::random() * 2.0));
		}
		int i;
		switch (this->type_) {
		case 0:	// Go right, jump, hit falling ball. Results in smash.
		case 1: {	// Go right, jump, hit falling ball. Results in long serve.
			int j = this->type_ == 0 ? 860 : 840;
			if ((this->ballVY > 12) && (this->p2X < j))
				move(1);
			if (this->p2X >= j)
				move(3);
			if ((this->ballVY == -3) && (this->p2X != 800))
				move(2);
			if ((this->ballVY < -12) && (this->p2Y != 0) && (this->p2X >= j - 15) && (this->type_ == 0))
				move(0);
			if (this->ballX >= 700) break;
			this->type_ = -1;
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
			this->type_ = (3 + fakeJump());
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
			this->type_ = -1;
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
			this->type_ = -1;
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
			this->type_ = -1;
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
			this->type_ = -1;
			return;
		case 9:
		default:
			if (ballY >= 150) break;
			type_ = (Math::random() < 0.5)? 100 : 101;
			break;
		case 100:	// Continuation: jumping right/up on a ball falling down, resulting in long serve.
			if (ballY < 150 || ballVY >= -5) break;
			// TRICKY: falls through.
		case 101:	// Continuation: (if not fallen through) jumping right/up on a ball just about to bounce, resulting in backwards serve, landing just over the net.
			i = 820;
			move(2);
			if (this->p2X < i) {
				move(1);
			} else {
				move(3);
				this->type_ = -1;
			} break;
		}
	}

	private: int fakeJump() {
		int i = 0;
		if (this->p1X < 200) i = 1;
		else if (this->p1X > 300) i = 0;
		if (Math::random() < 0.35) i = 1 - i;
		return i;
	}
};



}
