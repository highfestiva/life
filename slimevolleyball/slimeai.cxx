#include "pch.h"
#include "slimeai.hpp"
#include "slimevolleyball.hpp"



namespace slime {



SlimeAI::SlimeAI() {
	CLEAR_MEMBERS(ballX, app);
	percent_ = 100;
	team = -1;
}

int SlimeAI::init(SlimeVolleyball* slime1_p, int i) {
	this->app = slime1_p;
	this->player = i;
	return this->team;
}

void SlimeAI::saveVars(int array_of_int[], bool boolean1, bool boolean2) {
	this->ballX = (this->player == 2 ? array_of_int[0] : 1000 - array_of_int[0]);
	this->ballY = array_of_int[1];
	this->ballVX = (this->player == 2 ? array_of_int[2] : -array_of_int[2]);
	this->ballVY = array_of_int[3];
	this->p1X = (this->player == 2 ? array_of_int[4] : 1000 - array_of_int[8]);
	this->p1Y = (this->player == 2 ? array_of_int[5] : array_of_int[9]);
	this->p1VX = (this->player == 2 ? array_of_int[6] : -array_of_int[10]);
	this->p1VY = (this->player == 2 ? array_of_int[7] : array_of_int[11]);
	this->p2X = (this->player == 2 ? array_of_int[8] : 1000 - array_of_int[4]);
	this->p2Y = (this->player == 2 ? array_of_int[9] : array_of_int[5]);
	this->p2VX = (this->player == 2 ? array_of_int[10] : -array_of_int[6]);
	this->p2VY = (this->player == 2 ? array_of_int[11] : array_of_int[7]);
	this->p1Fire = (this->player == 2 ? boolean1 : boolean2);
	this->p2Fire = (this->player == 2 ? boolean2 : boolean1);
}

void SlimeAI::move(int i) {
	if (this->player == 1)
		switch (i) {
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
		switch (i) {
			case 0:
				this->app->moveP2Left(percent_); return;
			case 1:
				this->app->moveP2Right(percent_); return;
			case 2:
				this->app->moveP2Jump(percent_); return;
			case 3:
				this->app->moveP2Stop(); return;
		}
}



}
