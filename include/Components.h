#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "SDL.h"
#include <vector>
#include <string>
#include <map>
#include <queue>

#include "DamageTypes.h"
#include "Slots.h"
#include "UseableFunctionEnum.h"
#include "StatusTypes.h"
#include "GameObject.h"
#include "Utils.h"

struct Position
{
	int x;
	int y;
	Position(int i, int j);
	Position();
	~Position();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Renderable
{
	char chr;
	SDL_Color colour;
	int spriteX;
	int spriteY;
	int sheet;
	Renderable(char _chr, SDL_Color _colour, int _spriteX, int _spriteY, int _sheet);
	Renderable();
	~Renderable();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Fighter
{
	int maxHealth;
	int health;
	int temp_health;
	int strength;
	int base_strength;
	int dexterity;
	int base_dexterity;
	int constitution;
	int base_constitution;
	int intelligence;
	int base_intelligence;
	int wisdom;
	int base_wisdom;
	int charisma;
	int base_charisma;
	int armour_class;
	int base_armour_class;
	bool isAlive;
	Fighter(int _maxHealth, int base_str, int base_dex, int base_con, int base_int, int base_wis, int base_cha, int _base_AC);
	Fighter();
	~Fighter();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Actor
{
	Actor();
	~Actor();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Player
{
	Player();
	~Player();
	int level;
	int exp;
	int next;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct AI
{
	AI(int _exp, int _level, std::string _damage);
	AI();
	~AI();
	int exp;
	int level;
	std::string damage;
	std::vector<int> path;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Item
{
  std::string description;
	int level;
	Item(std::string desc, int _level);
	Item();
	~Item();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Inventory
{
	int capacity;
	std::vector<GameObject*> inventory;
	std::vector<int> inventoryMirror;
	Inventory(int _capacity);
	Inventory();
	~Inventory();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Weapon
{
	DamageTypes damageType;
	std::string damage;
	bool twoHanded;
	Weapon(DamageTypes _damageType, std::string _damage, bool _twoHanded);
	Weapon();
	~Weapon();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Armour
{
	DamageTypes resistance;
	DamageTypes weakness;
	int armourBonus;
	Armour(DamageTypes _resistance, DamageTypes _weakness, int _armourBonus);
	Armour();
	~Armour();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Wearable
{
	Wearable(EquipSlots _slot);
	Wearable();
	~Wearable();
	EquipSlots slot;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Body
{
	std::map<EquipSlots, GameObject*> slots;
	std::map<int, int> slotsMirror;
	Body();
	~Body();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Useable
{
	//Useable(UseableFunctionEnums func, int _numUses);
	Useable(std::string func, int _numUses, bool _ranged, bool _AOE);
	Useable();
	~Useable();	
	std::string funcToDo;
	int numUses;
	bool ranged;
	bool AOE;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Healing
{
	Healing(int _roll);
	Healing();
	~Healing();
	int roll;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Damage
{
	Damage(int _radius, int _roll, DamageTypes _type, int _chance);
	Damage();
	~Damage();
	int radius;
	int roll;
	DamageTypes type;
	int chance;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct AreaDamage
{
	AreaDamage(int _radius, int _roll, int _splashRadius, DamageTypes _type, int _chance);
	AreaDamage();
	~AreaDamage();
	int radius;
	int roll;
	int splashRadius;
	DamageTypes type;
	int chance;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Status
{
	Status(int _radius, StatusTypes _status, int _splashRadius);
	Status();
	~Status();
	int radius;
	StatusTypes statusType;
	int splashRadius;
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Consumable
{
	Consumable();
	~Consumable();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct Stairs
{
	Stairs();
	~Stairs();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
};

struct StatusContainer
{
	StatusContainer();
	~StatusContainer();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);
	std::map<StatusTypes, std::pair<int, int>> statuses;
};

struct Animation
{
	Animation();
	~Animation();
	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);

	Uint32 lifetime;
	Uint32 current_lifetime;
	std::queue<int> spriteSheets;
	std::queue<int> sprite_x;
	std::queue<int> sprite_y;
};

#endif
