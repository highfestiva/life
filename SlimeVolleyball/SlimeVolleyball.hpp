
#pragma once

#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/hirestimer.h"
#include "../lepra/include/thread.h"
#include "../uilepra/include/uiinput.h"
#include "../uilepra/include/uiopenglextensions.h"
#include "crapai.hpp"
#include "dannoai.hpp"
#include "dannoai2.hpp"
#include "highfestivaai.hpp"



namespace slime {



struct FingerMovement {
	int start_x_;
	int start_y_;
	int last_x_;
	int last_y_;
	bool is_press_;

	inline FingerMovement(int x, int y):
		start_x_(x),
		start_y_(y),
		last_x_(x),
		last_y_(y),
		is_press_(true) {
	}

	inline bool Update(int last_x, int last_y, int new_x, int new_y) {
		if (std::abs(last_x_-last_x) < 10 && std::abs(last_y_-last_y) < 10) {
			last_x_ = new_x;
			last_y_ = new_y;
			return true;
		}
		return false;
	}
};

typedef std::list<slime::FingerMovement> FingerMoveList;
extern FingerMoveList g_finger_move_list;

inline int countStartPos(bool left, int screen_width) {
	int count = 0;
	FingerMoveList::iterator i = g_finger_move_list.begin();
	for (; i != g_finger_move_list.end(); ++i) {
		const int ox = (int)(i->start_y_*1000/screen_width);
		if ((left && ox < 500) || (!left && ox > 500)) {
			++count;
		}
	}
	return count;
}



class SlimeVolleyball {
	private: static const int TAP_JUMP_HEIGHT = 50;
	private: static const int TAP_JUMP_MIN_HEIGHT = 27;
	private: static const int TAP_JUMP_DELTA_HEIGHT = 5;
	private: static const int TAP_BASE = 150;
	private: int nWidth;
	private: int nHeight;
	private: int p1X;
	private: int p1Y;
	private: int p2X;
	private: int p2Y;
	private: int p1Col;
	private: int p2Col;
	private: int p1XV;
	private: int p1YV;
	private: int p2XV;
	private: int p2YV;
	private: bool tapActive;
	private: int p1TapX;
	private: int p1TapY;
	private: bool p1TapJump;
	private: int p2TapX;
	private: int p2TapY;
	private: bool p2TapJump;
	private: int ballX;
	private: int ballY;
	private: int ballVX;
	private: int ballVY;
	private: Graphics screen;
	private: bool pressed_;
	public: bool fInPlay;
	private: bool fP1Fire;
	private: bool fP2Fire;
	private: bool flash_;
	private: int fP1Touched;
	private: int fP2Touched;
	private: int fP1Touches;
	private: int fP2Touches;
	private: int fP1TouchesTot;
	private: int fP2TouchesTot;
	private: int nPointsScored;
	private: int nScore;
	private: bool fServerMoved;
	private: bool doRunGameThread;
	private: bool fEndGame;
	private: long time_;
	private: long gameTime;
	private: long pausedTime;
	private: bool paused;
	private: int scoringRun_;
	private: Color colours_[7];
	private: Color human2PSlimeColours[5];
	private: Color* activeSlimeColours;
	private: int activeColCount;
	private: str* activeSlimeColText;
	public: Color SKY_COL;
	private: Color COURT_COL;
	private: Color BALL_COL;
	private: int frenzyCol;
	private: int ground_hit_x;
	private: long score_time;
	public: int speed_;
	private: HiResTimer wait_run_timer_;
	private: double average_loop_time_;
	private: HiResTimer loop_timer_;
	private: bool was_frozen_;
	private: int aiMode;
	private: SlimeAI* ai;
	public: bool bGameOver;
	public: int player_count_;
	private: bool show_title_;
	private: str promptMsg;
	private: str col_text_[7];
	private: str text1[6];
	private: str text2[6];
	private: str human2PSlimeColText[5];

#define aiStartLevel   0
#define ai2PStartLevel 1

	public: SlimeVolleyball() {
		CLEAR_MEMBERS(nWidth, show_title_);
		player_count_ = 2;
	}

