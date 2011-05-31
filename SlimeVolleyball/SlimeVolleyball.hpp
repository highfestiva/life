#pragma once
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/HiResTimer.h"
#include "../Lepra/Include/Thread.h"
#include "../UiLepra/Include/UiInput.h"
#include "SlimeAI.hpp"
#include "CrapAI.hpp"
#include "DannoAI.hpp"
#include "DannoAI2.hpp"



namespace Slime
{



class SlimeVolleyball
{
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
	private: int ballX;
	private: int ballY;
	private: int ballVX;
	private: int ballVY;
	private: Graphics screen;
	private: str promptMsg;
	private: bool mousePressed;
	private: bool fInPlay;
	private: bool fP1Fire;
	private: bool fP2Fire;
	private: bool superFlash;
	private: int fP1Touched;
	private: int fP2Touched;
	private: int fP1Touches;
	private: int fP2Touches;
	private: int fP1TouchesTot;
	private: int fP2TouchesTot;
	private: int fP1Clangers;
	private: int fP2Clangers;
	private: int fP1Aces;
	private: int fP2Aces;
	private: int fP1Winners;
	private: int fP2Winners;
	private: int fP1PointsWon;
	private: int fP2PointsWon;
	private: bool fP1HitStill;
	private: bool fP2HitStill;
	private: long fP1Frames;
	private: long fP2Frames;
	private: int fP1Super;
	private: int fP2Super;
	private: bool fServerMoved;
	private: bool hitNetSinceTouched;
	private: bool doRunGameThread;
	private: bool fEndGame;
	private: long startTime;
	private: long gameTime;
	private: long crossedNetTime;
	private: long pausedTime;
	private: bool paused;
	private: int scoringRun;
	private: str slimeColText[6];
	private: Color slimeColours[6];
	private: str loserText1[5];
	private: str loserText2[5];
	private: Color SKY_COL;
	private: Color COURT_COL;
	private: Color BALL_COL;
	private: static const int pointsToWin = 6;
	private: int _run_j;
	private: int _run_k;
	private: int _run_m;
	private: long _run_l;
	private: int mSpeed;
	private: HiResTimer mWaitRunTimer;
	private: double mAverageLoopTime;
	private: HiResTimer mLoopTimer;
	private: bool mWasFrozen;
	private: int aiMode;
	private: SlimeAI* ai;
	private: int gameScore;
	private: bool bGameOver;

	private: static const int aiStartLevel = 0;

	public: SlimeVolleyball()
	{
		CLEAR_MEMBERS(nWidth, bGameOver);
	}

	public: bool init(const Graphics& pGraphics)
	{
		System::out::println("One Slime: http://www.student.uwa.edu.au/~wedgey/slime1/");

		this->screen = pGraphics;
		this->nWidth = this->screen.width;
		this->nHeight = this->screen.height;
		this->fInPlay = this->fEndGame = false;
		this->promptMsg = "Click the mouse to play!";
		str s[] = { "Inferior Human Controlled Slime ", "The Pathetic White Slime ", "Angry Red Slimonds ", "The Slime Master ", "Psycho Slime ", "Psycho Slime " };
		LEPRA_ARRAY_ASSIGN(this->slimeColText, s);
		Color c[] = { YELLOW, WHITE, RED, BLACK, BLUE, BLUE };
		LEPRA_ARRAY_ASSIGN(this->slimeColours, c);
		str s2[] = { "You are a loser!", this->slimeColText[2] + "gives you the gong!", this->slimeColText[3] + "says \"You are seriously inept.\"", this->slimeColText[4] + "laughs at the pathetic slow opposition.", this->slimeColText[5] + "is still invincible!" };
		LEPRA_ARRAY_ASSIGN(this->loserText1, s2);
		str s3[] = { "Better luck next time.", "So who has the red face bombing out on level 2, huh?", "Congrats on reaching level 3.", "Congrats on reaching level 4!", "You fell at the last hurdle... but get up and try again!" };
		LEPRA_ARRAY_ASSIGN(this->loserText2, s3);
		this->p1Col = 0;
		this->gameScore = 0;
		this->bGameOver = true;
		this->paused = false;

		this->aiMode = aiStartLevel;

		setAI();
		//repaint();
		return true;
	}

