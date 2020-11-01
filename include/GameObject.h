#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>

struct Position;
struct Renderable;
struct Fighter;
struct Actor;
struct Player;
struct AI;
struct Item;
struct Inventory;
struct Weapon;
struct Armour;
struct Wearable;
struct Body;
struct Useable;
struct Consumable;
struct Stairs;
struct StatusContainer;
struct Animation;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i, int length);

	std::string m_name;
	int m_uid;
  
	Position *position;
	Renderable *renderable;
	Fighter *fighter;
	Actor *actor;
	Player *player;
	AI *ai;
	Item *item;
	Inventory *inventory;
	Weapon *weapon;
	Armour *armour;
	Wearable *wearable;
	Body *body;
	Useable *useable;
	Consumable *consumable;
	Stairs *stairs;
	StatusContainer *statusContainer;
	Animation* animation;

protected:

private:
	
};

#endif
