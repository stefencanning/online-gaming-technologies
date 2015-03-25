#pragma once
#include <sstream>
#include <string>

using namespace std;

class PacketStream
{
private:
	ostringstream outputStream;

public:
	istringstream inputStream;
	PacketStream(void);
	~PacketStream(void);

	void toCharArray(char* arrayIn);
	void fromCharArray(char* arrayIn);

	void writeInt(int x);
	void readInt(int &x);

	void writeFloat(float x);
	void readFloat(float &x);

	void writeBool(bool x);
	void readBool(bool& x);

	void writeString(string x);
	void readString(string& x);
};

