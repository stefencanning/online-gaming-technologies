#pragma once


#include <SDL_image.h>
#include "Player.h"
#include "maze.h"

using namespace std;

class Portal
{
public:

	Portal(int x, int y); 
	SDL_Rect position;
	//void Update(SDL_Rect);
	void Update(vector<Player*> enimies, Player* player,Maze* maze[52][28]);

	Uint32 startTime;
	//Box collision detector
	bool CheckCollision( SDL_Rect a, SDL_Rect b );

	void Draw(SDL_Renderer*r, SDL_Texture* img);

};