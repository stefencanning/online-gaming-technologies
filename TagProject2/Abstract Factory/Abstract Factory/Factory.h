#pragma once
#include "Character.h"
class Factory
{
public:
	Factory(void);
	~Factory(void);
	virtual Character* CreatePlayer()=0;
	virtual Character* CreateOpponents()=0;
};