	private: void setAI()
	{
		this->fP1PointsWon = 0;
		this->fP2PointsWon = 0;
		delete this->ai;
		switch (this->aiMode)
		{
		case 0:
		default:
			this->ai = new CrapAI();
			this->fP2Fire = false;
			this->SKY_COL = BLUE;
			this->COURT_COL = GRAY;
			this->BALL_COL = YELLOW;
			break;
		case 1:
			this->ai = new DannoAI();
			this->fP2Fire = false;
			this->SKY_COL = Color(30, 80, 0);
			this->COURT_COL = DARK_GRAY;
			this->BALL_COL = Color(128, 128, 255);
			break;
		case 2:
			this->ai = new DannoAI2();
			this->fP2Fire = false;
			this->SKY_COL = Color(98, 57, 57);
			this->COURT_COL = Color(0, 168, 0);
			this->BALL_COL = WHITE;
			break;
		case 3:
			this->ai = new DannoAI2();
			this->fP2Fire = true;
			this->SKY_COL = BLACK;
			this->COURT_COL = RED;
			this->BALL_COL = YELLOW;
			break;
		case 4:
			this->ai = new DannoAI2();
			this->fP2Fire = true;
			this->SKY_COL = BLACK;
			this->COURT_COL = RED;
			this->BALL_COL = YELLOW;
			this->fP2PointsWon = 5;
			break;
		}
		this->p2Col = (this->aiMode + 1);
		this->ai->init(this, 2);
	}

	public: void paint(Graphics paramGraphics)
	{
		this->screen = paramGraphics;
		this->nWidth = paramGraphics.width;
		this->nHeight = paramGraphics.height;
		paramGraphics.setColor(this->SKY_COL);
		paramGraphics.fillRect(0, 0, this->nWidth, 4 * this->nHeight / 5);
		paramGraphics.setColor(this->COURT_COL);
		paramGraphics.fillRect(0, 4 * this->nHeight / 5, this->nWidth, this->nHeight / 5);
		paramGraphics.setColor(WHITE);
		paramGraphics.fillRect(this->nWidth / 2 - 2, 7 * this->nHeight / 10, 4, this->nHeight / 10 + 5);
		FontMetrics localFontMetrics = paramGraphics.getFontMetrics();
		if (this->bGameOver)
		{
			this->screen.centerString("Slime Volleyball: One Slime", this->nHeight / 2);
			paramGraphics.centerString("Code base by Quin Pendragon", this->nHeight / 2 + localFontMetrics.getHeight() * 2);
			paramGraphics.centerString("AI and Mod by Daniel Wedge", this->nHeight / 2 + localFontMetrics.getHeight() * 3);
			paramGraphics.centerString("Port by high_festiva", this->nHeight / 2 + localFontMetrics.getHeight() * 4);
			paramGraphics.setColor(LIGHT_RED);
			paramGraphics.centerString("Latest version is at pixeldoctrine.com", this->nHeight / 2 + localFontMetrics.getHeight() * 11 / 2);
			if (this->aiMode != 0)
				this->promptMsg = "Click the mouse to play or press C to continue...";
			else
				this->promptMsg = "Click the mouse to play!";
			drawScores();
			DrawStatus();
			drawPrompt();
			this->promptMsg = "";

			return;
		}

		if (!this->fInPlay)
		{
			paramGraphics.setColor(WHITE);
			str s1 = str("Level ") + strutil::IntToString(this->aiMode + 1, 10) + " clear!";
			this->screen.centerString(s1, this->nHeight / 3);
			str s2 = str("Your score: ") + strutil::IntToString(this->gameScore, 10);
			paramGraphics.centerString(s2, this->nHeight / 2);
			if (this->fP1PointsWon == 6)
			{
				str s3 = str("Level bonus: ") + strutil::IntToString(1000 * this->fP1PointsWon / (this->fP1PointsWon + this->fP2PointsWon) * scale(), 10) + " points";
				paramGraphics.centerString(s3, this->nHeight / 2 + localFontMetrics.getHeight());
				str s4 = str("Time bonus: ") + strutil::IntToString((this->gameTime < 300000L ? 300000L - this->gameTime : 0L) / 1000L * scale(), 10) + " points";
				paramGraphics.centerString(s4, this->nHeight / 2 + localFontMetrics.getHeight() * 2);
				if (this->fP2PointsWon == 0)
				{
					str s5 = str("Flawless Victory: ") + strutil::IntToString(1000 * scale(), 10) + " points";
					paramGraphics.centerString(s5, this->nHeight / 2 + localFontMetrics.getHeight() * 3);
				}
			}
			this->promptMsg = "Click the mouse to continue...";
			drawPrompt();
			this->promptMsg = "";
			drawScores();

			return;
		}

		drawScores();
	}

