#include <SDL.h>
#undef main
#include <SDL_image.h>
#include <iostream>
#include <math.h>
#include "Player.h"
#include "networking.h"
#include <fstream>
#include <thread>
#include <string>
#include <stdlib.h>
#include "maze.h"
#include "PacketStream.h"
#include "SDL_ttf.h"
#include "Portal.h"

using namespace std; 

//Screen dimension constants
const int SCREEN_WIDTH = 1300;
const int SCREEN_HEIGHT = 700;
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
float speed = 400;
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
//Portal
SDL_Texture* portalTexture; 
bool upMove=false,downMove=false,leftMove=false,rightMove=false;
Uint32 startTime = 0;

bool initGameTime = true;
int GameTime = 0;
float playerAccumalatedTime = 0;
int amountOfTicksSinceLaunch = 0;

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
//Presents the victory screen showing all the players time accumalated and who did the best.
void VictoryUpdate();

//Creates players
Player* player = new Player();
//Dead rec
Player* playerGhost = new Player();
vector<Player*> enemies = vector<Player*>();
vector<Player*> enemiesPast = vector<Player*>();
vector<clock_t> updateTimer = vector<clock_t>();
networking network = networking();
Net* n;
//Portal
vector<Portal*> portals = vector<Portal*>();

//vector<float*> playerTimes = vector<float*>();


bool init();
bool loadMedia();
void GameRecieveLoop();
void updateLoop();
SDL_Texture* loadTexture( std::string path );
void close();
void recieveArrayLoop();
void LobbyRecieveLoop();
void VictoryRecieveLoop();
const int mazeSizeX = 52;
const int mazeSizeY = 28;
Maze* maze[52][28];
char* msg;
char* recieved;
bool quit;
enum GameState{Menu, Game, Lobby, Join,Victory};
GameState curState;
SDL_Event e;
string hostIP;
string portIP;
bool host;
SDL_Surface* textSurface = new SDL_Surface();
SDL_Surface* screen;
SDL_Texture* survivedTexture;
string* curString;
clock_t mainTime = clock();
Uint32 lastTimeSent = 0;
bool deadReckoning = false;


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
	//curState = GameState::Victory;
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
		time_t t =time(0);
		srand(t);
		n = new Net();
		network = networking(retval,n);
		player->myIdent = retval;
		bool done = false;
		while(!done)
		{
			int x = (rand() % 52);
			int y = (rand() % 28);
			if(maze[x][y] == NULL)
			{
				player->setPos(x * 25,y*25);
				playerGhost->setPos(x * 25,y*25);
				done=true;
			}
		}

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
		thread updating(updateLoop);
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
			else if(curState == GameState::Victory)
			{
				VictoryUpdate();
			}		
		}
		looping.detach();
		updating.detach();
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
				curString = &hostIP;
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
					/*if(curString == &hostIP)
					{
						curString = &portIP;
					}
					else if(curString == &portIP)*/
					{
						char* cstr = new char[hostIP.length() + 1];
						strcpy(cstr, hostIP.c_str());
						network.ips.push_back(cstr);
						enemies.push_back(new Player());
						enemies.at(enemies.size()-1)->myIdent = hostIP;
						enemiesPast.push_back(new Player());
						enemiesPast.at(enemiesPast.size()-1)->myIdent = hostIP;
						updateTimer.push_back(clock());
						curState = GameState::Lobby;
					}
				}								
			}
			else if(e.type == SDL_TEXTINPUT)
			{				
				curString->append(e.text.text);					
			}
			//Fill the surface white 
			SDL_FillRect(screen, NULL, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
			string testString = "Host ip: ";
			testString += hostIP;
			SDL_Color text_color = {138,7,7};
			//Create destination rectangle for host string
			SDL_Rect stretchRect3;
			stretchRect3.h = 100; 
			stretchRect3.w = testString.length()*28; 
			stretchRect3.x = SCREEN_WIDTH/4; 
			stretchRect3.y = 100;
			//Input text to the surface
			textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);  
			SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
			//Free surface once complete
			SDL_FreeSurface(textSurface);
			SDL_UpdateWindowSurface(gWindow);
		}	
	SDL_StopTextInput();
	
}

