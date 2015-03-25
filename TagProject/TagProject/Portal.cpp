#include "Portal.h"


Portal::Portal(int x, int y)
{
	position.x = x;
	position.y = y; 
	position.w = 25; 
	position.h = 25; 
	startTime = SDL_GetTicks();
}
	//void Update(SDL_Rect);
void Portal::Update(vector<Player*> enimies, Player* player,Maze* maze[52][28]){

	for(int i = 0; i < enimies.capacity(); i++)
	{
		if(	CheckCollision(position,enimies.at(i)->getPos()))
		{
			
		}
	}
	if(CheckCollision(position,player->getPos()))
	{
		int x = (rand() % 52);
		int y = (rand() % 28);
		if(maze[x][y] == NULL)
		{
			player->setPos(x * 25,y*25);
		}
		
	}

}

	//Box collision detector
bool Portal::CheckCollision( SDL_Rect a, SDL_Rect b )
{
	//The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

	 //If any of the sides from A are outside of B return false
   if( leftA >= rightB )
    {
        return false;
    }
	if( rightA <= leftB )
    {
        return false;
    }
	 if( topA >= bottomB )
    {
        return false;
    }
	if( bottomA <= topB )
    {
        return false;
    }
	
	return true;  
}

void Portal::Draw(SDL_Renderer* renderer, SDL_Texture* img){

	SDL_RenderCopy(renderer, img, NULL, &position);
}