	public: bool handleEvent(Event paramEvent)
	{
		switch (paramEvent.id)
		{
		case 503:
			//showStatus("Slime Volleyball: One Slime: http://www.student.uwa.edu.au/~wedgey/");
			break;
		case 501:
			this->mousePressed = true;
			if (this->fInPlay)
				break;
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
			this->hitNetSinceTouched = false;
			this->promptMsg = "";
			if ((this->gameScore != 0) && (this->aiMode < 4))
				this->aiMode += 1;
			if (this->bGameOver)
			{
				this->aiMode = aiStartLevel;
				this->bGameOver = false;
				this->gameScore = 0;
			}
			setAI();
			//repaint();
			this->StartRun();
			break;

		case 401:
		case 403:
			switch (paramEvent.key)
			{
			case 'a':
			case 'A':
			case UiLepra::InputManager::IN_KBD_LEFT:
				moveP1Left();
				break;
			case 'd':
			case 'D':
			case UiLepra::InputManager::IN_KBD_RIGHT:
				moveP1Right();
				break;
			case 'w':
			case 'W':
			case UiLepra::InputManager::IN_KBD_UP:
				moveP1Jump();
				break;
			case 'c':
			case 'C':
				if (!this->bGameOver)
					break;
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
				this->hitNetSinceTouched = false;
				this->promptMsg = "";
				this->bGameOver = false;
				this->gameScore = 0;
				setAI();
				//repaint();
				this->StartRun();
				break;

			case 'p':
			case 'P':
				if (!this->paused)
				{
					this->pausedTime = System::currentTimeMillis();
					this->paused = true;
				}
				else {
					this->startTime += System::currentTimeMillis() - this->pausedTime;
					this->paused = false;
				}
				break;
			case UiLepra::InputManager::IN_KBD_PLUS:
			case UiLepra::InputManager::IN_KBD_NUMPAD_PLUS:
				mSpeed = 0;
				break;
			case UiLepra::InputManager::IN_KBD_MINUS:
			case UiLepra::InputManager::IN_KBD_NUMPAD_MINUS:
				mSpeed = std::max(-40, mSpeed-5);
				break;
			}
			break;
		case 402:
		case 404:
			switch (paramEvent.key)
			{
			case 'a':
			case 'A':
			case UiLepra::InputManager::IN_KBD_LEFT:
				if (this->p1XV >= 0) break;
				this->p1XV = 0;
				break;
			case 'd':
			case 'D':
			case UiLepra::InputManager::IN_KBD_RIGHT:
				if (this->p1XV <= 0) break;
				this->p1XV = 0;
			}break;
		}

		return false;
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
	public: void moveP1Jump() { if (this->p1Y == 0) this->p1YV = (this->fP1Fire ? 45 : 31);	}

	public: void moveP2Left()
	{
		this->p2XV = (this->fP2Fire ? -16 : -8);
		if ((this->p2X == 800) && (this->ballX == 800) && (0==this->fP1Touched) && (!this->fServerMoved))
			this->fServerMoved = true; 
	}

	public: void moveP2Right() {
		this->p2XV = (this->fP2Fire ? 16 : 8);
		if ((this->p2X == 800) && (this->ballX == 800) && (0==this->fP1Touched) && (!this->fServerMoved))
			this->fServerMoved = true; 
	}
	public: void moveP2Stop() {
		this->p2XV = 0; } 
	public: void moveP2Jump() { if (this->p2Y == 0) this->p2YV = (this->fP2Fire ? 45 : 31); 
	}

	private: void doAI()
	{
		int arrayOfInt[] = { this->ballX, this->ballY, this->ballVX, this->ballVY, this->p1X, this->p1Y, this->p1XV, this->p1YV, this->p2X, this->p2Y, this->p2XV, this->p2YV };
		this->ai->saveVars(arrayOfInt, this->fP1Fire, this->fP2Fire);
		this->ai->moveSlime();
	}

	private: void MoveSlimers()
	{
		doAI();
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
		this->superFlash = (!this->superFlash);
		int i = this->nWidth / 10;
		int j = this->nHeight / 10;
		int k = this->nWidth / 50;
		int m = this->nHeight / 25;
		int n = this->ballX * this->nWidth / 1000;
		int i1 = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		int i2 = this->p1X * this->nWidth / 1000 - i / 2;
		int i3 = 7 * this->nHeight / 10 - this->p1Y * this->nHeight / 1000;
		this->screen.setColor((this->fP1Fire) && (this->superFlash) ? WHITE : this->slimeColours[this->p1Col]);
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
		this->screen.setColor((this->fP2Fire) && (this->superFlash) ? WHITE : this->slimeColours[this->p2Col]);
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
		if ((!this->fP1Fire) && (!this->fP2Fire)) this->superFlash = false; 
	}

	private: void MoveBall()
	{
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
					this->fP1HitStill = ((this->p1YV == 0) && (this->p1XV == 0));
					this->hitNetSinceTouched = false;
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
					this->fP2HitStill = ((this->p2YV == 0) && (this->p2XV == 0));
					this->hitNetSinceTouched = false;
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

			if ((this->ballX > 480) && (this->ballX < 520) && (this->ballY < 140))
			{
				if ((this->ballVY < 0) && (this->ballY > 130)) {
					this->ballVY *= -1;
					this->ballY = 130;
				}
				else if (this->ballX < 500) {
					this->ballX = 480;
					this->ballVX = (this->ballVX >= 0 ? -this->ballVX : this->ballVX);
					this->hitNetSinceTouched = true;
				} else {
					this->ballX = 520;
					this->ballVX = (this->ballVX <= 0 ? -this->ballVX : this->ballVX);
					this->hitNetSinceTouched = true;
				}
			}
		}
	}

