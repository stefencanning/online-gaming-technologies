#include "SDL.h"
#undef main
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <ctime>
#include "SDL_ttf.h"
#include <math.h>
#include "Player.h"
#include "networking.h"
#include <fstream>
#include <stdlib.h>
#include "maze.h"
#include "PacketStream.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1300;
const int SCREEN_HEIGHT = 700;
//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//The texture to load into
SDL_Texture* pTexture = NULL;
SDL_Texture* eTexture = NULL;
SDL_Texture* oTexture = NULL;
SDL_Texture* backGroundTexture = NULL;
SDL_Texture* joinButton = NULL;
SDL_Texture* hostButton = NULL;
SDL_Texture* MazeTextures[4];
bool loadMedia();
SDL_Texture* loadTexture( std::string path );
SDL_Rect backGroundPos;
TTF_Font* font; 
//Starts up SDL and creates window
bool init();
//Closes the SDL window
void close();
//Updates the game while players are playing.
void GameUpdate();
//Update the menu checking whether the player hits host or join
void MenuUpdate();
//Presents a screen and allows players to enter in a host and port
void JoinUpdate();
//Presents the lobby screen showing all the players currently in the game.
void LobbyUpdate();

//Creates players
Player* player = new Player();
vector<Player*> enemies = vector<Player*>();
networking network = networking();
Net* n;
bool init();
bool loadMedia();
void GameRecieveLoop();
SDL_Texture* loadTexture( std::string path );
void close();
void recieveArrayLoop();
void LobbyRecieveLoop();
const int mazeSizeX = 52;
const int mazeSizeY = 28;
Maze* maze[52][28];
char* msg;
char* recieved;
bool quit;
enum GameState{Menu, Game, Lobby, Join};
GameState curState;
SDL_Event e;
string hostIP;
string portIP;
bool host;
SDL_Surface* textSurface = new SDL_Surface();
SDL_Texture* survivedTexture;
SDL_Rect stretchRect;
using namespace std;

bool init() 
{ 
	//Initialization flag 
	bool success = true; 

	stretchRect.x = 0; 
	stretchRect.y = 0; 
	stretchRect.w = SCREEN_WIDTH; 
	stretchRect.h = SCREEN_HEIGHT;




	//Initialize SDL 
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) 
	{ 
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); 
		success = false; 
	} 
	else 
	{  
		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0 )
        {
            printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
		}
		
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Tag", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN ); 
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
				if( !( IMG_Init( imgFlags ) & imgFlags  ) ) 
				{ 
					string a = IMG_GetError();
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false; 
				} 
			} 
		}
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


bool loadMedia() 
{ 
	//Loading success flag 
	bool success = true; 
	//Load splash image 
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
	oTexture = loadTexture( "other.png" );
	if( eTexture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	backGroundTexture = loadTexture("trippyGreen.png");

	if( backGroundTexture== NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	hostButton = loadTexture("host.png");
	if( hostButton == NULL)
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}
	joinButton = loadTexture("join.png");
	if(joinButton == NULL)
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}

	MazeTextures[0] = loadTexture("maze01.png"); 
	MazeTextures[1] = loadTexture("maze02.png"); 
	MazeTextures[2] = loadTexture("maze03.png"); 
	MazeTextures[3] = loadTexture("maze04.png"); 

	for(int i = 0; i < 4; i++)
	{
		if( MazeTextures[i] == NULL )
		{
			printf( "Failed to load texture image!\n" );
			success = false;
		}
	}
	TTF_Init();
	font = TTF_OpenFont("FreeSans.ttf", 28);




	return success; 
}






void close() 
{ 
	//Free loaded image 
	//Destroy window 
	SDL_DestroyRenderer( gRenderer ); 
	SDL_DestroyWindow( gWindow ); 
	gWindow = NULL; 
	gRenderer = NULL; 
	//Quit SDL subsystems 
	IMG_Quit(); 
	SDL_Quit();
}


char* doit()
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        cerr << "Error " << WSAGetLastError() <<
                " when getting local host name." << endl;
        return NULL;
    }
    cout << "Host name is " << ac << "." << endl;

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) {
        cerr << "Yow! Bad host lookup." << endl;
        return NULL;
    }
	int i;
    for ( i= 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
    }
    
    struct in_addr addr;
    memcpy(&addr, phe->h_addr_list[i-1], sizeof(struct in_addr));
	cout <<inet_ntoa(addr) << endl;
    return inet_ntoa(addr);
}