	public: bool init(const Graphics& graphics) {
		this->tapActive = false;
		this->screen = graphics;
		this->nWidth = this->screen.width;
		this->nHeight = this->screen.height*4/5;
		this->fInPlay = this->fEndGame = false;
		nScore = 5;
		this->promptMsg = "";
		str s[] = { "Inferior Human Controlled Slime ", "The Lazy Orange Slime ", "The Pathetic White Slime ", "Angry Red Slimonds ", "The Slime Master ", "Psycho Slime ", "Slime Wall " };
		LEPRA_ARRAY_ASSIGN(this->col_text_, s);
		Color c[] = { YELLOW, ORANGE, Color(220,235,220), RED, BLACK, BLUE, PINK };
		LEPRA_ARRAY_ASSIGN(this->colours_, c);
		str s2[] = { "You are a loser!", this->col_text_[2] + "delivers pwnage!", this->col_text_[3] + "gives you the gong!", this->col_text_[4] + "says \"You are seriously inept.\"", this->col_text_[5] + "laughs at the pathetic slow opposition.", this->col_text_[6] + "is still invincible!" };
		LEPRA_ARRAY_ASSIGN(this->text1, s2);
		str s3[] = { "Better luck next time.", "You should repay in vengeance!", "So who has the red face bombing out on level 3, huh?", "Congrats on reaching level 4.", "Congrats on reaching level 5!", "You fell at the last hurdle... but get up and try again!" };
		LEPRA_ARRAY_ASSIGN(this->text2, s3);
		str s4[] = { "Big Red Slime ", "Magic Green Slime ", "Golden Boy ", "The Great White Slime ", "The Grass Tree(r) " };
		LEPRA_ARRAY_ASSIGN(this->human2PSlimeColText, s4);
		Color c2[] = { RED, GREEN, YELLOW, WHITE, BLACK };
		LEPRA_ARRAY_ASSIGN(this->human2PSlimeColours, c2);
		this->p1Col = 0;
		this->bGameOver = true;
		this->paused = false;
		this->loop_timer_.PopTimeDiff();

		this->aiMode = aiStartLevel;

		setAI();
		//repaint();
		return true;
	}

	private: void setAI() {
		//this->fP1PointsWon = 0;
		//this->fP2PointsWon = 0;
		this->nScore = 5;
		delete this->ai;
		switch (this->aiMode) {
		case 0:
		default:
			this->ai = new CrapAI(0.4, 75);
			this->fP2Fire = false;
			this->SKY_COL = LIGHT_BLUE;
			this->COURT_COL = BROWN;
			this->BALL_COL = WHITE;
			break;
		case 1:
			this->ai = new CrapAI(0, 100);
			this->fP2Fire = false;
			this->SKY_COL = BLUE;
			this->COURT_COL = GRAY;
			this->BALL_COL = YELLOW;
			break;
		case 2:
			this->ai = new DannoAI();
			this->fP2Fire = false;
			this->SKY_COL = Color(30, 80, 0);
			this->COURT_COL = DARK_GRAY;
			this->BALL_COL = Color(128, 128, 255);
			break;
		case 3:
			this->ai = new DannoAI2();
			this->fP2Fire = false;
			this->SKY_COL = Color(98, 57, 57);
			this->COURT_COL = Color(0, 168, 0);
			this->BALL_COL = WHITE;
			break;
		case 4:
			this->ai = new DannoAI2();
			this->fP2Fire = true;
			this->SKY_COL = BLACK;
			this->COURT_COL = RED;
			this->BALL_COL = YELLOW;
			break;
		case 5:
			this->ai = new HighfestivaAI();
			this->fP2Fire = false;
			this->SKY_COL = DARK_CYAN;
			this->COURT_COL = BLACK;
			this->BALL_COL = LIGHT_GREEN;
			break;
		}
		this->p2Col = (this->aiMode + 1);
		this->ai->init(this, 2);
	}

	public: void paint(const Graphics& graphics) {
		activeSlimeColText = (player_count_ == 2)? human2PSlimeColText : col_text_;
		activeSlimeColours = (player_count_ == 2)? human2PSlimeColours : colours_;
		activeColCount = (player_count_ == 2)? LEPRA_ARRAY_COUNT(human2PSlimeColours) : LEPRA_ARRAY_COUNT(colours_);

		this->screen = graphics;
		this->nWidth = screen.width;
		this->nHeight = screen.height*4/5;
		/*screen.setColor(this->SKY_COL);
		screen.fillRect(0, 0, this->nWidth, 4 * this->nHeight / 5);*/
		screen.setColor(this->COURT_COL);
		screen.fillRect(0, 4 * this->nHeight / 5, this->nWidth, this->nHeight/2);
		screen.setColor(WHITE);
		screen.fillRect(this->nWidth / 2 - 2, 7 * this->nHeight / 10, 4, this->nHeight / 10 + 5);
		FontMetrics font_metrics = screen.getFontMetrics();
		if (show_title_) {
			const int h = font_metrics.getHeight();
			this->screen.centerString("SlimeBall", this->nHeight / 2 - h*4);
			str s = "Base by Quin Pendragon\n" \
				"AI by Daniel Wedge\n" \
				"Port by Pixel Doctrine\n" \
				"Music by Grotesco";
			screen.centerString(s, this->nHeight / 2 - h*2);
			drawScores();
			DrawStatus();
			return;
		}

		if (!this->fInPlay && !this->bGameOver) {
			if (player_count_ == 1) {
				screen.setColor(WHITE);
				str s1 = str("Level ") + strutil::IntToString(this->aiMode + 1, 10) + " clear!";
				this->screen.centerString(s1, this->nHeight / 3);
			}
			//str s2 = str("Your score: ") + strutil::IntToString(this->gameScore, 10);
			//screen.centerString(s2, this->nHeight / 2);
			/*if (this->fP1PointsWon == 6) {
				str s3 = str("Level bonus: ") + strutil::IntToString(1000 * this->fP1PointsWon / (this->fP1PointsWon + this->fP2PointsWon) * scale(), 10) + " points";
				screen.centerString(s3, this->nHeight / 2 + font_metrics.getHeight());
				str s4 = str("Time bonus: ") + strutil::IntToString((this->gameTime < 300000L ? 300000L - this->gameTime : 0L) / 1000L * scale(), 10) + " points";
				screen.centerString(s4, this->nHeight / 2 + font_metrics.getHeight() * 2);
				if (this->fP2PointsWon == 0) {
					str s5 = str("Flawless Victory: ") + strutil::IntToString(1000 * scale(), 10) + " points";
					screen.centerString(s5, this->nHeight / 2 + font_metrics.getHeight() * 3);
				}
			}*/
			drawScores();

			return;
		}

		drawScores();

		drawTapIndicators();
	}