	private: void DrawBall()
	{
		int i = 30 * this->nHeight / 1000;
		int j = this->ballX * this->nWidth / 1000;
		int k = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		this->screen.setColor(this->BALL_COL);
		this->screen.fillOval(j - i, k - i, i * 2, i * 2);
	}

	private: void drawScores() {
		int i = this->nHeight / 15;
		int j = 20;

		for (int k = 0; k < 6; k++)
		{
			if (this->fP1PointsWon >= k + 1)
			{
				this->screen.setColor(this->slimeColours[this->p1Col]);
				this->screen.fillOval(j+2, i*1/5+2, i-4, i-4);
			}
			this->screen.setColor(WHITE);
			this->screen.drawOval(j, i*1/5, i, i);
			j += i + 10;
		}

		j = this->nWidth - 20 - 6 * (i + 10);
		for (int m = 0; m < 6; m++)
		{
			if (this->fP2PointsWon >= 6 - m)
			{
				this->screen.setColor(this->slimeColours[this->p2Col]);
				this->screen.fillOval(j+2, i*1/5+2, i-4, i-4);
			}
			this->screen.setColor(WHITE);
			this->screen.drawOval(j, i*1/5, i, i);
			j += i + 10;
		}
	}

	private: str MakeTime(long paramLong)
	{
		str str = "";

		long l1 = paramLong / 1000L % 60L;
		long l2 = paramLong / 60000L % 60L;

		str = str + strutil::IntToString(l2, 10) + ":";
		if (l1 < 10L) str = str + "0";
		str = str + strutil::IntToString(l1, 10);
		return str;
	}

	private: void DrawStatus()
	{
		str s = str("Score: ") + strutil::IntToString(this->gameScore, 10) + (!this->fInPlay ? "" : str("	 Time: ") + MakeTime((this->paused ? this->pausedTime : System::currentTimeMillis()) - this->startTime));
		this->screen.setColor(WHITE);
		int i = this->nHeight / 15;
		this->screen.centerString(s, i/5 + i/2);
	}

	public: void drawPrompt() {
		drawPrompt(this->promptMsg, 0);
	}

	public: void drawPrompt(str paramString, int paramInt)
	{
		FontMetrics localFontMetrics = this->screen.getFontMetrics();
		this->screen.setColor(WHITE);
		this->screen.centerString(paramString, this->nHeight * 4 / 5 + localFontMetrics.getHeight() * (paramInt + 1) + 10);
	}

