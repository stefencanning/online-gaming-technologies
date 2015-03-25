#pragma once
#include <SDL.h>

using namespace std;

class Maze
{
private:
	
	bool enemy;
	bool chaser;
public:
	Maze(bool, int x, int y);
	~Maze(void);
	SDL_Rect pos;
	///Get Methods///
	
	bool isEnemy();
	/////////////////

	///Set Methods///
	void setPos(int x, int y);
	void setChaser(bool);
	/////////////////

	void Update(SDL_Rect);

	void Draw(SDL_Renderer*, SDL_Texture* img);
};
