#pragma once
#include "../Lepra/Include/MemberThread.h"
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
	private: int p1OldX;
	private: int p1OldY;
	private: int p2OldX;
	private: int p2OldY;
	private: int p1XV;
	private: int p1YV;
	private: int p2XV;
	private: int p2YV;
	private: int ballX;
	private: int ballY;
	private: int ballVX;
	private: int ballVY;
	private: int ballOldX;
	private: int ballOldY;
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
	private: MemberThread<SlimeVolleyball>* gameThread;
	private: bool fEndGame;
	private: long startTime;
	private: long gameTime;
	private: long crossedNetTime;
	private: long pausedTime;
	private: bool paused;
	private: int scoringRun;
	private: int oldScoringRun;
	private: str slimeColText[6];
	private: Color slimeColours[6];
	private: str loserText1[5];
	private: str loserText2[5];
	private: Color SKY_COL;
	private: Color COURT_COL;
	private: Color BALL_COL;
	private: static const int pointsToWin = 6;
	private: int aiMode;
	private: SlimeAI* ai;
	private: int gameScore;
	private: bool bGameOver;

	private: static const int aiStartLevel = 0;

	public: SlimeVolleyball()
	{
		CLEAR_MEMBERS(nWidth, bGameOver);
	}

	public: bool init()
	{
		System::out::println("One Slime: http://www.student.uwa.edu.au/~wedgey/slime1/");

		this->screen = Graphics();
		this->nWidth = this->screen.width;
		this->nHeight = this->screen.height;
		this->fInPlay = this->fEndGame = false;
		this->promptMsg = "Click the mouse to play!";
		//this->screen.setFont(new Font(this->screen.getFont().getName(), 1, 15));
		/*this->slimeColText = { "Inferior Human Controlled Slime ", "The Pathetic White Slime ", "Angry Red Slimonds ", "The Slime Master ", "Psycho Slime ", "Psycho Slime " };
		this->slimeColours = { YELLOW, WHITE, RED, BLACK, BLUE, BLUE };
		this->loserText1 = { "You are a loser!", this->slimeColText[2] + "gives you the gong!", this->slimeColText[3] + "says \"You are seriously inept.\"", this->slimeColText[4] + "laughs at the pathetic slow opposition.", this->slimeColText[5] + "is still invincible!" };
		this->loserText2 = { "Better luck next time.", "So who has the red face bombing out on level 2, huh?", "Congrats on reaching level 3.", "Congrats on reaching level 4!", "You fell at the last hurdle... but get up and try again!" };
		this->p1Col = 0;*/
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
			this->screen.drawString("Slime Volleyball: One Slime", this->nWidth / 2 - this->screen.getFontMetrics().stringWidth("Slime Volleyball: One Slime") / 2, this->nHeight / 2 - localFontMetrics.getHeight());
			paramGraphics.setColor(WHITE);
			paramGraphics.drawString("Code base by Quin Pendragon", this->nWidth / 2 - localFontMetrics.stringWidth("Code base by Quin Pendragon") / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 2);
			paramGraphics.drawString("AI and Mod by Daniel Wedge", this->nWidth / 2 - localFontMetrics.stringWidth("AI and Mod by Daniel Wedge") / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 3);
			paramGraphics.drawString("Port by high_festiva", this->nWidth / 2 - localFontMetrics.stringWidth("Port by high_festiva") / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 4);
			paramGraphics.drawString("Latest version is at http://www.student.uwa.edu.au/~wedgey/", this->nWidth / 2 - localFontMetrics.stringWidth("Latest version is at http://www.student.uwa.edu.au/~wedgey/") / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 11 / 2);
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
			this->screen.drawString(s1, this->nWidth / 2 - this->screen.getFontMetrics().stringWidth(s1) / 2, this->nHeight / 3);
			str s2 = str("Your score: ") + strutil::IntToString(this->gameScore, 10);
			paramGraphics.drawString(s2, this->nWidth / 2 - localFontMetrics.stringWidth(s2) / 2, this->nHeight / 2 - localFontMetrics.getHeight());
			if (this->fP1PointsWon == 6)
			{
				str s3 = str("Level bonus: ") + strutil::IntToString(1000 * this->fP1PointsWon / (this->fP1PointsWon + this->fP2PointsWon) * scale(), 10) + " points";
				paramGraphics.drawString(s3, this->nWidth / 2 - localFontMetrics.stringWidth(s3) / 2, this->nHeight / 2 + localFontMetrics.getHeight());
				str s4 = str("Time bonus: ") + strutil::IntToString((this->gameTime < 300000L ? 300000L - this->gameTime : 0L) / 1000L * scale(), 10) + " points";
				paramGraphics.drawString(s4, this->nWidth / 2 - localFontMetrics.stringWidth(s4) / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 2);
				if (this->fP2PointsWon == 0)
				{
					str s5 = str("Flawless Victory: ") + strutil::IntToString(1000 * scale(), 10) + " points";
					paramGraphics.drawString(s5, this->nWidth / 2 - localFontMetrics.stringWidth(s5) / 2, this->nHeight / 2 + localFontMetrics.getHeight() * 3);
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
			delete this->gameThread;
			this->gameThread = new MemberThread<SlimeVolleyball>(_T("GameThread"));
			this->gameThread->RequestSelfDestruct();
			this->gameThread->Start(this, &SlimeVolleyball::run);

			break;
		case 401:
		case 403:
			switch (paramEvent.key)
			{
			case 75:
			case 107:
				this->aiMode = aiStartLevel;
			case 76:
			case 108:
				this->fP1PointsWon = this->fP2PointsWon = 0;
				setAI();
				this->gameScore = 0;
				this->fP1Touched = this->fP2Touched = 0;
				this->fP1Touches = this->fP2Touches = 0;
				this->hitNetSinceTouched = false;
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
				this->startTime = System::currentTimeMillis();
				this->paused = false;
				//repaint();
				break;
			case 65:
			case 97:
			case 1006:
				moveP1Left();
				break;
			case 68:
			case 100:
			case 1007:
				moveP1Right();
				break;
			case 87:
			case 119:
			case 1004:
				moveP1Jump();
				break;
			case 67:
			case 99:
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
				delete this->gameThread;
				this->gameThread = new MemberThread<SlimeVolleyball>(_T("GameThread"));
				this->gameThread->RequestSelfDestruct();
				this->gameThread->Start(this, &SlimeVolleyball::run);

				break;
			case 80:
			case 112:
				if (!this->paused)
				{
					this->pausedTime = System::currentTimeMillis();
					this->paused = true;
				}
				else {
					this->startTime += System::currentTimeMillis() - this->pausedTime;
					this->paused = false;
				}
			}
			break;
		case 402:
		case 404:
			switch (paramEvent.key)
			{
			case 65:
			case 97:
			case 1006:
				if (this->p1XV >= 0) break;
				this->p1XV = 0;
				break;
			case 68:
			case 100:
			case 1007:
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
		int i2 = this->p1OldX * this->nWidth / 1000 - i / 2;
		int i3 = 7 * this->nHeight / 10 - this->p1OldY * this->nHeight / 1000;
		this->screen.setColor(this->SKY_COL);
		this->screen.fillRect(i2, i3, i, j);
		i2 = this->p2OldX * this->nWidth / 1000 - i / 2;
		i3 = 7 * this->nHeight / 10 - this->p2OldY * this->nHeight / 1000;
		this->screen.setColor(this->SKY_COL);
		this->screen.fillRect(i2, i3, i, j);
		MoveBall();
		i2 = this->p1X * this->nWidth / 1000 - i / 2;
		i3 = 7 * this->nHeight / 10 - this->p1Y * this->nHeight / 1000;
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
		int i = 30 * this->nHeight / 1000;
		int j = this->ballOldX * this->nWidth / 1000;
		int k = 4 * this->nHeight / 5 - this->ballOldY * this->nHeight / 1000;
		this->screen.setColor(this->SKY_COL);
		this->screen.fillOval(j - i, k - i, i * 2, i * 2);
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
		j = this->ballX * this->nWidth / 1000;
		k = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		this->screen.setColor(this->BALL_COL);
		this->screen.fillOval(j - i, k - i, i * 2, i * 2);
	}

	private: void drawScores() {
		Graphics localGraphics = this->screen;
		localGraphics.getFontMetrics();
		int i = this->nHeight / 15;
		localGraphics.setColor(this->SKY_COL);
		localGraphics.fillRect(0, 0, this->nWidth, i + 22);
		int j = 20;

		for (int k = 0; k < 6; k++)
		{
			if (this->fP1PointsWon >= k + 1)
			{
				localGraphics.setColor(this->slimeColours[this->p1Col]);
				localGraphics.fillOval(j, 30 - i / 2, i, i);
			}
			localGraphics.setColor(WHITE);
			localGraphics.drawOval(j, 30 - i / 2, i, i);
			j += i + 10;
		}

		j = this->nWidth - 20 - 6 * (i + 10);
		for (int m = 0; m < 6; m++)
		{
			if (this->fP2PointsWon >= 6 - m)
			{
				localGraphics.setColor(this->slimeColours[this->p2Col]);
				localGraphics.fillOval(j, 30 - i / 2, i, i);
			}
			localGraphics.setColor(WHITE);
			localGraphics.drawOval(j, 30 - i / 2, i, i);
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

	private: void DrawStatus() {
		Graphics localGraphics = this->screen;
		int i = this->nHeight / 20;
		localGraphics.setColor(this->SKY_COL);
		FontMetrics localFontMetrics = this->screen.getFontMetrics();
		str s = str("Score: ") + strutil::IntToString(this->gameScore, 10) + (!this->fInPlay ? "" : str("	 Time: ") + MakeTime((this->paused ? this->pausedTime : System::currentTimeMillis()) - this->startTime));
		int j = localFontMetrics.stringWidth(s);
		int k = this->nWidth / 2 - j / 2 - 10;
		localGraphics.fillRect(k, 0, j + 20, i + 22);
		localGraphics.setColor(WHITE);
		this->screen.drawString(s, this->nWidth / 2 - localFontMetrics.stringWidth(s) / 2, localFontMetrics.getHeight() * 2);
	}

	public: void drawPrompt() {
		this->screen.setColor(this->COURT_COL);
		this->screen.fillRect(0, 4 * this->nHeight / 5 + 6, this->nWidth, this->nHeight / 5 - 10);
		drawPrompt(this->promptMsg, 0);
	}

	public: void drawPrompt(str paramString, int paramInt) {
		FontMetrics localFontMetrics = this->screen.getFontMetrics();
		this->screen.setColor(WHITE);
		this->screen.drawString(paramString, (this->nWidth - localFontMetrics.stringWidth(paramString)) / 2, this->nHeight * 4 / 5 + localFontMetrics.getHeight() * (paramInt + 1) + 10);
	}

	public: void run() {
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
		int i = 0;
		int j = 0;
		int k = 0;
		this->bGameOver = false;
		this->startTime = System::currentTimeMillis();
		while ((this->gameThread != null) && (!this->bGameOver))
		{
			if (!this->paused)
			{
				this->p1OldX = this->p1X;
				this->p1OldY = this->p1Y;
				this->p2OldX = this->p2X;
				this->p2OldY = this->p2Y;
				this->ballOldX = this->ballX;
				this->ballOldY = this->ballY;
				MoveSlimers();
				DrawSlimers();
				DrawStatus();
			}
			if (this->ballY < 35) {
				long l = System::currentTimeMillis();
				if (this->ballX > 500)
					this->fP1PointsWon += 1;
				else {
					this->fP2PointsWon += 1;
				}

				if ((this->ballX <= 500) && ((this->fP1Touches >= 3) || ((this->hitNetSinceTouched) && (this->fP1Touches > 0)) || (0==this->fP2Touched) || (
					(this->fP1HitStill) && (this->fP1Touches > 0)))) {
					this->fP1Clangers += 1;
					i = 1;
				}
				else if ((this->ballX > 500) && ((this->fP2Touches >= 3) || ((this->hitNetSinceTouched) && (this->fP2Touches > 0)) || (0==this->fP1Touched) || (
					(this->fP2HitStill) && (this->fP2Touches > 0)))) {
					this->fP2Clangers += 1;
					i = 1;
				}

				if ((0!=this->fP1Touched) && (0==this->fP2Touched) && (this->ballX >= 500)) {
					this->fP1Aces += 1;
					j = 1;
					this->gameScore += 200 * scale();
				}
				else if ((0!=this->fP2Touched) && (0==this->fP1Touched) && (this->ballX < 500)) {
					this->fP2Aces += 1;
					j = 1;
				}
				else if ((this->ballX > 500) && (this->fP1Touches > 0)) {
					this->fP1Winners += 1;
					k = 1;
					this->gameScore += 100 * scale();
				}
				else if ((this->ballX <= 500) && (this->fP2Touches > 0)) {
					this->fP2Winners += 1;
					k = 1;
				}

				if ((this->ballX > 500) && (k == 0) && (j == 0)) {
					this->gameScore += 50 * scale();
				}

				this->promptMsg = (this->ballX <= 500 ? this->slimeColText[this->p2Col] : this->slimeColText[this->p1Col]);

				if ((this->fP1PointsWon == 6) || (this->fP2PointsWon == 6))
					this->promptMsg += "wins!";
				else if (j != 0)
					this->promptMsg += "aces the serve!";
				else if (k != 0)
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
				int m = this->ballX;
				drawPrompt();
				drawScores();
				DrawStatus();

				i = 0;
				j = 0;
				k = 0;
				this->mousePressed = false;
				sleep(1500L, true);
				if ((this->fP1PointsWon == 6) || (this->fP2PointsWon == 6))
					finishGame();
				this->promptMsg = "";
				drawPrompt();
				this->p1X = 200;
				this->p1Y = 0;
				this->p2X = 800;
				this->p2Y = 0;
				this->p1XV = 0;
				this->p1YV = 0;
				this->p2XV = 0;
				this->p2YV = 0;
				this->ballX = (m >= 500 ? 200 : 800);
				this->ballY = 400;
				this->ballVX = 0;
				this->ballVY = 0;
				this->fP1Touched = this->fP2Touched = 0;
				this->fServerMoved = false;
				//repaint();
				this->startTime += System::currentTimeMillis() - l;
			}


		sleep(1, true);

		}
		this->fEndGame = true;
		this->fInPlay = false;
		this->promptMsg = "";
		//repaint();
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
		this->gameThread = null;
	}

	private: void gameOver(bool paramBoolean) {
		FontMetrics localFontMetrics1 = this->screen.getFontMetrics();
		drawScores();
		DrawStatus();
		Graphics localGraphics = Graphics();
		FontMetrics localFontMetrics2 = localGraphics.getFontMetrics();
		if (!paramBoolean)
		{
			localGraphics.setColor(this->COURT_COL);
			localGraphics.fillRect((this->nWidth - max(localFontMetrics2.stringWidth(this->loserText1[this->aiMode]), localFontMetrics2.stringWidth(this->loserText2[this->aiMode]))) / 2 - 30, this->nHeight / 2 - localFontMetrics2.getAscent() * 5, max(localFontMetrics2.stringWidth(this->loserText1[this->aiMode]), localFontMetrics2.stringWidth(this->loserText2[this->aiMode])) + 60, localFontMetrics2.getAscent() * 5 + localFontMetrics1.getAscent() * 2);
			localGraphics.setColor(WHITE);
			localGraphics.drawString(this->loserText1[this->aiMode], (this->nWidth - localFontMetrics2.stringWidth(this->loserText1[this->aiMode])) / 2, this->nHeight / 2 - localFontMetrics2.getAscent() * 3);
			localGraphics.drawString(this->loserText2[this->aiMode], (this->nWidth - localFontMetrics2.stringWidth(this->loserText2[this->aiMode])) / 2, this->nHeight / 2 - localFontMetrics2.getAscent() * 2);
			this->screen.drawString("GAME OVER", (this->nWidth - localFontMetrics1.stringWidth("GAME OVER")) / 2, this->nHeight / 2 + localFontMetrics1.getAscent());
		}
		else
		{
			fatality(localGraphics);
			localGraphics.setColor(WHITE);
			this->screen.drawString("YOU WIN!", (this->nWidth - localFontMetrics1.stringWidth("YOU WIN!")) / 2, this->nHeight / 2);
			localGraphics.drawString("The Slimes bow down before the new Slime King!", (this->nWidth - localFontMetrics2.stringWidth("The Slimes bow down before the new Slime King!")) / 2, this->nHeight / 2 + localFontMetrics2.getAscent());
		}

		sleep(500L, false);
		this->bGameOver = true;
	}

	private: void fatality(Graphics /*paramGraphics*/)
	{
	}

	private: void drawP1() {
		int i = this->nWidth / 10;
		int j = this->nHeight / 10;
		int k = this->nWidth / 50;
		int m = this->nHeight / 25;
		int n = this->ballX * this->nWidth / 1000;
		int i1 = 4 * this->nHeight / 5 - this->ballY * this->nHeight / 1000;
		int i2 = this->p1OldX * this->nWidth / 1000 - i / 2;
		int i3 = 7 * this->nHeight / 10 - this->p1OldY * this->nHeight / 1000;
		this->screen.setColor(this->SKY_COL);
		this->screen.fillRect(i2, i3, i, j);
		i2 = this->p2OldX * this->nWidth / 1000 - i / 2;
		i3 = 7 * this->nHeight / 10 - this->p2OldY * this->nHeight / 1000;
		this->screen.setColor(this->SKY_COL);
		this->screen.fillRect(i2, i3, i, j);
		MoveBall();
		i2 = this->p1X * this->nWidth / 1000 - i / 2;
		i3 = 7 * this->nHeight / 10 - this->p1Y * this->nHeight / 1000;
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
	}

	private: int max(int paramInt1, int paramInt2)
	{
		if (paramInt1 > paramInt2) return paramInt1;
		return paramInt2;
	}

	private: void sleep(long paramLong, bool /*paramBoolean*/) {
		if (this->gameThread != null)
			for (int i = 0; i < paramLong / 20L; i++)
				Thread::Sleep(0.02);
	}

	private: int scale() {
		return (int)Math::pow(2.0, this->aiMode);
	}
	public: void destroy() {
		if (this->gameThread != null) {
			this->gameThread->RequestStop();
			this->gameThread = null;
		}
	}
};



}