	public: void run()
	{
		mAverageLoopTime = Lepra::Math::Lerp(mAverageLoopTime, mLoopTimer.QueryTimeDiff(), 0.05);
		Thread::Sleep(0.02 - mSpeed/1000.0 - mAverageLoopTime);
		mLoopTimer.PopTimeDiff();

		drawScores();
		drawPrompt();

		if (mWaitRunTimer.QueryTimeDiff() < 0)
		{
			DrawSlimers();
			DrawBall();
			return;
		}

		if (mWasFrozen)
		{
			mWasFrozen = false;
			this->promptMsg = "";
			if ((this->fP1PointsWon == 6) || (this->fP2PointsWon == 6))
				finishGame();
			this->p1X = 200;
			this->p1Y = 0;
			this->p2X = 800;
			this->p2Y = 0;
			this->p1XV = 0;
			this->p1YV = 0;
			this->p2XV = 0;
			this->p2YV = 0;
			this->ballX = (_run_m >= 500 ? 200 : 800);
			this->ballY = 400;
			this->ballVX = 0;
			this->ballVY = 0;
			this->fP1Touched = this->fP2Touched = 0;
			this->fServerMoved = false;
			//repaint();
			this->startTime += System::currentTimeMillis() - _run_l;
		}

		if (this->doRunGameThread && !this->bGameOver)
		{
			if (!this->paused)
			{
				MoveSlimers();
				MoveBall();
			}
			DrawSlimers();
			DrawBall();
			DrawStatus();

			if (this->ballY < 35)	// Uh-oh...
			{
				_run_l = System::currentTimeMillis();
				if (this->ballX > 500)
					this->fP1PointsWon += 1;
				else {
					this->fP2PointsWon += 1;
				}

				if ((this->ballX <= 500) && ((this->fP1Touches >= 3) || ((this->hitNetSinceTouched) && (this->fP1Touches > 0)) || (0==this->fP2Touched) || (
					(this->fP1HitStill) && (this->fP1Touches > 0)))) {
					this->fP1Clangers += 1;
				}
				else if ((this->ballX > 500) && ((this->fP2Touches >= 3) || ((this->hitNetSinceTouched) && (this->fP2Touches > 0)) || (0==this->fP1Touched) || (
					(this->fP2HitStill) && (this->fP2Touches > 0)))) {
					this->fP2Clangers += 1;
				}

				if ((0!=this->fP1Touched) && (0==this->fP2Touched) && (this->ballX >= 500)) {
					this->fP1Aces += 1;
					_run_j = 1;
					this->gameScore += 200 * scale();
				}
				else if ((0!=this->fP2Touched) && (0==this->fP1Touched) && (this->ballX < 500)) {
					this->fP2Aces += 1;
					_run_j = 1;
				}
				else if ((this->ballX > 500) && (this->fP1Touches > 0)) {
					this->fP1Winners += 1;
					_run_k = 1;
					this->gameScore += 100 * scale();
				}
				else if ((this->ballX <= 500) && (this->fP2Touches > 0)) {
					this->fP2Winners += 1;
					_run_k = 1;
				}

				if ((this->ballX > 500) && (_run_k == 0) && (_run_j == 0)) {
					this->gameScore += 50 * scale();
				}

				this->promptMsg = (this->ballX <= 500 ? this->slimeColText[this->p2Col] : this->slimeColText[this->p1Col]);

				if ((this->fP1PointsWon == 6) || (this->fP2PointsWon == 6))
					this->promptMsg += "wins!";
				else if (_run_j != 0)
					this->promptMsg += "aces the serve!";
				else if (_run_k != 0)
					this->promptMsg += "scores a winner!";
				else if (((this->ballX > 500) && (0==this->fP1Touched) && (0!=this->fP2Touched)) || ((this->ballX <= 500) && (0!=this->fP1Touched) && (0==this->fP2Touched)))
					this->promptMsg += "laughs at his opponent's inability to serve!";
				else if (this->fP1PointsWon == this->fP2PointsWon)
					this->promptMsg += "draws level!";
				else if (((this->ballX > 500) && (this->fP1PointsWon == this->fP2PointsWon + 1)) || (
					(this->ballX <= 500) && (this->fP1PointsWon + 1 == this->fP2PointsWon)))
					this->promptMsg += "takes the lead!";
				else
					this->promptMsg += "scores!";
				_run_m = this->ballX;

				_run_j = 0;
				_run_k = 0;
				this->mousePressed = false;
				mWaitRunTimer.PopTimeDiff();
				mWaitRunTimer.ReduceTimeDiff(1.5);
				mWasFrozen = true;
			}
		}
	}