	public: bool handleEvent(Event event) {
		switch (event.id) {
		case 401:
		case 403:
			switch (event.key) {
			case uilepra::InputManager::kInKbdDown:
				if (player_count_ == 2) {
					do
						this->p2Col = (this->p2Col != 4 ? this->p2Col + 1 : 0);
					while (this->p1Col == this->p2Col);
				} break;
			case uilepra::InputManager::kInKbdLeft:
				if (player_count_ == 2) {
					moveP2Left();
					break;
				}
				// TRICKY: fall thru.
			case 'a':
			case 'A':
				moveP1Left();
				break;
			case uilepra::InputManager::kInKbdRight:
				if (player_count_ == 2) {
					moveP2Right();
					break;
				}
				// TRICKY: fall thru.
			case 'd':
			case 'D':
				moveP1Right();
				break;
				// TRICKY: fall thru.
			case uilepra::InputManager::kInKbdUp:
				if (player_count_ == 2) {
					moveP2Jump();
					break;
				}
				// TRICKY: fall thru.
			case 'w':
			case 'W':
				moveP1Jump();
				break;
			case 's':
			case 'S':
				if (player_count_ == 2) {
					do
						this->p1Col = (this->p1Col != 4 ? this->p1Col + 1 : 0);
					while (this->p1Col == this->p2Col);
				} break;

			case 'p':
			case 'P':
				if (!this->paused) {
					this->pausedTime = system::currentTimeMillis();
					this->paused = true;
				} else {
					this->time_ += system::currentTimeMillis() - this->pausedTime;
					this->paused = false;
				} break;
			case uilepra::InputManager::kInKbdPlus:
			case uilepra::InputManager::kInKbdNumpadPlus:
				speed_ = 0;
				break;
			case uilepra::InputManager::kInKbdMinus:
			case uilepra::InputManager::kInKbdNumpadMinus:
				speed_ = std::max(-40, speed_-5);
				break;
			} break;
		case 402:
		case 404:
			switch (event.key) {
			case uilepra::InputManager::kInKbdLeft:
				if (player_count_ == 2) {
					if (this->p2XV >= 0) break;
					this->p2XV = 0;
					break;
				}
				// TRICKY: fall thru.
			case 'a':
			case 'A':
				if (this->p1XV >= 0) break;
				this->p1XV = 0;
				break;

			case uilepra::InputManager::kInKbdRight:
				if (player_count_ == 2) {
					if (this->p2XV <= 0) break;
					this->p2XV = 0;
					break;
				}
				// TRICKY: fall thru.
			case 'd':
			case 'D':
				if (this->p1XV <= 0) break;
				this->p1XV = 0;
			} break;
		}

		return false;
	}

	public: void resetGame() {
		tinyReset();
		this->StartRun();
	}

	private: void tinyReset() {
		this->pressed_ = true;
		if (this->fInPlay)
			return;
		show_title_ = false;
		this->fEndGame = false;
		this->fInPlay = true;
		this->p1X = 200;
		this->p1Y = 0;
		this->p2X = 800;
		this->p2Y = 0;
		this->p1XV = 0;
		this->p1YV = 0;
		this->p2XV = 0;
		this->p2YV = 0;
		this->ballX = 200;
		this->ballY = 400;
		this->ballVX = 0;
		this->ballVY = 0;
		this->nPointsScored = 0;
		this->nScore = 5;
		ground_hit_x = 800;	// Opposing side lost it.
		fP1Fire = false;
		fP2Fire = false;
		resetTap();
		this->promptMsg = "";
		this->aiMode = (player_count_ == 2)? ai2PStartLevel : aiStartLevel;
		this->bGameOver = false;
		setAI();
		this->doRunGameThread = true;
		this->flash_ = false;
		this->scoringRun_ = 0;
		this->fP1Touches = 0;
		this->fP2Touches = 0;
		this->fP1TouchesTot = 0;
		this->fP2TouchesTot = 0;
		this->fServerMoved = false;
		this->fP1Touched = this->fP2Touched = 0;
	}

