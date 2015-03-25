#include "Maze.h"
#include <iostream>


Maze::Maze(bool playerType,int x,int y)
{
	pos.x = x;
	pos.y = y;
	pos.h = 25;
	pos.w = 25;
}


Maze::~Maze(void)
{
}

///Get Methods///
void Maze::setPos(int x, int y)
{
	pos.x = x;
	pos.y = y;
}

void Maze::Update(SDL_Rect enemyPos)
{
	//Rectangle collision with enemyPos
}

void Maze::Draw(SDL_Renderer* r, SDL_Texture* img)
{
	SDL_RenderCopy(r, img, NULL, &pos);
}