int main()
{
	quit = false;
	SDL_Event e;
	hostIP = "";
	portIP = "";
	host = false;
	quit = false;
	curState = GameState::Menu;
	int xPos = 0;
	int yPos = 0;

	backGroundPos.x = 0;
	backGroundPos.y = 0;
	backGroundPos.w = 1300;
	backGroundPos.h = 700;
	
	msg = new char();
	string map[mazeSizeY]={
            "0000000000000000000000000000000000000000000000000000",
            "0nnn0nnn0nnn0nnnn0nnnnnnnnnnn0nnn0nnn0nnnnnnnn0nnnn0",
            "0n0nnn0n0nn0nn0nn0nn00n00n00n0nnn0n0nnn000nn0n0n0nn0",
            "0n00nn0nnn00nnn00nn0nnnn0n0nn0n0n0n0nnnnnn0n0nnn0n00",
            "0nnnn0000n0nn0nnnn0nn00n0n0nnnn0n0n0000nnn0n0n000nn0",
            "000nn0nn0n0n00nn0nn0nnnnnn0n0000nnn0nnn0nnnnnnnn0nn0",
            "000nnnnn0n0nnnn0n0n000n0nn0nnnn0nn0nnnnn0nn0nnnn00n0",
            "0nnn00nn0nnnnn0nnnnnnnn00n000nn0nn0nn0nnn0n0n00nnnn0",
            "0nnnnn0nn0nnn0nn0nn00nnnnnn0nn0n0nnnn000n0nnn00nnn00",
            "0n0n0nn0nn000nnnnn0nnnnn00nnn0nnn0nnnnn0nnnnnnnn0nn0",
            "00nnn0nn0nnnnn00n0nn0n0nn00n0nn0nn0n00nnnn00nn0n00n0",
            "00nnn0nn0nn000nn0nnnnn0nnnnnnnnnnnnnnnnnn0nnnn0n00n0",
            "0n0n0nn0nn0nnnnnnnn00n0n00nn000nn00nn0n00nn0n0nn0nn0",
            "0nnnnn0nn0nnn0nnn0n00nnnn0n0nnnn0nn0n0nnnn0nnn0nnn00",
            "00nnn0nnnnn000n000nnnn00n0n0nn0nnnn0n0nnnn0nnn0nnn00",
            "000n0nn000nnn000n0000n00n0nn00nn000nn0n00nn0n0nn0nn0",
            "0nnnnnn00nn0nnnnnnn0nnnnn00nnnnnnnnnnnnnn0nnnnnn00n0",
            "0n0nn0n0nn000nnnnnnnn000nnnn0nn0nn0n00nnnn00n00n00n0",
            "0n0n0n00nn000n000nn00nnnnnnnn0nnn0nnnnn0nnnnnnnn0nn0",
            "0n00nnnnnnnnnn0n0nnn0n000n0nnn0n0nn0n000n0nnn00nnnn0",
            "0n00n0nn0n000nnn0n0n0nnn0nn00nn0nnn0n0nnn0n0n00nnnn0",
            "0nnnn00nn0nnnnnn000nnn0n0nnnnnnnn0n0nnn00nn0nnnn0n00",
            "0n0nnn0n0nnn000nnnnn000n0000n000n0nn0n0nnnnnnnnn0nn0",
            "0nn00nnn00n0nnn0nn0nnnnnnnnnnnn0nn0nnnnn0n0n0n0000n0",
            "0nnn0n0nnnn0nnnnn00n000n00n0n0nn0nn0nn0n0nnn0nnn0nn0",
            "0n0nn0nn0nn0n0n000nnnn0n00n0n00n0nnnnn0nnn0n0n0n0n00",
            "0n0nnnnn0nnnn0nnnnnn0nnnnnnnnnnn000nnn0n0n0nnn0nnnn0",
			"0000000000000000000000000000000000000000000000000000"
    };//nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn


	for(int i = 0; i < mazeSizeX; i++)
	{

		xPos = i*25;
		
		for(int j = 0; j < mazeSizeY; j++)
		{
			char c = map[j].at(i);
			yPos = j*25;
			if(map[j].at(i)=='0')
			{
				maze[i][j] = new Maze(true,xPos,yPos);
			}
			else
			{
					maze[i][j] = NULL;
			}
		}
		
		
	}
	if(init()!=true)
	{
		printf("Failed To Initialise\n");
	}	
	else
	{
		loadMedia();
		//*
		//ifstream myfile ("C:/config.txt");

		
		WSAData wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
			return 255;
		}

		char* retval = doit();

		WSACleanup();

		
		n = new Net();
		network = networking(retval,n);
		player->myIdent = retval;

		/*if (myfile.is_open())
		{
			for(int i = 0; i < 4; i++)
			{
				getline(myfile,line);
				if(i==0)
				{
					n = new Net();
					network = networking(retval,n);
					player->myIdent = line;
				}
				else
				{
					char* cstr = new char[line.length() + 1];
					strcpy(cstr, line.c_str());
					network.ips.push_back(cstr);
				}
			}
			myfile.close();
		}*/
		//*/
		//n = new Net();
		//network = networking("149.153.102.40",n);
		//player->myIdent = "149.153.102.40";
		thread looping(recieveArrayLoop);
		while(!quit) //While a player has not quit the game -- Game Loop
		{
			if(curState == GameState::Menu)
			{
				MenuUpdate();
			}
			if(curState == GameState::Join)
			{
				JoinUpdate();
			}
			else if(curState == GameState::Lobby)
			{
				LobbyUpdate();
			}
			else if(curState == GameState::Game)
			{
				GameUpdate();
			}			
		}
		looping.detach();
	}
	return 0;
}

