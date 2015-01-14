
#include <WinSock2.h>
#include "Net.h"
#include <iostream>
#include <string>

using namespace std;

void playerA();
void playerB();
int ship[5];
int dead[5];
int fired[50];
string name;


int main()
{
	string input = "";
	cout<<"please input your name\n";
	cin >> input;
	name = input;
	playerA();
	/*if(input == "a")
	{
		playerA();
		player = 1;
	}
	if(input == "b")
	{
		playerB();
		player = 2;
	}*/
	system("PAUSE");
	return 0;
}



void playerA()
{
	Net net = Net();
	net.initialise();
	net.setupUDP(28000,"149.153.102.40");
	cout<<"your port is: "<<net.portNum<<"\n";
	char message[200] = "";
	for(int i = 0; i < 5; i++)
	{
		cout << "enter ship "<<(i+1)<<" position between 1 and 50\n";
		cin>>message;
		ship[i] = atoi(message);
		cout << "ship "<<(i+1)<<" was placed at "<<ship[i]<<"\n";
	}
	cout<<"waiting on enemy placement!\n";
	net.sendData("149.153.102.56",28001,"11001121");
	while(atoi(message)!=11001121)
	{
		net.receiveData(message);
	}
	cout<<"enemy found\n";
	//net.sendData("149.153.102.56",28001,"11001121");
	cout<<"take your first shot, you haven't shot at:\n";
	for (int i = 0; i < 50; i++)
	{
		if(fired[i]==0)
		{
			cout<<i+1<<", ";
		}
	}
	cout<<"\n";
	cin>>message;
	cout<<"you fired at position "<<atoi(message)<<"\n";
	if(atoi(message)!=0)
	fired[atoi(message)-1]=1;
	net.sendData("149.153.102.56",28001,message);
	cout<<"waiting for enemy attack!"<<"\n";
	while(message != "quit")
	{
		while(!net.receiveData(message))
		{
		}
		cout<<"Enemy shot at " << message << "\n";
		int number = atoi(message);
		int count = 0;
		for(int i = 0; i < 5; i++)
		{     
			 if(number == ship[i]||dead[i] == 1)
			 {
			  dead[i] =1;
			  cout<<"Ship " << i + 1 << " destroyed at "<<ship[i]<<"\n";
			  count++;
			 }
			 else
			 {
			  cout<<"Ship " << i + 1 << " safe at "<<ship[i]<<"\n";
			 }
			 if(count > 5)
			 {
			  cout << "Game Over Man, GAME OVER!\n";      
			 }
		}    


		cout<<"take your shot, you haven't shot at:\n";
		for (int i = 0; i < 50; i++)
		{
			if(fired[i]==0)
			{
				cout<<i+1<<", ";
			}
		}
		cout<<"\n";
		cin>>message;
		cout<<"you fired at position "<<atoi(message)<<"\n";
		if(atoi(message)!=0)
		{
			fired[atoi(message)-1]=1;
		}
		net.sendData("149.153.102.56",28001,message);
		cout<<"waiting for enemy attack!"<<"\n";
	}
}






void playerB()
{
	Net net = Net();
	net.initialise();
	net.setupUDP(28001,"149.153.102.40");
	cout<<net.portNum<<"\n";
	char message[200] = "";
	while(message != "quit")
	{
		while(!net.receiveData(message))
		{
		}
		cout<<"[A]: "<<message<<"\n";
		cin>>message;
		net.sendData("149.153.102.56",28000,message);
		cout<<"waiting for reply!"<<"\n";
	}
}