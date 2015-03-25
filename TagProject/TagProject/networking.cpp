#include "networking.h"


networking::networking(string ip, Net* n)
{

	connection = n;
	myIP = ip;
	char *cstr = new char[ip.length() + 1];
	strcpy(cstr, ip.c_str());
	connection->initialise();
	connection->setupUDP(28000, cstr);
	port = 28000;
}

networking::networking()
{
	myIP="";
}


networking::~networking(void)
{
}


void networking::send(char* msg)
{
	for(int i = 0; i < ips.size(); i++)
	{
		connection->sendData(ips.at(i),port,msg);
	}
	//connection.sendData(ip,port,msg);
}
void networking::send(char* msg,string ip)
{
	for(int i = 0; i < ips.size(); i++)
	{
		if(ip.compare(ips.at(i))!=-1&&ip.compare(ips.at(i))!=0)
		{
			connection->sendData(ips.at(i),port,msg);
		}
	}
	//connection.sendData(ip,port,msg);
}

void networking::send(string msg)
{
	char *cstr = new char[msg.length() + 1];
	strcpy(cstr, msg.c_str());
	for(int i = 0; i < ips.size(); i++)
	{
		connection->sendData(ips.at(i),port,cstr);
	}
	//connection->sendData("149.153.102.40",port,cstr);
}

string networking::recieve()
{
	string msg="";
	char message[500]="";
	if (connection->receiveData(message))
	{
		msg+=message;
	}
	return msg;
}

char* networking::recieveCharArray()
{
	char message[500]="";
	connection->receiveData(message);
	return message;
}