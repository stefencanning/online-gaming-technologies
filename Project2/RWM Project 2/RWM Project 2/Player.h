#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <math.h>
#include "Player.h"
#include "networking.h"
#include <fstream>
#include <thread>
#include <string>
#include <stdlib.h>

using namespace std;

class Player
{
private:
	bool enemy;
	bool chaser;
public:
	Player();
	~Player(void);
	SDL_Rect pos;

	///Get Methods///
	SDL_Rect getPos();
	bool getChaser();
	bool isEnemy();
	/////////////////

	///Set Methods///
	void setPos(int x, int y);
	void setChaser(bool);
	/////////////////

	//void Update(SDL_Rect);
	void Update(vector<Player*> , const int, const int);

	//Box collision detector
	bool CheckCollision( SDL_Rect a, SDL_Rect b );
	string myIdent;

	void Draw(SDL_Renderer*, SDL_Texture* img);
};