void MenuUpdate()
{
	//Declare button positions
	SDL_Rect hostRect = {200, 300, 400, 100};
	SDL_Rect joinRect = {650, 300, 400, 100};
	int mouseX = 0;
	int mouseY = 0;
	while(SDL_PollEvent(&e) != 0)
	{
		if(e.type == SDL_MOUSEBUTTONDOWN)
		{
			SDL_GetMouseState(&mouseX, &mouseY);
			SDL_Point mousePoint = {mouseX, mouseY};
			if(SDL_EnclosePoints(&mousePoint, 1, &hostRect, NULL))
			{
				host = true;
				curState = GameState::Lobby;
			}
			else if(SDL_EnclosePoints(&mousePoint, 1, &joinRect, NULL))
			{
				host = false;
				curState = GameState::Join;
			}
		}
	}
	//Draw buttons
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, hostButton, NULL, &hostRect);
	SDL_RenderCopy(gRenderer, joinButton, NULL, &joinRect);
	SDL_RenderPresent(gRenderer);
}

void JoinUpdate()
{	
	SDL_StartTextInput();
	//Point curString to host value
	string* curString = &hostIP;
	while(curState == GameState::Join)
	{		
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT ) //If user selects quit close the game
			{
				quit = true;
			}
			else if(e.type == SDL_KEYDOWN) //If a text input or keydown is recieved
			{
				if(e.key.keysym.sym == SDLK_BACKSPACE && curString->length()>0)
				{
					//If backspace is pressed, remove the last character of curString						
					curString->erase(curString->length() - 1);
				}
				else if(e.key.keysym.sym == SDLK_RETURN)
				{	
					if(curString == &hostIP)
					{
						curString = &portIP;
					}
					else if(curString == &portIP)
					{
						char* cstr = new char[hostIP.length() + 1];
						strcpy_s(cstr,hostIP.length() + 1, hostIP.c_str());
						network.ips.push_back(cstr);
						curState = GameState::Lobby;
					}
				}								
			}
			else if(e.type == SDL_TEXTINPUT)
			{				
				curString->append(e.text.text);					
			}
			
		}	
		SDL_RenderClear(gRenderer);


		string testString = "host ip: ";
		testString += hostIP;
		SDL_Color text_color = {138,7,7};
		SDL_FreeSurface(textSurface);
		textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
		SDL_Rect stretchRect3;
		stretchRect3.h = 100; 
		stretchRect3.w = testString.length()*28; 
		stretchRect3.x = SCREEN_WIDTH/4; 
		stretchRect3.y = 100;

		survivedTexture = NULL;
		survivedTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		//(textSurface, textRec, gScreenSurface, NULL);
		SDL_RenderCopy( gRenderer, survivedTexture, NULL, &stretchRect3);

		testString = "port: ";
		testString += portIP;
		SDL_FreeSurface(textSurface);
		textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
		stretchRect3.h = 100; 
		stretchRect3.w = testString.length()*28; 
		stretchRect3.x = SCREEN_WIDTH/4; 
		stretchRect3.y = 200;

		survivedTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		//(textSurface, textRec, gScreenSurface, NULL);
		SDL_RenderCopy( gRenderer, survivedTexture, NULL, &stretchRect3);
			
		SDL_RenderPresent(gRenderer);	
	}
	SDL_StopTextInput();
	
}

