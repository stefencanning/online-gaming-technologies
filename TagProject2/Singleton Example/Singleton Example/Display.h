#pragma once
#include <stddef.h>
using namespace std;
class Display
{
private:
	Display(void);
public:
	static Display* me;
	~Display(void);
	static Display* getInstance();
	static int counter;
};

