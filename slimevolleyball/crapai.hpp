#pragma once
#include "slimeai.hpp"



namespace slime {



class CrapAI: public SlimeAI {
private:
	private: int delay_frames_;
	private: int frame_;
	private: int last_ball_vx_;
	private: int last_ball_vy_;
	private: int type_;

	public: CrapAI(double delay, int percent):
		delay_frames_((int)(delay*44)),
		frame_(0),
		last_ball_vx_(0),
		last_ball_vy_(0) {
		this->team = 4;
		this->percent_ = percent;
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
		++frame_;
		if ((this->ballX < 500) && (this->type_ != -1)) this->type_ = -1;
		int i = whereWillBallCross(125); howManyFrames(125);
		int j;
		if ((this->p2Y != 0) && (this->p2X < 575)) j = 0; else {
			j = 23 + (int)(15.0 * Math::random());
		}
		if (((this->ballVX == 0) && (this->ballX == 800)) || (this->type_ != -1)) {
			if (this->type_ == -1) {
				if (this->p1X > 250) this->type_ = 0; else
					this->type_ = 1;
				if (Math::random() < 0.35)
					this->type_ = (int)(2.0 * Math::random());
			}
			switch (this->type_) {
			case 0:
				if ((this->ballY >= 300) || (this->ballVY >= -3))
					break;
				move(1);
				move(2);
				SaveBall();
				return;
			case 1:
				const int jump_height = (percent_ == 100)? 300 : 350;
				if ((this->ballY >= jump_height) || (this->ballVY >= 0))
					break;
				move(0);
				move(2);
				SaveBall();
				return;
			}
			SaveBall();
			return;
		}

		if (i < 500) {
			if (Math::abs(this->p2X - 800) < 20) { Move(3); SaveBall(); return; }
			if (this->p2X > 800) { Move(0); SaveBall(); return; }
			if (this->p2X < 800) Move(1);
			SaveBall();
			return;
		}
		if (Math::abs(this->p2X - i) < j) {
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
		if ((this->p2Y == 0) && (this->type_ == -1)) {
			if (Math::abs(this->p2X - i) < j) { Move(3); SaveBall(); return; }
			if (i + j < this->p2X) { Move(0); SaveBall(); return; }
			if (i + j > this->p2X) {
				Move(1);
				SaveBall();
				return;
			}

		} else if (this->type_ == -1) {
			if (this->p2X < 575) { SaveBall(); return; }

			if (this->p2X > 900) {
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

	private: void jump() {
		if (Math::random() < 0.4)
			Move(2);
	}

	private: void Move(int movement) {
		if (!delay_frames_) {
			move(movement);
		}
		if (//(mLastBallVX > 0) != (this->ballVX > 0) ||	// Changed X-direction, or...
			(last_ball_vy_ < 0 && this->ballVY > 0)) {	// ... bounced up.
			frame_ = 0;
		}
		if (frame_ < delay_frames_) {
			return;
		}
		move(movement);
	}

	private: void SaveBall() {
		last_ball_vx_ = this->ballVX;
		last_ball_vy_ = this->ballVY;
	}
};



}