void LobbyUpdate()
{
	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT ) //If user selects quit close the game
		{
			quit = true;
		}
		else if(e.type == SDL_KEYDOWN) //If a text input or keydown is recieved
		{
			if(e.key.keysym.sym == SDLK_RETURN)
			{	
				if(host==true)
				{
					PacketStream pStream;
					pStream.writeString("start");
					pStream.toCharArray(msg);
					network.send(msg);
					curState = GameState::Game;
				}
			}								
		}
	}	
	PacketStream pStream;
	pStream.writeString(player->myIdent);
	pStream.toCharArray(msg);
	network.send(msg);
	
	SDL_RenderClear(gRenderer);

	string testString = "in lobby: ";
	SDL_Color text_color = {138,7,7};
	SDL_FreeSurface(textSurface);
	try
	{
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	}
	catch(exception e)
	{
		cout<<e.what()<<endl;
	}
	SDL_Rect stretchRect3;
	stretchRect3.h = 50; 
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 30;

	survivedTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
	//(textSurface, textRec, gScreenSurface, NULL);
	SDL_RenderCopy( gRenderer, survivedTexture, NULL, &stretchRect3);



	testString = "me";
	SDL_FreeSurface(textSurface);
	try
	{
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	}
	catch(exception e)
	{
		cout<<e.what()<<endl;
	}
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 60;

	survivedTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
	//(textSurface, textRec, gScreenSurface, NULL);
	SDL_RenderCopy( gRenderer, survivedTexture, NULL, &stretchRect3);
			
	for(int i =0; i < network.ips.size();i++)
	{
		testString = network.ips.at(i);
		SDL_FreeSurface(textSurface);
		try
		{
		textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
		}
		catch(exception e)
		{
			cout<<e.what()<<endl;
		}
		stretchRect3.w = testString.length()*28; 
		stretchRect3.x = SCREEN_WIDTH/5; 
		stretchRect3.y = 90+(30*i);

		survivedTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		//(textSurface, textRec, gScreenSurface, NULL);
		SDL_RenderCopy( gRenderer, survivedTexture, NULL, &stretchRect3);
	}
	try
	{
	SDL_RenderPresent(gRenderer);	
	}
	catch(exception e)
	{
		cout<<e.what()<<endl;
	}
}

void GameUpdate()
{
	player->Update(enemies, SCREEN_WIDTH,SCREEN_HEIGHT); 
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


	for(int i = 0; i < mazeSizeX; i++)
	{
		for(int j = 0; j < mazeSizeY; j++)
		{
			if( maze[i][j] != NULL)
			{
				if( player->getPos().x + player->getPos().w > maze[i][j]->pos.x  &&  player->getPos().x < maze[i][j]->pos.x + maze[i][j]->pos.w &&
				player->getPos().y + player->getPos().h > maze[i][j]->pos.y  &&  player->getPos().y < maze[i][j]->pos.y + maze[i][j]->pos.h )
				{
					//int overlap = 
					if(player->getPos().x < maze[i][j]->pos.x)
					{
						player->setPos(maze[i][j]->pos.x - player->getPos().w,player->getPos().y);
					}
					if(player->getPos().x > maze[i][j]->pos.x)
					{
						player->setPos(maze[i][j]->pos.x + maze[i][j]->pos.w,player->getPos().y);
					}
					if(player->getPos().y < maze[i][j]->pos.y)
					{
						player->setPos(player->getPos().x,maze[i][j]->pos.y - player->getPos().h);
					}
					if(player->getPos().y > maze[i][j]->pos.y)
					{
						player->setPos(player->getPos().x,maze[i][j]->pos.y + maze[i][j]->pos.h);
					}
							
				}
			}

		}
	}
	PacketStream pStream;
	pStream.writeFloat(player->getPos().x);
	pStream.writeFloat(player->getPos().y);
	pStream.writeBool(player->getChaser());
	pStream.writeString(player->myIdent);
	/*
	std::string x = "X:";
	std::string xpos = std::to_string();
	std::string y = ",Y:";
	std::string ypos = std::to_string(player->getPos().y);
	std::string c =",catcher:";
	std::string cbool = std::to_string(player->getChaser());
	std::string ident =",ident:";
	std::string identIp =player->myIdent;
	std::string end  =";";*/
	pStream.toCharArray(msg);
	network.send(msg);
	//recieveLoop();

	SDL_RenderClear(gRenderer);

	SDL_RenderCopy(gRenderer, backGroundTexture, NULL, &backGroundPos);
	//Draw players			
	player->Draw(gRenderer, pTexture);

	for(int i = 0;i < mazeSizeX;i++)
	{
		for(int j = 0;j < mazeSizeY;j++)
		{
			int random = rand() % 4;

			if(maze[i][j] != NULL)
				maze[i][j]->Draw(gRenderer, MazeTextures[random]);
		}
	} 
	for(int i = 0; i < enemies.size(); i++)
	{
		enemies.at(i)->Draw(gRenderer, eTexture);
	}
	//cout<<enemies.at(i)->myIdent<<endl;
			
	//cout<<"x: "<<enemies.at(0)->pos.x<<", y: "<<enemies.at(0)->pos.x<<"\t"<<"x: "<<enemies.at(1)->pos.x<<", y: "<<enemies.at(1)->pos.x<<"\t"<<"x: "<<enemies.at(2)->pos.x<<", y: "<<enemies.at(2)->pos.x<<"\t"<<endl;

	//Update the renderer
	SDL_RenderPresent(gRenderer);
}