	public: void nextGameLevel() {
		if (this->fInPlay)
			return;
		const int prev_mode = this->aiMode;
		tinyReset();
		this->aiMode = prev_mode;
		if (this->aiMode < 5)
			this->aiMode += 1;
		setAI();
	}

	public: void retryGame() {
		if (!this->bGameOver)
			return;
		const int prev_mode = this->aiMode;
		tinyReset();
		this->aiMode = prev_mode;
		setAI();
	}

	private: void resetTap() {
		tapActive = false;
		p1TapX = 200;
		p1TapY = 0;
		p1TapJump = false;
		p2TapX = 800;
		p2TapY = 0;
		p2TapJump = false;
	}

	public: void moveP1Left() {
		this->p1XV = (this->fP1Fire ? -16 : -8);
		if ((this->p1X == 200) && (this->ballX == 200) && (0==this->fP2Touched) && (!this->fServerMoved))
			this->fServerMoved = true;
	}

	public: void moveP1Right() {
		this->p1XV = (this->fP1Fire ? 16 : 8);
		if ((this->p1X == 200) && (this->ballX == 200) && (0==this->fP2Touched) && (!this->fServerMoved))
			this->fServerMoved = true;
	}
	public: void moveP1Stop() {
		this->p1XV = 0; }
	public: void moveP1Jump() {
		if (this->p1Y == 0) this->p1YV = (this->fP1Fire ? 45 : 31);
	}

	public: void moveP2Left(int percent = 100) {
		this->p2XV = (this->fP2Fire ? -16 : -8);
		this->p2XV = this->p2XV * percent / 100;
		if ((this->p2X == 800) && (this->ballX == 800) && (0==this->fP1Touched) && (!this->fServerMoved))
			this->fServerMoved = true;
	}

	public: void moveP2Right(int percent = 100) {
		this->p2XV = (this->fP2Fire ? 16 : 8);
		this->p2XV = this->p2XV * percent / 100;
		if ((this->p2X == 800) && (this->ballX == 800) && (0==this->fP1Touched) && (!this->fServerMoved))
			this->fServerMoved = true;
	}
	public: void moveP2Stop() {
		this->p2XV = 0; }
	public: void moveP2Jump(int percent = 100) {
		if (this->p2Y == 0) {
			this->p2YV = (this->fP2Fire ? 45 : 31);
			this->p2XV = this->p2XV * percent / 100;
		}
	}

	private: void doAI() {
		int arrayOfInt[] = { this->ballX, this->ballY, this->ballVX, this->ballVY, this->p1X, this->p1Y, this->p1XV, this->p1YV, this->p2X, this->p2Y, this->p2XV, this->p2YV };
		this->ai->saveVars(arrayOfInt, this->fP1Fire, this->fP2Fire);
		this->ai->moveSlime();
	}

	private: void MoveSlimers() {
		if (player_count_ == 1) {
			doAI();
		}

		this->p1X += this->p1XV;
		if (this->p1X < 50)
			this->p1X = 50;
		if (this->p1X > 445)
			this->p1X = 445;
		if (this->p1YV != 0) {
			this->p1Y += this->p1YV -= (this->fP1Fire ? 4 : 2);
			if (this->p1Y < 0) {
				this->p1Y = 0;
				this->p1YV = 0;
			}
		}
		this->p2X += this->p2XV;
		if (this->p2X > 950)
			this->p2X = 950;
		if (this->p2X < 555)
			this->p2X = 555;
		if (this->p2YV != 0) {
			this->p2Y += this->p2YV -= (this->fP2Fire ? 4 : 2);
			if (this->p2Y < 0) {
				this->p2Y = 0;
				this->p2YV = 0;
			}
		}
	}

