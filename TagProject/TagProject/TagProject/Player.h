#pragma once
#include <SDL.h>

using namespace std;

class Player
{
private:
	SDL_Rect pos;
	bool enemy;
	bool chaser;
public:
	Player(bool);
	~Player(void);

	///Get Methods///
	SDL_Rect getPos();
	bool isEnemy();
	/////////////////
	void setPos(int x, int y);
	void setChaser(bool);
	/////////////////

	void Update(SDL_Rect, const int, const int);

	//Box collision detector
	bool CheckCollision( SDL_Rect a, SDL_Rect b );

	void Draw(SDL_Renderer*, SDL_Texture* img);

};

