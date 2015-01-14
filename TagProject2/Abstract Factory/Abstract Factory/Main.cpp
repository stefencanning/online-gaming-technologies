#include "CharacterFactory.h"
#include "Character.h"
#include <vector>

void main()
{
	Factory* factory = new CharacterFactory();
	vector<Character*> characters;
	characters.push_back(factory->CreatePlayer());
	characters.push_back(factory->CreateOpponents());
	for (int i = 0; i < characters.size(); i++)
	{
		characters.at(i)->Draw();
	}
	system("pause");
}