	private: void DrawSlimers() {
		this->flash_ = (!this->flash_);
		int i = this->nWidth / 10;
		int j = this->nHeight / 10;
		int k = this->nWidth / 50;
		int m = this->nHeight / 25;
		int n = this->ballX * this->nWidth / 1000;
		int i1 = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		int i2 = this->p1X * this->nWidth / 1000 - i / 2;
		int i3 = 7 * this->nHeight / 10 - this->p1Y * this->nHeight / 1000;
		this->screen.setColor((this->fP1Fire) && (this->flash_) ? this->activeSlimeColours[frenzyCol = ++frenzyCol % activeColCount] : this->activeSlimeColours[this->p1Col]);
		this->screen.fillArc(i2, i3, i, 2 * j, 0, 180);
		int i4 = this->p1X + 38;
		int i5 = this->p1Y - 60;
		i2 = i4 * this->nWidth / 1000;
		i3 = 7 * this->nHeight / 10 - i5 * this->nHeight / 1000;
		int i6 = i2 - n;
		int i7 = i3 - i1;
		int i8 = (int)Math::sqrt(i6 * i6 + i7 * i7);
		this->screen.setColor(WHITE);
		this->screen.fillOval(i2 - k, i3 - m, k, m);
		this->screen.setColor(BLACK);
		this->screen.fillOval(i2 - 4 * i6 / i8 - 3 * k / 4, i3 - 4 * i7 / i8 - 3 * m / 4, k / 2, m / 2);
		i2 = this->p2X * this->nWidth / 1000 - i / 2;
		i3 = 7 * this->nHeight / 10 - this->p2Y * this->nHeight / 1000;
		this->screen.setColor((this->fP2Fire) && (this->flash_) ? this->activeSlimeColours[frenzyCol = ++frenzyCol % activeColCount] : this->activeSlimeColours[this->p2Col]);
		this->screen.fillArc(i2, i3, i, 2 * j, 0, 180);
		i4 = this->p2X - 18;
		i5 = this->p2Y - 60;
		i2 = i4 * this->nWidth / 1000;
		i3 = 7 * this->nHeight / 10 - i5 * this->nHeight / 1000;
		i6 = i2 - n;
		i7 = i3 - i1;
		i8 = (int)Math::sqrt(i6 * i6 + i7 * i7);
		this->screen.setColor(WHITE);
		this->screen.fillOval(i2 - k, i3 - m, k, m);
		this->screen.setColor(BLACK);
		this->screen.fillOval(i2 - 4 * i6 / i8 - 3 * k / 4, i3 - 4 * i7 / i8 - 3 * m / 4, k / 2, m / 2);
		if ((!this->fP1Fire) && (!this->fP2Fire)) this->flash_ = false;

		if (this->nScore > 8) {
			int i10 = this->p1X * this->nWidth / 1000;
			int i11 = 7 * this->nHeight / 10 - (this->p1Y - 40) * this->nHeight / 1000;
			int i12 = this->nWidth / 20;
			int i13 = this->nHeight / 20;
			int i14 = 0;
			do {
				this->screen.setColor(DARK_GRAY);
				this->screen.drawArc(i10, i11 + i14, i12, i13, -30, -150);
				i14++; } while (i14 < 3);
			return;
		}
		if (this->nScore < 2) {
			int i10 = this->nWidth / 20;
			int i11 = this->nHeight / 20;
			int i12 = this->p2X * this->nWidth / 1000 - i10;
			int i13 = 7 * this->nHeight / 10 - (this->p2Y - 40) * this->nHeight / 1000;
			int i14 = 0;
			do {
				this->screen.setColor(DARK_GRAY);
				this->screen.drawArc(i12, i13 + i14, i10, i11, -10, -150);
				i14++; } while (i14 < 3);
		}
	}

	private: void MoveBall() {
		this->ballY += --this->ballVY;
		this->ballX += this->ballVX;
		if (!this->fEndGame) {
			int m = (this->ballX - this->p1X) * 2;
			int n = this->ballY - this->p1Y;
			int i1 = m * m + n * n;
			int i3 = this->ballVX - this->p1XV;
			int i4 = this->ballVY - this->p1YV;
			int i5;
			int i6;
			if ((n > 0) && (i1 < 15625) && (i1 > 25)) {
				i5 = (int)Math::sqrt(i1);
				i6 = (m * i3 + n * i4) / i5;
				this->ballX = (this->p1X + m * 63 / i5);
				this->ballY = (this->p1Y + n * 125 / i5);
				if (i6 <= 0) {
					this->ballVX += this->p1XV - 2 * m * i6 / i5;
					if (this->ballVX < -15)
						this->ballVX = -15;
					if (this->ballVX > 15)
						this->ballVX = 15;
					this->ballVY += this->p1YV - 2 * n * i6 / i5;
					if (this->ballVY < -22)
						this->ballVY = -22;
					if (this->ballVY > 22)
						this->ballVY = 22;
				}
				if (this->fServerMoved) {
					this->fP1Touched = 1;

					this->fP1Touches += 1;
					this->fP1TouchesTot += 1;
					this->fP2Touches = 0;
				}
			}
			m = (this->ballX - this->p2X) * 2;
			n = this->ballY - this->p2Y;
			int i2 = m * m + n * n;
			i3 = this->ballVX - this->p2XV;
			i4 = this->ballVY - this->p2YV;
			if ((n > 0) && (i2 < 15625) && (i2 > 25)) {
				i5 = (int)Math::sqrt(i2);
				i6 = (m * i3 + n * i4) / i5;
				this->ballX = (this->p2X + m * 63 / i5);
				this->ballY = (this->p2Y + n * 125 / i5);
				if (i6 <= 0) {
					this->ballVX += this->p2XV - 2 * m * i6 / i5;
					if (this->ballVX < -15)
						this->ballVX = -15;
					if (this->ballVX > 15)
						this->ballVX = 15;
					this->ballVY += this->p2YV - 2 * n * i6 / i5;
					if (this->ballVY < -22)
						this->ballVY = -22;
					if (this->ballVY > 22)
						this->ballVY = 22;
				}
				if (this->fServerMoved) {
					this->fP2Touched = 1;

					this->fP1Touches = 0;
					this->fP2Touches += 1;
					this->fP2TouchesTot += 1;
				}
			}

			if (this->ballX < 15) {
				this->ballX = 15;
				this->ballVX = (-this->ballVX);
			}

			if (this->ballX > 985) {
				this->ballX = 985;
				this->ballVX = (-this->ballVX);
			}

			if ((this->ballX > 480) && (this->ballX < 520) && (this->ballY < 140)) {
				if ((this->ballVY < 0) && (this->ballY > 130)) {
					this->ballVY *= -1;
					this->ballY = 130;
				} else if (this->ballX < 500) {
					this->ballX = 480;
					this->ballVX = (this->ballVX >= 0 ? -this->ballVX : this->ballVX);
				} else {
					this->ballX = 520;
					this->ballVX = (this->ballVX <= 0 ? -this->ballVX : this->ballVX);
				}
			}
		}
	}

