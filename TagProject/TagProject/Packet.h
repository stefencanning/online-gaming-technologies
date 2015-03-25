#ifndef _PACKET_H_
#define _PACKET_H_

class Packet
{

public:
	Packet();

	int ReadInt();
	float ReadFloat();
	bool ReadBool();

	int WriteInt();
	float WriteFloat();
	bool WriteBool(); 
};

#endif 