#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <math.h>
#include "Player.h"

using namespace std; 

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//The texture to load into
SDL_Texture* pTexture = NULL;
SDL_Texture* eTexture = NULL;
SDL_Texture* menu_Texture = NULL; 
SDL_Rect menu_Pos; 
bool loadMedia();
SDL_Texture* loadTexture( std::string path );

//Starts up SDL and creates window
bool init();
//Closes the SDL window
void close();

//Creates players
Player* player = new Player(false);
Player* enemy = new Player(true);

enum State {MENU, INGAME, OVER};

int main()
{
	State state = MENU; 
	bool quit = false;
	SDL_Event e;
	enemy->setPos(200,100); 
	menu_Pos.x = 0;
	menu_Pos.y = 0;
	menu_Pos.w = SCREEN_WIDTH;
	menu_Pos.h = SCREEN_HEIGHT; 
 
	if(init()!=true)
	{
		printf("Failed To Initialise\n");
	}	
	else
	{
		loadMedia();

	
		while(!quit) //While a player has not quit the game -- Game Loop
		{
			if(state == MENU)
			{   
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//If a mouse button was pressed
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
						case(SDLK_SPACE):
							state = INGAME; 
							break;
						}
					}
				}
				//Draw menu Screen 
				//Clear the renderer
				SDL_RenderClear(gRenderer);
				SDL_RenderCopy(gRenderer, menu_Texture, NULL, &menu_Pos);
				//Update the renderer
				SDL_RenderPresent(gRenderer);
			}
			else if(state == INGAME)
			{
				player->Update(enemy->getPos(), SCREEN_WIDTH,SCREEN_HEIGHT); 
				//Check events in queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//If a mouse button was pressed
					else if( e.type == SDL_KEYDOWN )
					{
						switch( e.key.keysym.sym )
						{
						case(SDLK_UP):
							player->setPos(player->getPos().x, player->getPos().y - 5);
							break;
						case(SDLK_DOWN):
							player->setPos(player->getPos().x, player->getPos().y + 5);
							break;
						case(SDLK_LEFT):
							player->setPos(player->getPos().x - 5, player->getPos().y);
							break;
						case(SDLK_RIGHT):
							player->setPos(player->getPos().x + 5, player->getPos().y);
							break;
						}
					}
				}
				//Clear the renderer
				SDL_RenderClear(gRenderer);
			
				//Draw players			
				player->Draw(gRenderer, pTexture);
				enemy->Draw(gRenderer, eTexture);

				//Update the renderer
				SDL_RenderPresent(gRenderer);
				
			}// END INGAME
		}//END WHILE
	}
	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	pTexture = loadTexture( "player.png" );
	if( pTexture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	eTexture = loadTexture( "enemy.png" );
	if( eTexture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	menu_Texture = loadTexture( "menuScreen.png" );
	if( menu_Texture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	return success;
}

SDL_Texture* loadTexture( std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}
void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();
}