	private: void DrawBall() {
		int i = 30 * this->nHeight / 1000;
		int j = this->ballX * this->nWidth / 1000;
		int k = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		this->screen.setColor(this->BALL_COL);
		this->screen.fillOval(j - i, k - i, i * 2, i * 2);
	}

	private: void drawScores() {
		Graphics graphics = this->screen;
		int i = this->nHeight / 20+2;
		for (int j = 0; j < this->nScore; j++) {
			int k = (j + 1) * this->nWidth / 24;
			graphics.setColor(this->activeSlimeColours[this->p1Col]);
			graphics.fillOval(k+2, 20+2, i-4, i-4);
			graphics.setColor(WHITE);
			graphics.drawOval(k, 20, i, i);
		}

		for (int k = 0; k < 10 - this->nScore; k++) {
			int m = this->nWidth - (k + 1) * this->nWidth / 24 - i;
			graphics.setColor(this->activeSlimeColours[this->p2Col]);
			graphics.fillOval(m+2, 20+2, i-4, i-4);
			graphics.setColor(WHITE);
			graphics.drawOval(m, 20, i, i);
		}
	}

	private: str MakeTime(long _long) {
		str s = "";

		long l1 = _long / 1000L % 60L;
		long l2 = _long / 60000L % 60L;

		s = strutil::IntToString(l2, 10) + ":";
		if (l1 < 10L) s += "0";
		s += strutil::IntToString(l1, 10);
		return s;
	}

	private: void DrawStatus() {
		str s = (!this->fInPlay ? "" : str("	 Time: ") + MakeTime((this->paused ? this->pausedTime : system::currentTimeMillis()) - this->time_));
		this->screen.setColor(WHITE);
		int i = this->nHeight*5/4 * 7/8 + 5;
		this->screen.centerString(s, i);
	}

	public: void drawPrompt() {
		drawPrompt(this->promptMsg, 0);
	}

	public: void drawPrompt(str _s, int _i) {
		FontMetrics font_metrics = this->screen.getFontMetrics();
		this->screen.setColor(WHITE);
		this->screen.centerString(_s, this->nHeight * 4 / 5 + font_metrics.getHeight() * (_i + 1) + 10);
	}

