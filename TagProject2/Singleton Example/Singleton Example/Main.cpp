#pragma once
#include <stddef.h>
#include "Display.h"
#include <iostream>
using namespace std;
Display* display;


void main()
{
	for(int i = 0; i < 100; i++)
	{
		display = Display::getInstance();
		cout << "counter: "<<display->counter<<"\n";
	}
	system("PAUSE");
}