	private: void finishGame()
	{
		if (this->fP1PointsWon == 6)
		{
			this->gameTime = (System::currentTimeMillis() - this->startTime);
			if (this->fP1PointsWon == 6)
			{
				this->gameScore += 1000 * this->fP1PointsWon / (this->fP1PointsWon + this->fP2PointsWon) * scale();

				this->gameScore = (int)(this->gameScore + (this->gameTime < 300000L ? 300000L - this->gameTime : 0L) / 1000L * scale());
			}
			if (this->fP2PointsWon == 0)
				this->gameScore += 1000 * scale();
			if (this->aiMode == 4)
			{
				this->aiMode = 5;
				gameOver(true);
			}
		} else {
			gameOver(false);
		}this->fInPlay = false;
		this->StopRun();
	}

	private: void gameOver(bool paramBoolean)
	{
		FontMetrics localFontMetrics1 = this->screen.getFontMetrics();
		drawScores();
		DrawStatus();
		FontMetrics localFontMetrics2 = this->screen.getFontMetrics();
		if (!paramBoolean)
		{
			this->screen.setColor(this->COURT_COL);
			this->screen.fillRect((this->nWidth - max(localFontMetrics2.stringWidth(this->loserText1[this->aiMode]), localFontMetrics2.stringWidth(this->loserText2[this->aiMode]))) / 2 - 30, this->nHeight / 2 - localFontMetrics2.getAscent() * 5, max(localFontMetrics2.stringWidth(this->loserText1[this->aiMode]), localFontMetrics2.stringWidth(this->loserText2[this->aiMode])) + 60, localFontMetrics2.getAscent() * 5 + localFontMetrics1.getAscent() * 2);
			this->screen.setColor(WHITE);
			this->screen.centerString(this->loserText1[this->aiMode], this->nHeight / 2 - localFontMetrics2.getAscent() * 3);
			this->screen.centerString(this->loserText2[this->aiMode], this->nHeight / 2 - localFontMetrics2.getAscent() * 2);
			this->screen.centerString("GAME OVER", this->nHeight / 2 + localFontMetrics1.getAscent());
		}
		else
		{
			fatality(this->screen);
			this->screen.setColor(WHITE);
			this->screen.centerString("YOU WIN!", this->nHeight / 2);
			this->screen.centerString("The Slimes bow down before the new Slime King!", this->nHeight / 2 + localFontMetrics2.getAscent());
		}

		mWaitRunTimer.PopTimeDiff();
		mWaitRunTimer.ReduceTimeDiff(3.0);
		this->bGameOver = true;
	}

	private: void fatality(Graphics /*paramGraphics*/)
	{
	}

	private: int max(int paramInt1, int paramInt2)
	{
		if (paramInt1 > paramInt2) return paramInt1;
		return paramInt2;
	}

	private: void sleep(long paramLong, bool /*paramBoolean*/) {
		if (this->doRunGameThread)
			Thread::Sleep(paramLong*0.001);
	}

	private: int scale() {
		return (int)Math::pow(2.0, this->aiMode);
	}
	public: void destroy() {
		if (this->doRunGameThread) {
			StopRun();
		}
	}
	private: void StartRun()
	{
		this->doRunGameThread = true;
		drawPrompt();
		this->superFlash = false;
		this->scoringRun = 0;
		this->fP1Touches = 0;
		this->fP2Touches = 0;
		this->fP1TouchesTot = 0;
		this->fP2TouchesTot = 0;
		this->fP1Clangers = 0;
		this->fP2Clangers = 0;
		this->fP1Aces = 0;
		this->fP2Aces = 0;
		this->fP1Winners = 0;
		this->fP2Winners = 0;
		this->fP1Frames = 0L;
		this->fP2Frames = 0L;
		this->fP1Super = 0;
		this->fP2Super = 0;
		this->fP1HitStill = false;
		this->fP2HitStill = false;
		this->fServerMoved = false;
		drawScores();
		this->fP1Touched = this->fP2Touched = 0;
		this->hitNetSinceTouched = false;
		this->bGameOver = false;
		this->startTime = System::currentTimeMillis();
		_run_j = 0;
		_run_k = 0;
	}
	private: void StopRun()
	{
		this->doRunGameThread = false;
		this->fEndGame = true;
		this->fInPlay = false;
		this->promptMsg = "";
		//repaint();
	}
};



}
