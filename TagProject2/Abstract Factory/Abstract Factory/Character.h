#pragma once
#include <iostream>
#include <stddef.h>
using namespace std;
class Character
{
public:
	Character(void);
	virtual ~Character(void){};
	virtual void Draw() = 0;
};

