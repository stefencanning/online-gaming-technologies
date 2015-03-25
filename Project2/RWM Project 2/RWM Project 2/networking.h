#pragma once
#include <WinSock2.h>
#include "Net.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
class networking
{
public:
	networking(void);
	networking(string,Net*);
	~networking(void);
	void send(char* msg);
	void send(char* msg,string ip);
	void send(string msg);
	string recieve();
	char* recieveCharArray();
	Net* connection;
	string myIP;
	vector<char*> ips;
	int port;
};