	public: void run() {
		activeSlimeColText = (player_count_ == 2)? human2PSlimeColText : col_text_;
		activeSlimeColours = (player_count_ == 2)? human2PSlimeColours : colours_;
		activeColCount = (player_count_ == 2)? LEPRA_ARRAY_COUNT(human2PSlimeColours) : LEPRA_ARRAY_COUNT(colours_);

		average_loop_time_ = lepra::Math::Lerp(average_loop_time_, loop_timer_.QueryTimeDiff(), 0.05);
		Thread::Sleep(0.0225 - speed_/1000.0 - average_loop_time_);
		loop_timer_.PopTimeDiff();

		if (tapActive) {
			doTapMove();
		}

		drawScores();
		drawPrompt();

		if (wait_run_timer_.QueryTimeDiff() < 0) {
			DrawSlimers();
			DrawBall();
			return;
		}

		if (was_frozen_) {
			was_frozen_ = false;
			this->promptMsg = "";
			resetTap();
			this->p1X = 200;
			this->p1Y = 0;
			this->p2X = 800;
			this->p2Y = 0;
			this->p1XV = 0;
			this->p1YV = 0;
			this->p2XV = 0;
			this->p2YV = 0;
			this->ballX = (ground_hit_x >= 500 ? 200 : 800);
			this->ballY = 400;
			this->ballVX = 0;
			this->ballVY = 0;
			this->fP1Touched = this->fP2Touched = 0;
			this->fServerMoved = false;
			//repaint();
			loop_timer_.PopTimeDiff();
			this->time_ += system::currentTimeMillis() - score_time;
		}

		if (this->doRunGameThread && !this->bGameOver) {
			if (!this->paused) {
				MoveSlimers();
				MoveBall();
			}
			DrawSlimers();
			DrawBall();
			DrawStatus();

			if (this->ballY < 35) {	// Uh-oh...
				score_time = system::currentTimeMillis();
				this->nPointsScored += 1;
				this->nScore += (this->ballX <= 500 ? -1 : 1);
				if ((this->ballX <= 500) && (this->scoringRun_ >= 0)) this->scoringRun_ += 1;
				else if ((this->ballX > 500) && (this->scoringRun_ <= 0)) this->scoringRun_ -= 1;
				else if ((this->ballX <= 500) && (this->scoringRun_ <= 0)) this->scoringRun_ = 1;
				else if ((this->ballX > 500) && (this->scoringRun_ >= 0)) this->scoringRun_ = -1;
				bool normal_score = false;
				this->promptMsg = (this->ballX <= 500 ? this->activeSlimeColText[this->p2Col] : this->activeSlimeColText[this->p1Col]);
				if ((!this->fP1Touched) && (!this->fP2Touched)) {
					this->promptMsg = "What can I say?";
				} else if ((this->scoringRun_ < 0 ? -this->scoringRun_ : this->scoringRun_) == 3) {
					this->promptMsg += "is on fire!";
				} else if (((this->ballX > 500) && (this->fP1Touched) && (!this->fP2Touched)) || ((this->ballX <= 500) && (!this->fP1Touched) && (this->fP2Touched))) {
					this->promptMsg += "aces the serve!";
				} else if (((this->ballX > 500) && (!this->fP1Touched) && (this->fP2Touched)) || ((this->ballX <= 500) && (this->fP1Touched) && (!this->fP2Touched)))
					this->promptMsg += "dies laughing! :P";
				else
					normal_score = true;
				if (normal_score || nScore == 0 || nScore == 10) {
					this->promptMsg = (this->ballX <= 500 ? this->activeSlimeColText[this->p2Col] : this->activeSlimeColText[this->p1Col]);
					switch (this->nScore) {
					case 0:
					case 10:
						this->promptMsg = (this->ballX <= 500 ? this->activeSlimeColText[this->p2Col] : this->activeSlimeColText[this->p1Col]);
						if (this->nPointsScored == 5)
							this->promptMsg += "wins with a QUICK FIVE!!!";
						else if (this->scoringRun_ == 8)
							this->promptMsg += "wins with a BIG NINE!!!";
						else
							this->promptMsg += "wins!!!";
						finishGame();
						return;
					case 4:
						this->promptMsg += (this->ballX >= 500 ? "scores!" : "takes the lead!!");
						break;
					case 6:
						this->promptMsg += (this->ballX <= 500 ? "scores!" : "takes the lead!!");
						break;
					case 5:
						this->promptMsg += "equalizes!";
						break;
					default:
						this->promptMsg += "scores!";
						break;
					}
				}

				if (this->scoringRun_ <= -3) {
					fP1Fire = true;
				} else if (this->scoringRun_ >= 3 && player_count_ == 2) {
					fP2Fire = true;
				} else {
					fP1Fire = false;
					if (player_count_ == 2) {
						fP2Fire = false;
					}
				}

				//this->fCanChangeCol = false;
				ground_hit_x = this->ballX;

				this->pressed_ = false;
				wait_run_timer_.PopTimeDiff();
				wait_run_timer_.ReduceTimeDiff(1.5);
				was_frozen_ = true;
			}
		}
	}

	private: void finishGame() {
		if (player_count_ == 1) {
			if (nScore == 10) {
				this->gameTime = (system::currentTimeMillis() - this->time_);
				/*if (this->fP1PointsWon == 6) {
					this->gameScore += 1000 * this->fP1PointsWon / (this->fP1PointsWon + this->fP2PointsWon) * scale();

					this->gameScore = (int)(this->gameScore + (this->gameTime < 300000L ? 300000L - this->gameTime : 0L) / 1000L * scale());
				}
				if (this->fP2PointsWon == 0)
					this->gameScore += 1000 * scale();*/
				if (this->aiMode == 5) {
					this->aiMode = 6;
					gameOver(true);
				}
			} else {
				gameOver(false);
			}
		}
		this->fInPlay = false;
		this->doRunGameThread = false;
		this->fEndGame = true;
		wait_run_timer_.PopTimeDiff();
		wait_run_timer_.ReduceTimeDiff(3.5);
		was_frozen_ = true;
	}

	public: void ShowTitle() {
		show_title_ = true;
	}

	private: void gameOver(bool fDidWin) {
		drawScores();
		DrawStatus();
		if (!fDidWin) {
			promptMsg += "\n" + this->text1[this->aiMode] + "\n" + this->text2[this->aiMode] + "\nGAME OVER";
		} else {
			fatality(this->screen);
			promptMsg += "\nYOU WIN!\nThe Slimes bow down before the new Slime King!";
		}

		this->bGameOver = true;
	}

	private: void fatality(const Graphics& /*graphics*/) {
	}

	private: int max(int int1, int int2) {
		if (int1 > int2) return int1;
		return int2;
	}

	private: void sleep(long _long, bool /*paramBoolean*/) {
		if (this->doRunGameThread)
			Thread::Sleep(_long*0.001);
	}