void LobbyUpdate() //<<P2P
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
					int num = rand()%(enemies.size()+1);
					if(num==0)
					{
						player->setChaser(true);
					}
					else
					{
						num-=1;
						enemies.at(num)->setChaser(true);
						PacketStream pStream;
						pStream.writeString("update");
						pStream.writeFloat(enemies.at(num)->getPos().x);
						pStream.writeFloat(enemies.at(num)->getPos().y);
						pStream.writeFloat(enemies.at(num)->ghostVelX);
						pStream.writeFloat(enemies.at(num)->ghostVelY);
						pStream.writeBool(true);
						pStream.writeString(enemies.at(num)->myIdent);
						pStream.toCharArray(msg);
						network.send(msg);
					}
					curState = GameState::Game;
					upMove=false;downMove=false;leftMove=false;rightMove=false;
					mainTime = clock();
				}
			}								
		}
	}	
	PacketStream pStream;
	pStream.writeString("lobby");
	pStream.writeString(player->myIdent);
	pStream.toCharArray(msg);
	network.send(msg);
	if(host)
	{
		for(int i = 0; i < enemies.size(); i++)
		{
			PacketStream oStream;
			oStream.writeString("lobby");
			oStream.writeString(enemies.at(i)->myIdent);
			oStream.toCharArray(msg);
			network.send(msg);
		}
	}
	//Fill the surface white 
	SDL_FillRect(screen, NULL, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
	//Create destination rectangle for host string
	//Input text to the surface
	//Free surface once complete
	string testString = "in lobby: ";
	SDL_Color text_color = {138,7,7};
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	SDL_Rect stretchRect3;
	stretchRect3.h = 50; 
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 30;
	
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);  
	SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
	SDL_FreeSurface(textSurface);



	testString = "me";
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 60;
	SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
	SDL_FreeSurface(textSurface);
			
	for(int i =0; i < network.ips.size();i++)
	{
		testString = network.ips.at(i);
		textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
		stretchRect3.w = testString.length()*28; 
		stretchRect3.x = SCREEN_WIDTH/5; 
		stretchRect3.y = 90+(30*i);
		SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
		SDL_FreeSurface(textSurface);

	}
	SDL_UpdateWindowSurface(gWindow);

}

void VictoryUpdate()
{
	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT ) //If user selects quit close the game
		{
			quit = true;
		}
	}		
		PacketStream pStream;
		pStream.writeString("victory");
		pStream.writeFloat(player->accumalatedTime);
		pStream.writeString(player->myIdent);
		pStream.toCharArray(msg);
		network.send(msg);
		if(host)
		{
			for(int i = 0; i < enemies.size(); i++)
			{
				PacketStream oStream;
				oStream.writeString("victory");
				oStream.writeFloat(enemies.at(i)->accumalatedTime);
				oStream.writeString(enemies.at(i)->myIdent);
				oStream.toCharArray(msg);
				network.send(msg);
			}
		}

	//Fill the surface white 
	SDL_FillRect(screen, NULL, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
	//Create destination rectangle for host string
	//Input text to the surface
	//Free surface once complete
	string testString = "Accumalted time of players: ";
	SDL_Color text_color = {138,7,7};
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	SDL_Rect stretchRect3;
	stretchRect3.h = 50; 
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 30;
	
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);  
	SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
	SDL_FreeSurface(textSurface);

	testString = "me: ";
	
    std::ostringstream ostr; //output string stream
	GameTime = (SDL_GetTicks() - amountOfTicksSinceLaunch)/1000;
    ostr << (int)player->accumalatedTime/1000; //use the string stream just like cout,
    //except the stream prints not to stdout but to a string.
	string seconds = " seconds";
    testString += ostr.str() + seconds; //the str() function of the stream 
    //returns the string.
	
	//testString;
	textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
	stretchRect3.w = testString.length()*28; 
	stretchRect3.x = SCREEN_WIDTH/5; 
	stretchRect3.y = 60;
	SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
	SDL_FreeSurface(textSurface);
			
	for(int i =0; i < enemies.size();i++)
	{
		testString = enemies.at(i)->myIdent;
		std::ostringstream ostr; //output string stream
		GameTime = (SDL_GetTicks() - amountOfTicksSinceLaunch)/1000;
		ostr << ((int)enemies.at(i)->accumalatedTime/1000); //use the string stream just like cout,
		//except the stream prints not to stdout but to a string.
		string seconds = " seconds";
		string space = ": ";
		testString += space + ostr.str() + seconds; //the str() function of the stream 

		textSurface = TTF_RenderText_Solid(font, testString.c_str(), text_color);
		stretchRect3.w = testString.length()*28; 
		stretchRect3.x = SCREEN_WIDTH/5; 
		stretchRect3.y = 90+(30*i);
		SDL_BlitSurface(textSurface, NULL, screen, &stretchRect3);
		SDL_FreeSurface(textSurface);

	}
	SDL_UpdateWindowSurface(gWindow);

}



