#include "Display.h"


Display* Display::me = NULL;
int Display::counter = 0;

Display* Display::getInstance()
{
	if(me==NULL)
	{
		me = new Display();
	}
	return me;
}

Display::Display(void)
{
	counter++;
}


Display::~Display(void)
{
}
