#include "SlimeAI.hpp"
#include "SlimeVolleyball.hpp"



namespace Slime
{



SlimeAI::SlimeAI()
{
	CLEAR_MEMBERS(ballX, app);
	team = -1;
}

int SlimeAI::init(SlimeVolleyball* paramSlime1P, int paramInt)
{
	this->app = paramSlime1P;
	this->player = paramInt;
	return this->team;
}

void SlimeAI::saveVars(int paramArrayOfInt[], bool paramBoolean1, bool paramBoolean2)
{
	this->ballX = (this->player == 2 ? paramArrayOfInt[0] : 1000 - paramArrayOfInt[0]);
	this->ballY = paramArrayOfInt[1];
	this->ballVX = (this->player == 2 ? paramArrayOfInt[2] : -paramArrayOfInt[2]);
	this->ballVY = paramArrayOfInt[3];
	this->p1X = (this->player == 2 ? paramArrayOfInt[4] : 1000 - paramArrayOfInt[8]);
	this->p1Y = (this->player == 2 ? paramArrayOfInt[5] : paramArrayOfInt[9]);
	this->p1VX = (this->player == 2 ? paramArrayOfInt[6] : -paramArrayOfInt[10]);
	this->p1VY = (this->player == 2 ? paramArrayOfInt[7] : paramArrayOfInt[11]);
	this->p2X = (this->player == 2 ? paramArrayOfInt[8] : 1000 - paramArrayOfInt[4]);
	this->p2Y = (this->player == 2 ? paramArrayOfInt[9] : paramArrayOfInt[5]);
	this->p2VX = (this->player == 2 ? paramArrayOfInt[10] : -paramArrayOfInt[6]);
	this->p2VY = (this->player == 2 ? paramArrayOfInt[11] : paramArrayOfInt[7]);
	this->p1Fire = (this->player == 2 ? paramBoolean1 : paramBoolean2);
	this->p2Fire = (this->player == 2 ? paramBoolean2 : paramBoolean1);
}

void SlimeAI::move(int paramInt)
{
	if (this->player == 1)
		switch (paramInt) {
			case 0:
				this->app->moveP1Right(); return;
			case 1:
				this->app->moveP1Left(); return;
			case 2:
				this->app->moveP1Jump(); return;
			case 3:
				this->app->moveP1Stop(); return;
			}
	if (this->player == 2)
		switch (paramInt)
		{
			case 0:
				this->app->moveP2Left(); return;
			case 1:
				this->app->moveP2Right(); return;
			case 2:
				this->app->moveP2Jump(); return;
			case 3:
				this->app->moveP2Stop(); return;
		}
}



}