void GameUpdate()
{
	clock_t curTime = clock();
	player->Update(enemies, SCREEN_WIDTH,SCREEN_HEIGHT);
	
	if (initGameTime == true)
	{
		
		amountOfTicksSinceLaunch = SDL_GetTicks();
		initGameTime = false;
	}
	
	


	if(deadReckoning)
	{
		playerGhost->setPos(playerGhost->x +  (playerGhost->ghostVelX*(speed*((curTime-mainTime)/1000.0f))), playerGhost->y + (playerGhost->ghostVelY*(speed*((curTime-mainTime)/1000.0f))));
		for(int i = 0; i< enemies.size();i++)
		{
			enemies.at(i)->setPos(enemies.at(i)->x +  (enemies.at(i)->ghostVelX*(speed*((curTime-mainTime)/1000.0f))), enemies.at(i)->y + (enemies.at(i)->ghostVelY*(speed*((curTime-mainTime)/1000.0f))));
		}
	}

	if(host)//<<authoritative host
	{
		if((SDL_GetTicks() - amountOfTicksSinceLaunch)/1000 > 180)
		{
			curState = GameState::Victory;
		}
		if((SDL_GetTicks() - startTime) > 6000.0f)
		{
			int x = (rand() % 52);
			int y = (rand() % 28);
			if(maze[x][y] == NULL)
			{
				portals.push_back(new Portal(x * 25,y * 25));
				PacketStream oStream;
				oStream.writeString("portal");
				oStream.writeBool(false);
				oStream.writeFloat(x*25);
				oStream.writeFloat(y*25);
				oStream.writeFloat(portals.size()-1);
				oStream.toCharArray(msg);
				network.send(msg);
			}
			startTime = SDL_GetTicks();
		}
		for(int i = 0; i < portals.size(); i++)
		{
			if(SDL_GetTicks() - portals.at(i)->startTime > 7000.0f)
			{
				PacketStream oStream;
				oStream.writeString("portal");
				oStream.writeBool(true);
				oStream.writeFloat(portals.at(i)->position.x);
				oStream.writeFloat(portals.at(i)->position.y);
				oStream.writeFloat(i);
				oStream.toCharArray(msg);
				network.send(msg);
				portals.erase(portals.begin()+i);
				i--;
			}
		}
		for(int i = 0; i < portals.size(); i++)
		{
			if(player->CheckCollision(player->pos, portals.at(i)->position))
			{
				bool done = false;
				while(!done)
				{
					int x = (rand() % 52);
					int y = (rand() % 28);
					if(maze[x][y] == NULL)
					{
						player->setPos(x * 25,y*25);
						done=true;
					}
				}
				PacketStream oStream;
				oStream.writeString("portal");
				oStream.writeBool(true);
				oStream.writeFloat(portals.at(i)->position.x);
				oStream.writeFloat(portals.at(i)->position.y);
				oStream.writeFloat(i);
				oStream.toCharArray(msg);
				network.send(msg);
				portals.erase(portals.begin()+i);
				i--;
				break;
			}
			for(int j = 0; j < enemies.size(); j++)
			{
				SDL_Rect pos;
				pos=enemies.at(j)->getPos();
				if(!deadReckoning)
				{
					float tempX=(enemies.at(j)->x-enemiesPast.at(j)->x);
					float tempY=(enemies.at(j)->y-enemiesPast.at(j)->y);
					float dist = sqrt((tempX*tempX)+(tempY*tempY));
					if(dist!=0)
					{
						tempX/=dist;
						tempY/=dist;
					}
					tempX=enemiesPast.at(j)->x+(speed*(tempX)*min(((mainTime-updateTimer.at(j))/1000.0f),dist/speed));
					tempY=enemiesPast.at(j)->y+(speed*(tempY)*min(((mainTime-updateTimer.at(j))/1000.0f),dist/speed));
					pos.x=tempX;
					pos.y=tempY;
				}
				if(player->CheckCollision(portals.at(i)->position, pos))
				{
					bool done = false;
					int x=0;
					int y=0;
					while(!done)
					{
						x = (rand() % 52);
						y = (rand() % 28);
						if(maze[x][y] == NULL)
						{
							done=true;
								x*=25;
								y*=25;
						}
					}
					PacketStream oStream;
					oStream.writeString("update");
					oStream.writeFloat(x);
					oStream.writeFloat(y);
					oStream.writeFloat(enemies.at(j)->ghostVelX);
					oStream.writeFloat(enemies.at(j)->ghostVelY);
					oStream.writeBool(enemies.at(j)->getChaser());
					oStream.writeString(enemies.at(j)->myIdent);
					oStream.toCharArray(msg);
					network.send(msg);
					PacketStream dStream;
					dStream.writeString("portal");
					dStream.writeBool(true);
					dStream.writeFloat(portals.at(i)->position.x);
					dStream.writeFloat(portals.at(i)->position.y);
					dStream.writeFloat(i);
					dStream.toCharArray(msg);
					network.send(msg);					
					portals.erase(portals.begin()+i);
					i--;
					break;
				}
			}
		}
		if(player->getChaser() == true)
		{

			player->accumalatedTime += curTime - mainTime;

			for(int i = 0; i < enemies.size(); i++)
			{
				SDL_Rect pos;
				pos=enemies.at(i)->getPos();
				if(!deadReckoning)
				{
					float tempX=(enemies.at(i)->x-enemiesPast.at(i)->x);
					float tempY=(enemies.at(i)->y-enemiesPast.at(i)->y);
					float dist = sqrt((tempX*tempX)+(tempY*tempY));
					if(dist!=0)
					{
						tempX/=dist;
						tempY/=dist;
					}
					tempX=enemiesPast.at(i)->x+(speed*(tempX)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
					tempY=enemiesPast.at(i)->y+(speed*(tempY)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
					pos.x=tempX;
					pos.y=tempY;
				}
				if(player->CheckCollision(player->pos, pos))
				{
					bool done = false;
					int x=0;
					int y=0;
					while(!done)
					{
						x = (rand() % 52);
						y = (rand() % 28);
						if(maze[x][y] == NULL)
						{
							done=true;
								x*=25;
								y*=25;
						}
					}
					player->setChaser(false);
					PacketStream pStream;
					pStream.writeString("update");
					pStream.writeFloat(x);
					pStream.writeFloat(y);
					pStream.writeFloat(enemies.at(i)->ghostVelX);
					pStream.writeFloat(enemies.at(i)->ghostVelY);
					pStream.writeBool(true);
					pStream.writeString(enemies.at(i)->myIdent);
					pStream.toCharArray(msg);
					network.send(msg);
					enemiesPast.at(i)->x=x;
					enemiesPast.at(i)->y=y;
					enemies.at(i)->x=x;
					enemies.at(i)->y=y;
				}
			}
		}
		else
		{
			for(int i = 0; i < enemies.size(); i++)
			{
				if(enemies.at(i)->getChaser())
				{
					SDL_Rect pos;
					pos=enemies.at(i)->getPos();
					enemies.at(i)->accumalatedTime += curTime - mainTime;
					if(!deadReckoning)
					{
						float tempX=(enemies.at(i)->x-enemiesPast.at(i)->x);
						float tempY=(enemies.at(i)->y-enemiesPast.at(i)->y);
						float dist = sqrt((tempX*tempX)+(tempY*tempY));
						if(dist!=0)
						{
							tempX/=dist;
							tempY/=dist;
						}
						tempX=enemiesPast.at(i)->x+(speed*(tempX)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
						tempY=enemiesPast.at(i)->y+(speed*(tempY)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
						pos.x=tempX;
						pos.y=tempY;
					}
					if(player->CheckCollision(player->pos,pos))
					{
						bool done = false;
						int x=0;
						int y=0;
						while(!done)
						{
							x = (rand() % 52);
							y = (rand() % 28);
							if(maze[x][y] == NULL)
							{
								x*=25;
								y*=25;
								done=true;
							}
						}
						player->setPos(x,y);
						player->setChaser(true);
						PacketStream oStream;
						oStream.writeString("update");
						oStream.writeFloat(enemies.at(i)->getPos().x);
						oStream.writeFloat(enemies.at(i)->getPos().y);
						oStream.writeFloat(enemies.at(i)->ghostVelX);
						oStream.writeFloat(enemies.at(i)->ghostVelY);
						oStream.writeBool(false);
						oStream.writeString(enemies.at(i)->myIdent);
						oStream.toCharArray(msg);
						network.send(msg);
					}
					for(int j = 0; j < enemies.size(); j++)
					{
						if(i!=j)
						{
							SDL_Rect pos2;
							pos2=enemies.at(j)->getPos();
							if(!deadReckoning)
							{
								float tempX2=(enemies.at(j)->x-enemiesPast.at(j)->x);
								float tempY2=(enemies.at(j)->y-enemiesPast.at(j)->y);
								float dist2 = sqrt((tempX2*tempX2)+(tempY2*tempY2));
								if(dist2!=0)
								{
									tempX2/=dist2;
									tempY2/=dist2;
								}
								tempX2=enemiesPast.at(j)->x+(speed*(tempX2)*min(((mainTime-updateTimer.at(j))/1000.0f),dist2/speed));
								tempY2=enemiesPast.at(j)->y+(speed*(tempY2)*min(((mainTime-updateTimer.at(j))/1000.0f),dist2/speed));
								pos2.x=tempX2;
								pos2.y=tempY2;
							}
							if(player->CheckCollision(pos2, pos))
							{
								bool done = false;
								int x=0;
								int y=0;
								while(!done)
								{
									x = (rand() % 52);
									y = (rand() % 28);
									if(maze[x][y] == NULL)
									{
										done=true;
								x*=25;
								y*=25;
									}
								}
								PacketStream pStream;
								pStream.writeString("update");
								pStream.writeFloat(x);
								pStream.writeFloat(y);
								pStream.writeFloat(enemies.at(j)->ghostVelX);
								pStream.writeFloat(enemies.at(j)->ghostVelY);
								pStream.writeBool(true);
								pStream.writeString(enemies.at(j)->myIdent);
								pStream.toCharArray(msg);
								network.send(msg);
								enemiesPast.at(j)->x=x;
								enemiesPast.at(j)->y=y;
								enemies.at(j)->x=x;
								enemies.at(j)->y=y;
								PacketStream oStream;
								oStream.writeString("update");
								oStream.writeFloat(enemies.at(i)->getPos().x);
								oStream.writeFloat(enemies.at(i)->getPos().y);
								oStream.writeFloat(enemies.at(i)->ghostVelX);
								oStream.writeFloat(enemies.at(i)->ghostVelY);
								oStream.writeBool(false);
								oStream.writeString(enemies.at(i)->myIdent);
								oStream.toCharArray(msg);
								network.send(msg);
							}
						}
					}
				}
			}
		}
	}
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
				upMove=true;
				break;
			case(SDLK_DOWN):
				downMove=true;
				break;
			case(SDLK_LEFT):
				leftMove=true;
				break;
			case(SDLK_RIGHT):
				rightMove=true;
				break;
			}
		}
		else if( e.type == SDL_KEYUP )
		{
			switch( e.key.keysym.sym )
			{
			case(SDLK_UP):
				upMove=false;
				break;
			case(SDLK_DOWN):
				downMove=false;
				break;
			case(SDLK_LEFT):
				leftMove=false;
				break;
			case(SDLK_RIGHT):
				rightMove=false;
				break;
			}
		}
	}
	player->SetLastSentVelocity(0,0);
	if(upMove)
	{
		player->setPos(player->x, player->y - (speed*((curTime-mainTime)/1000.0f)));
		player->SetLastSentVelocity(player->ghostVelX,player->ghostVelY-1);
	}
	if(downMove)
	{
		player->setPos(player->x, player->y + (speed*((curTime-mainTime)/1000.0f)));
		player->SetLastSentVelocity(player->ghostVelX,player->ghostVelY+1);
	}
	if(leftMove)
	{
		player->setPos(player->x - (speed*((curTime-mainTime)/1000.0f)), player->y);
		player->SetLastSentVelocity(player->ghostVelX-1,player->ghostVelY);
	}
	if(rightMove)
	{
		player->setPos(player->x + (speed*((curTime-mainTime)/1000.0f)), player->y);
		player->SetLastSentVelocity(player->ghostVelX+1,player->ghostVelY);
	}
	//Clear the renderer

	float totalOverlapX=0,totalOverlapY=0;
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
					float x=player->x;
					float y=player->y;
					float w=player->getPos().w;
					float h=player->getPos().h;
					float x2=maze[i][j]->pos.x;
					float y2=maze[i][j]->pos.y;
					float w2=maze[i][j]->pos.w;
					float h2=maze[i][j]->pos.h;
					float overlapX,overlapY;
					if(x<x2)
					{
						overlapX = x2-(x+w);
					}
					else
					{
						overlapX = x2+w2-x;
					}
					if(y<y2)
					{
						overlapY = y2-(y+h);
					}
					else
					{
						overlapY = y2+h2-y;
					}
					if(abs(overlapY)!=abs(overlapX))
					{
						if(abs(overlapY)<abs(overlapX))
						{
							totalOverlapY=overlapY;
						}
						else
						{
							totalOverlapX=overlapX;
						}
					}
					else
					{
						totalOverlapY=overlapY;
						totalOverlapX=overlapX;
					}
				}
			}
		}
	}
	player->setPos(player->x+totalOverlapX,player->y+totalOverlapY);
	if(totalOverlapX!=0)
	{
		player->SetLastSentVelocity(0,player->ghostVelY);
	}
	if(totalOverlapY!=0)
	{
		player->SetLastSentVelocity(player->ghostVelX,0);
	}

	SDL_RenderClear(gRenderer);

	SDL_RenderCopy(gRenderer, backGroundTexture, NULL, &backGroundPos);
	
	
	
	//Draw players			
	player->Draw(gRenderer, pTexture);

	/*if(sqrt((player->pos.x - playerGhost->pos.x)*(player->pos.x - playerGhost->pos.x)+(player->pos.y - playerGhost->pos.y)*(player->pos.y- playerGhost->pos.y)) > 50)
	{
		 lastTimeSent = SDL_GetTicks();
		 player->SetLastSentPosition(player->getPos().x, player->getPos().y);
		 player->SetLastSentVelocity(1);
		 playerGhost->setPos(playerGhost->SetDRPositionX(SDL_GetTicks(), lastTimeSent),playerGhost->SetDRPositionY(SDL_GetTicks() , lastTimeSent));			
	}*/
	if(deadReckoning)
	{
		if(sqrt((player->pos.x - playerGhost->pos.x)*(player->pos.x - playerGhost->pos.x)+(player->pos.y - playerGhost->pos.y)*(player->pos.y- playerGhost->pos.y)) > 5)
		{
			playerGhost->setPos(player->pos.x,player->pos.y);
			playerGhost->SetLastSentVelocity(player->ghostVelX,player->ghostVelY);
			PacketStream pStream;
			pStream.writeString("update");
			pStream.writeFloat(player->getPos().x);
			pStream.writeFloat(player->getPos().y);
			pStream.writeFloat(player->ghostVelX);
			pStream.writeFloat(player->ghostVelY);
			pStream.writeBool(player->getChaser());
			pStream.writeString(player->myIdent);
			pStream.toCharArray(msg);
			network.send(msg);
		}
	}
	//playerGhost->Draw(gRenderer, pTexture);

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
		if(enemies.at(i)->getChaser() == true)
		{
			if(deadReckoning)
			{
				enemies.at(i)->Draw(gRenderer, eTexture);
			}
			else
			{
				float tempX=(enemies.at(i)->x-enemiesPast.at(i)->x);
				float tempY=(enemies.at(i)->y-enemiesPast.at(i)->y);
				float dist = sqrt((tempX*tempX)+(tempY*tempY));
				if(dist!=0)
				{
					tempX/=dist;
					tempY/=dist;
				}
				tempX=enemiesPast.at(i)->x+(speed*(tempX)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
				tempY=enemiesPast.at(i)->y+(speed*(tempY)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
				enemies.at(i)->Draw(gRenderer, eTexture,tempX,tempY);
			}
		}
		else
		{
			if(deadReckoning)
			{
				enemies.at(i)->Draw(gRenderer, oTexture);
			}
			else
			{
				float tempX=(enemies.at(i)->x-enemiesPast.at(i)->x);
				float tempY=(enemies.at(i)->y-enemiesPast.at(i)->y);
				float dist = sqrt((tempX*tempX)+(tempY*tempY));
				if(dist!=0)
				{
					tempX/=dist;
					tempY/=dist;
				}
				tempX=enemiesPast.at(i)->x+(speed*(tempX)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
				tempY=enemiesPast.at(i)->y+(speed*(tempY)*min(((mainTime-updateTimer.at(i))/1000.0f),dist/speed));
				enemies.at(i)->Draw(gRenderer, oTexture,tempX,tempY);
			}
		}
	}
	for(int i = 0; i < portals.size(); i++)
	{
		portals.at(i)->Draw(gRenderer, portalTexture);
	}
	//cout<<enemies.at(i)->myIdent<<endl;
			
	//cout<<"x: "<<enemies.at(0)->pos.x<<", y: "<<enemies.at(0)->pos.x<<"\t"<<"x: "<<enemies.at(1)->pos.x<<", y: "<<enemies.at(1)->pos.x<<"\t"<<"x: "<<enemies.at(2)->pos.x<<", y: "<<enemies.at(2)->pos.x<<"\t"<<endl;

	//Update the renderer
	SDL_RenderPresent(gRenderer);
	mainTime = curTime;
}


bool init()
{
	//Initialization flag
	bool success = true;
	startTime = SDL_GetTicks();
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
	screen = SDL_GetWindowSurface(gWindow);
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

	portalTexture = loadTexture("portal.png");

	if( portalTexture== NULL )
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

void updateLoop() //<<state based updates
{  
	clock_t timeLast = clock();
	while(!quit)
	{
		if(curState == GameState::Game)
		{
			clock_t curTime = clock();
			if((curTime-timeLast)>1000.0f/1000.0f)
			{
				if(!deadReckoning)
				{
					PacketStream pStream;
					pStream.writeString("update");
					pStream.writeFloat(player->getPos().x);
					pStream.writeFloat(player->getPos().y);
					pStream.writeFloat(player->ghostVelX);
					pStream.writeFloat(player->ghostVelY);
					pStream.writeBool(player->getChaser());
					pStream.writeString(player->myIdent);
					pStream.toCharArray(msg);
					network.send(msg);
					timeLast=curTime;
				}
			}
		}
	}
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
		else if(curState == GameState::Victory)
		{
			VictoryRecieveLoop();
		}	
	}
}

void GameRecieveLoop()
{
	PacketStream pStream;
	string type;
	string ident;
	float x;
	float y;
	float velX;
	float velY;
	bool catcher;
	//pStream.fromCharArray(network.recieveCharArray());
	char* recievedCon = network.recieveCharArray();
	if(recievedCon[0]!='\0')
	{
		char test[500];
		memcpy(test, recievedCon, strlen(recievedCon));
		//std::string recievedstr = recievedCon;
	
	
		pStream.fromCharArray(test);
		pStream.readString(type);
		/*if(ident!= "149.153.102.56")
		{
			int a = 0;
		}*/
		if(type.compare("update")==0)
		{
			pStream.readFloat(x);
			pStream.readFloat(y);
			pStream.readFloat(velX);
			pStream.readFloat(velY);
			pStream.readBool(catcher);
			pStream.readString(ident);
			bool found = false;
			if(player->myIdent.compare(ident)==0)
			{
				player->setChaser(catcher);
				player->setPos(x,y);
			}
			else
			{
				for(int i = 0; i < enemies.size();i++)
				{
					if(enemies.at(i)->myIdent.compare(ident)==0)
					{
						found = true;
						enemiesPast.at(i)->setChaser(enemies.at(i)->getChaser());
						enemiesPast.at(i)->setPos(enemies.at(i)->x,enemies.at(i)->y);
						enemiesPast.at(i)->SetLastSentVelocity(velX,velY);
						enemies.at(i)->setChaser(catcher);
						enemies.at(i)->setPos(x,y);
						enemies.at(i)->SetLastSentVelocity(velX,velY);
						updateTimer.at(i) = clock();
					}
				}
				if(!found)
				{
					enemies.push_back(new Player());
					enemies.at(enemies.size()-1)->myIdent = ident;
					enemies.at(enemies.size()-1)->setChaser(catcher);
					enemies.at(enemies.size()-1)->setPos(x,y);
					enemies.at(enemies.size()-1)->SetLastSentVelocity(velX,velY);
					updateTimer.push_back(clock());
					enemiesPast.push_back(new Player());
					enemiesPast.at(enemiesPast.size()-1)->myIdent = ident;
					enemiesPast.at(enemiesPast.size()-1)->setChaser(catcher);
					enemiesPast.at(enemiesPast.size()-1)->setPos(x,y);
					enemiesPast.at(enemiesPast.size()-1)->SetLastSentVelocity(velX,velY);
				}
			}
		}
		else if(type.compare("portal")==0)
		{
			bool kill;
			float iter;
			pStream.readBool(kill);
			pStream.readFloat(x);
			pStream.readFloat(y);
			pStream.readFloat(iter);
			if(kill)
			{
				if(iter<portals.size())
				{
					portals.erase(portals.begin()+iter);
				}
			}
			else
			{
				portals.push_back(new Portal(x,y));
			}
		}
		else if(type.compare("victory")==0)
		{
			/*float timeForPlayer;
			pStream.readFloat(timeForPlayer);
			pStream.readString(ident);
			for(int i = 0; i < enemies.size();i++)
			{
				if(enemies.at(i)->myIdent.compare(ident)==0)
				{
					enemies.at(i)->accumalatedTime = timeForPlayer;
				}
			}*/
			curState = GameState::Victory;
		}
		
	}
}


void LobbyRecieveLoop()
{
	PacketStream pStream;
	string ident;
	string type;
	//pStream.fromCharArray(network.recieveCharArray());
	char* recievedCon = network.recieveCharArray();
	if(recievedCon[0]!='\0')
	{
		char test[500];
		memcpy(test, recievedCon, strlen(recievedCon));
		pStream.fromCharArray(test);
		pStream.readString(type);
		//std::string recievedstr = recievedCon;
		if(type.compare("lobby")==0)
		{
			pStream.readString(ident);
			bool found = false;
			/*if(ident!= "149.153.102.56")
			{
				int a = 0;
			}*/
			if(ident!=player->myIdent)
			{
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
					enemiesPast.push_back(new Player());
					enemiesPast.at(enemiesPast.size()-1)->myIdent = ident;
					updateTimer.push_back(clock());
					char* cstr = new char[ident.length() + 1];
					strcpy(cstr, ident.c_str());
					network.ips.push_back(cstr);
				}
			}
		}
		else if(type.compare("start")==0)
		{
			mainTime = clock();
			upMove=false,downMove=false,leftMove=false,rightMove=false;
			curState=GameState::Game;
		}
	}
}

void VictoryRecieveLoop()
{	
	PacketStream pStream;
	string type;
	string ident;
	float timeForPlayer;

	char* recievedCon = network.recieveCharArray();
	if(recievedCon[0]!='\0')
	{
		char test[500];
		memcpy(test, recievedCon, strlen(recievedCon));

		pStream.fromCharArray(test);
		pStream.readString(type);

		if(type.compare("victory")==0)
		{
			pStream.readFloat(timeForPlayer);
			pStream.readString(ident);

			if(player->myIdent.compare(ident)==0)
			player->accumalatedTime = timeForPlayer;

			for(int i = 0; i < enemies.size();i++)
			{
				if(enemies.at(i)->myIdent.compare(ident)==0)
				{
					enemies.at(i)->accumalatedTime = timeForPlayer;
				}
			}
			//curState = GameState::Victory;
		}
	}
}
