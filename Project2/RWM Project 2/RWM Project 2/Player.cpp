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


Player::Player()
{
	pos.x = 25;
	pos.y = 25;
	pos.h = 25;
	pos.w = 25;
	//True for enemy, False for player
	//enemy = playerType;
	//True for chaser, False for runner
	chaser = false;
}


Player::~Player(void)
{
}

///Get Methods///
SDL_Rect Player::getPos()
{
	return pos;
}
bool Player::getChaser()
{
	return chaser;
}

bool Player::isEnemy()
{
	return enemy;
}
/////////////////

///Set Methods///
void Player::setPos(int x, int y)
{
	pos.x = x;
	pos.y = y;
}
void Player::setChaser(bool chase)
{
	chaser = chase;
}
/////////////////

void Player::Update(vector<Player*> enemies, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	 
    //If the player went too far to the left or right
	//Collision Detection between player and enemy

	//Bounds checking 
	if( ( pos.x < 0 ))
    {
        //Move back
		pos.x = 0;
    }

    //If the player collided or went too far up or down
	if( ( pos.y < 0 ) )
    {
		pos.y = 0;
    }

	/*if(pos.x + pos.w > SCREEN_WIDTH)
	{
		pos.x = SCREEN_WIDTH - pos.w;
	}

	if(pos.y + pos.y > SCREEN_HEIGHT)
	{
		pos.y = SCREEN_HEIGHT - pos.y;
	}
*/
	for(int i = 0; i < enemies.size(); i++)
	{
		if(	CheckCollision(pos, enemies.at(i)->getPos()))
	{	
		pos.x = 50; 
		pos.y = 50;
	}
	}

}


void Player::Draw(SDL_Renderer* r, SDL_Texture* img)
{
	SDL_RenderCopy(r, img, NULL, &pos);
}

bool Player::CheckCollision( SDL_Rect a, SDL_Rect b )
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
