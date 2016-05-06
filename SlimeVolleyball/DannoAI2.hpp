#pragma once
#include "slimeai.hpp"



namespace slime {



class DannoAI2: public SlimeAI {
	private: int type_;

	public: DannoAI2() {
		this->team = 2;
		this->type_ = -1;
	}

	public: void moveSlime() {
		if ((this->type_ != -1) || ((this->ballX == 800) && (this->ballVX == 0))) {
			serve();
			return;
		}

		int j = xAtY(this->p2Y + this->p2VY + 30);
		int i;
		if (j < 600) i = 0;
		else if (j < 700) i = 10;
		else i = 20;

		i += (int)(Math::random()*8);	// high_festiva was here!
		if (Math::random() > 0.98) {
			i += (int)(Math::random()*10) + 10;
		}

		if (j < 450) {
			if (Math::abs(this->p2X - 666) < 10) move(3);
			else if (this->p2X > 666) move(0);
			else if (this->p2X < 666) move(1);

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
			if (Math::abs(this->ballX - this->p2X) < (this->p2Fire ? 135 : 150)) if ((this->ballY > 50) && (this->ballY < 250))
					move(2);
		}
	}

	private: int framesTillY(int /*_i*/) {
		int i = 0; int j = this->ballY; int k = this->ballVY;
		do { i++; k--; } while ((j += k) > 0);
		return i;
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

	private: int ballDist() {
		int i = this->p2X - this->ballX; int j = this->p2Y - this->ballY;
		return (int)Math::sqrt(i * i + j * j);
	}

	private: void serve() {
		if (this->type_ == -1) {
			if (Math::random() < 0.3) {
				if ((this->p1X < 300) && (!this->p2Fire))
					this->type_ = 0;
				else if (this->p1X > 200)
					this->type_ = 1;
				else this->type_ = 2;
			} else
				this->type_ = 2;
			if ((this->type_ == -1) || (Math::random() < 0.3))
				this->type_ = (int)(Math::random() * 3.0);
			if ((this->p2Fire) && (this->type_ == 0))
				this->type_ = (1 + (int)(Math::random() * 2.0));
		}
		int i;
		switch (this->type_) {
		case 0:
		case 1: {
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
		case 2:
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
		case 3:
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
		case 4:
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