	private: void StartRun() {
		this->time_ = system::currentTimeMillis();
		score_time = this->time_;
	}

	private: void drawTapIndicators() {
#ifdef LEPRA_TOUCH
		const int dy = screen.height*2/10;
		const int y = screen.height - std::min(std::max(dy, 120), 60);
		const int h = screen.height/3;
		const int jh = h * 3/2;
		const int yt = y - h;
		const int jyt = y - jh;
		const bool p1jmp = (p1Y > 0);
		const bool p2jmp = (p2Y > 0);
		screen.setColor(p1jmp? RED : GREEN);
		screen.fillRect(xformx(50), y-1, xformx(400), 3);
		if (player_count_ == 2) {
			screen.setColor(p2jmp? RED : GREEN);
			screen.fillRect(xformx(550), y-1, xformx(400), 3);
		}
		if (player_count_ == 1) {
			const int dx = screen.width*2/10;
			const int x = screen.width - std::min(std::max(dx, 120), 60);
			const float a = system::currentTimeMillis() * 0.005f;
			screen.setColor(Color(sin(a)*20+50, -sin(a)*15+50, 0));
			screen.fillOval(x-15, y-15, 30, 30);
		}
		if (!tapActive) {
			return;
		}
		drawSpike(xformx(p1TapX), p1jmp? jyt : yt, p1jmp? jh : h, p1jmp? RED : GREEN);
		if (player_count_ == 2) {
			drawSpike(xformx(p2TapX), p2jmp? jyt : yt, p2jmp? jh : h, p2jmp? RED : GREEN);
		}
#endif // iOS
	}

	private: int xformx(int x) {
		return x*screen.width/1000;
	}

	private: int xformy(int y) {
		return (1000-y)*screen.height/1000;
	}

	private: void drawSpike(int x, int y, int h, const Color c) {
		GLubyte C[] = {c.red_, c.green_, c.blue_, 100,
			       c.red_, c.green_, c.blue_, 20,
			       c.red_, c.green_, c.blue_, 20,
			       c.red_, c.green_, c.blue_, 255,
			       c.red_, c.green_, c.blue_, 255,
			       c.red_, c.green_, c.blue_, 20,};
		GLfloat fx = (GLfloat)x;
		GLfloat fy = (GLfloat)y;
		GLfloat fh = (GLfloat)h;
		GLfloat v[] = {fx, (fy+fh)*0.5f, fx-1, fy, fx+1, fy, fx+1, fy+fh, fx-1, fy+fh, fx-1, fy};
		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		::glEnableClientState(GL_COLOR_ARRAY);
		::glColorPointer(4, GL_UNSIGNED_BYTE, 0, C);
		::glVertexPointer(2, GL_FLOAT, 0, v);
		::glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		::glDisableClientState(GL_COLOR_ARRAY);
		::glDisable(GL_BLEND);
	}

	public: void MoveTo(const FingerMovement& move) {
		tapActive = true;
		const int ox = (int)(move.start_y_*1000/screen.width);
		//const int oy = (int)(move.start_x_*1000/screen.height);
		int ix = (int)(move.last_y_*1000/screen.width);
		const int iy = (int)(move.last_x_*1000/screen.height);
		if (player_count_ == 1) {
			if (ox < 600) {
				p1TapX = std::min(ix, 500);
				p1TapY = iy;
			} else if (ox > 600) {
				if (move.is_press_ && !p1TapJump) {
					moveP1Jump();
				}
				p1TapJump = move.is_press_;
			}
		} else {
			if (ox < 500) {	// P1.
				ix = std::min(500, ix);
				const int fingers = countStartPos(true, screen.width);
				if (fingers >= 2) {
					if (!p1TapJump) {
						moveP1Jump();
					}
				} else {
					p1TapJump = false;
				}
				if (fingers == 1 || std::abs(ix-p1TapX) < 15) {
					p1TapX = ix;
					p1TapY = iy;
				}
			}
			if (ox > 500) {	// P2
				ix = std::max(500, ix);
				const int fingers = countStartPos(false, screen.width);
				if (fingers >= 2) {
					if (!p2TapJump) {
						moveP2Jump();
					}
				} else {
					p2TapJump = false;
				}
				if (fingers == 1 || std::abs(ix-p2TapX) < 15) {
					p2TapX = ix;
					p2TapY = iy;
				}
			}
		}
	}

	public: void doTapMove() {
		{
			const int distance = fP1Fire? 16 : 8;
			const int dx = p1TapX-p1X;
			if (dx <= -distance)		p1XV = -distance;
			else if (dx >= distance)	p1XV = distance;
			else				p1XV = dx;
		}
		if (player_count_ == 2) {
			const int distance = fP2Fire? 16 : 8;
			const int dx = p2TapX-p2X;
			if (dx <= -distance)		p2XV = -distance;
			else if (dx >= distance)	p2XV = distance;
			else				p2XV = dx;
		}

	}

	public: bool canContinue() {
		return (player_count_ == 1 && aiMode <= 5);
	}
};



}
