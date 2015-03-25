#pragma once
#include <SDL.h>

using namespace std;

class Maze
{
private:
	
public:
	Maze(bool, int x, int y);
	~Maze(void);
	SDL_Rect pos;
	
	///Set Methods///
	void setPos(int x, int y);

	void Update(SDL_Rect);

	void Draw(SDL_Renderer*, SDL_Texture* img);
};