void recieveArrayLoop()
{
	while(!quit)
	{
		if(curState == GameState::Lobby)
		{
			LobbyRecieveLoop();
		}
		else if(curState == GameState::Game)
		{
			GameRecieveLoop();
		}	
	}
}

void GameRecieveLoop()
{
	PacketStream pStream;
	string ident;
	float x;
	float y;
	bool catcher;
	//pStream.fromCharArray(network.recieveCharArray());
	char* recievedCon = network.recieveCharArray();
	char test[500];
	memcpy(test, recievedCon, strlen(recievedCon));
	//std::string recievedstr = recievedCon;
	//if(recievedstr!="")
	{
		pStream.fromCharArray(test);
		pStream.readFloat(x);
		pStream.readFloat(y);
		pStream.readBool(catcher);
		pStream.readString(ident);
		bool found = false;
		/*if(ident!= "149.153.102.56")
		{
			int a = 0;
		}*/
		for(int i = 0; i < enemies.size();i++)
		{
			if(enemies.at(i)->myIdent.compare(ident)==0)
			{
				found = true;
				enemies.at(i)->setChaser(catcher);
				enemies.at(i)->setPos(x,y);
			}
		}
		if(!found)
		{
			enemies.push_back(new Player());
			enemies.at(enemies.size()-1)->myIdent = ident;
			enemies.at(enemies.size()-1)->setChaser(catcher);
			enemies.at(enemies.size()-1)->setPos(x,y);
		}
		if(host)
		{
			PacketStream oStream;
			oStream.writeFloat(x);
			oStream.writeFloat(y);
			oStream.writeBool(catcher);
			oStream.writeString(ident);
			oStream.toCharArray(msg);
			network.send(msg,ident);
		}
	}
}


void LobbyRecieveLoop()
{
	PacketStream pStream;
	string ident;
	//pStream.fromCharArray(network.recieveCharArray());
	char* recievedCon = network.recieveCharArray();
	if(recievedCon[0]!='\0')
	{
		char test[500];
		memcpy(test, recievedCon, strlen(recievedCon));
		//std::string recievedstr = recievedCon;
		if(recievedCon[0]!='s')
		{
			pStream.fromCharArray(test);
			pStream.readString(ident);
			bool found = false;
			/*if(ident!= "149.153.102.56")
			{
				int a = 0;
			}*/
			for(int i = 0; i < enemies.size();i++)
			{
				if(enemies.at(i)->myIdent.compare(ident)==0)
				{
					found = true;
				}
			}
			if(!found)
			{
				enemies.push_back(new Player());
				enemies.at(enemies.size()-1)->myIdent = ident;
				char* cstr = new char[ident.length() + 1];
				strcpy_s(cstr,ident.length() + 1, ident.c_str());
				network.ips.push_back(cstr);
			}
		}
		else
		{
			curState=GameState::Game;
		}
	}
}