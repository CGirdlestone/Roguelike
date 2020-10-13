
#include "SDL.h"

#include "Components.h"
#include <string>
#include <iostream>

void debug_component(int x)
{
	std::cout << x << std::endl;
}

void debug_component(const std::string& word)
{
	std::cout << word << std::endl;
}

void serialiseInt(std::vector<uint8_t> &byteVector, int x)
{
	byteVector.push_back((x >> 24) & 0xFF);
	byteVector.push_back((x >> 16) & 0xFF);
	byteVector.push_back((x >> 8) & 0xFF);
	byteVector.push_back((x >> 0) & 0xFF);
}

void serialiseString(std::vector<uint8_t> &byteVector, std::string stringToSerialise)
{
	int size = static_cast<int>(stringToSerialise.length());
	int letterCode;

	serialiseInt(byteVector, size);
	
	for (int i = 0; i < size; ++i){
		letterCode = static_cast<int>(stringToSerialise[i]);
		serialiseInt(byteVector, letterCode);
	}
}

int deserialiseInt(char* buffer, int i)
{
	int value = int(buffer[i + 0] << 24 |
					buffer[i + 1] << 16 |
					buffer[i + 2] << 8 |
					buffer[i + 3]);

	return value;
}

int advanceFourBytes(int i)
{
	return i += 32;
}

Position::Position(int i, int j)
{
  x = i;
  y = j;
}

Position::Position()
{
	x = 0;
	y = 0;
}

Position::~Position()
{

}

void Position::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "Position" << std::endl;
	serialiseInt(byteVector, x);
	std::cout << "x: " << x << std::endl;
	serialiseInt(byteVector, y);
	std::cout << "y: " << y << std::endl;
}

int Position::deserialise(char* buffer, int i)
{
	x = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);
	debug_component(x);

	y = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);
	debug_component(y);
	return i;
}


Renderable::Renderable(char _chr, SDL_Color _colour, int _spriteX, int _spriteY, int _sheet)
{
	chr = _chr;
	colour = _colour;
	spriteX = _spriteX;
	spriteY = _spriteY;
	sheet = _sheet;
}

Renderable::Renderable()
{

}

Renderable::~Renderable()
{

}

void Renderable::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "Renderable" << std::endl;
	serialiseInt(byteVector, static_cast<int>(chr));
	std::cout << "char: " << chr << std::endl;
	serialiseInt(byteVector, colour.r);
	std::cout << "red: " << colour.r << std::endl;
	serialiseInt(byteVector, colour.g);
	std::cout << "green: " << colour.g << std::endl;
	serialiseInt(byteVector, colour.b);
	std::cout << "blue: " << colour.b << std::endl;
	serialiseInt(byteVector, spriteX);
	std::cout << "sprite x: " << spriteX << std::endl;
	serialiseInt(byteVector, spriteY);
	std::cout << "sprite y: " << spriteY << std::endl;
	serialiseInt(byteVector, sheet);
	std::cout << "sheet: " << sheet << std::endl;
}

int Renderable::deserialise(char* buffer, int i)
{
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	chr = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	red = (uint8_t)deserialiseInt(buffer, i);
	i = advanceFourBytes(i);
	
	green = (uint8_t)deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	blue = (uint8_t)deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	colour = {red, green, blue};

	spriteX = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	spriteY = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	sheet = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Fighter::Fighter(int _maxHealth, int _power, int _defence):
maxHealth(_maxHealth), health(_maxHealth), power(_power), defence(_defence), isAlive(true)
{

}

Fighter::Fighter()
{
	health = 0;
	maxHealth = 0;
	power = 0;
	defence = 0;
	isAlive = true;
}

Fighter::~Fighter()
{

}

void Fighter::serialise(std::vector<uint8_t> &byteVector)
{
	int _isAlive = 1;

	std::cout << "Fighter" << std::endl;
	serialiseInt(byteVector, health);
	std::cout << "health: " << health << std::endl;
	serialiseInt(byteVector, maxHealth);
	std::cout << "max health: " << maxHealth << std::endl;
	serialiseInt(byteVector, power);
	std::cout << "power: " << power << std::endl;
	serialiseInt(byteVector, defence);
	std::cout << "defence: " << defence << std::endl;
	serialiseInt(byteVector, _isAlive);
	std::cout << "is alive: " << _isAlive << std::endl;
}

int Fighter::deserialise(char* buffer, int i)
{
	health = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	maxHealth = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	power = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	defence = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	isAlive = deserialiseInt(buffer, i) == 1 ? true : false;
	i = advanceFourBytes(i);

	return i;
}

Actor::Actor()
{

}

Actor::~Actor()
{

}

void Actor::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "actor" << std::endl;
}

int Actor::deserialise(char* buffer, int i)
{

	return i;
}


Player::Player()
{
	level = 1;
	exp = 0;
	next = 100;
}

Player::~Player()
{

}
#include <stdio.h>
void Player::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "player" << std::endl;
	serialiseInt(byteVector, level);
	std::cout << "level" << level << std::endl;
	serialiseInt(byteVector, exp);
	std::cout << "exp: " << exp << std::endl;
	serialiseInt(byteVector, next);
	std::cout << "next level: " << next << std::endl;
}

int Player::deserialise(char* buffer, int i)
{
	level = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	exp = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	next = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Item::Item(std::string desc, int _level):
description(desc), level(_level)
{

}

Item::Item()
{
	description = "";
}

Item::~Item()
{

}

void Item::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "item" << std::endl;
	serialiseString(byteVector, description);
	std::cout << "description: " << description << std::endl;
	serialiseInt(byteVector, level);
	std::cout << "level: " << level << std::endl;
}

int Item::deserialise(char* buffer, int i)
{
	std::cout << "Index in Item: " << i << std::endl;
	int descLength = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	char* desc = new char[descLength];
	for (int k = 0; k < descLength; ++k){
		desc[k] = static_cast<char>(deserialiseInt(buffer, i));
		i = advanceFourBytes(i);
	}
	description.assign(desc, descLength);

	delete[] desc;

	level = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}

AI::AI(int _exp, int _level):
exp(_exp), level(_level)
{

}

AI::AI()
{
	exp = 0;
}

AI::~AI()
{

}

void AI::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "ai" << std::endl;
	serialiseInt(byteVector, exp);
	std::cout << "exp: " << exp << std::endl;
	serialiseInt(byteVector, level);
	std::cout << "level: " << level << std::endl;
}

int AI::deserialise(char* buffer, int i)
{
	exp = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	level = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}

Inventory::Inventory(int _capacity):
capacity(_capacity)
{

}

Inventory::Inventory()
{
	capacity = 0;
}

Inventory::~Inventory()
{

}

void Inventory::serialise(std::vector<uint8_t> &byteVector)
{
	int inventorySize = static_cast<int>(inventory.size());

	std::cout << "inventory:" << std::endl;
	serialiseInt(byteVector, capacity);
	std::cout << "capacity: " << capacity << std::endl;
	serialiseInt(byteVector, inventorySize);
	std::cout << "inventory size: " << inventorySize << std::endl;
	for (int i = 0; i < inventorySize; ++i){
		serialiseInt(byteVector, inventory.at(i)->m_uid);
		std::cout << "item uid: " << inventory.at(i)->m_uid << std::endl;
	}
}

int Inventory::deserialise(char* buffer, int i)
{
	capacity = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	int inventorySize = deserialiseInt(buffer, i);	
	i = advanceFourBytes(i);

	for (int k = 0; k < inventorySize; ++k){
		inventoryMirror.push_back(deserialiseInt(buffer, i));
		i = advanceFourBytes(i);
	}

	return i;
}


Weapon::Weapon(DamageTypes _damageType, int _sidedDie, bool _twoHanded):
damageType(_damageType), sidedDie(_sidedDie), twoHanded(_twoHanded)
{

}

Weapon::Weapon()
{
	sidedDie = 0;
	twoHanded = false;
}

Weapon::~Weapon()
{

}

void Weapon::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "weapon" << std::endl;
	serialiseInt(byteVector, static_cast<int>(damageType));
	std::cout << "damage type: " << damageType << std::endl;
	serialiseInt(byteVector, sidedDie);
	std::cout << "sided die: " << sidedDie << std::endl;
	serialiseInt(byteVector, static_cast<int>(twoHanded));
	std::cout << "two handed: " << twoHanded << std::endl;
}

int Weapon::deserialise(char* buffer, int i)
{
	damageType = static_cast<DamageTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	sidedDie = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	twoHanded = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}

Armour::Armour(DamageTypes _resistance, DamageTypes _weakness, int _armourBonus):
resistance(_resistance), weakness(_weakness), armourBonus(_armourBonus)
{

}

Armour::Armour()
{

}

Armour::~Armour()
{

}

void Armour::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "armour" << std::endl;
	serialiseInt(byteVector, static_cast<int>(resistance));
	std::cout << "resistance: " << resistance << std::endl;
	serialiseInt(byteVector, static_cast<int>(weakness));
	std::cout << "weakness: " << weakness << std::endl;
	serialiseInt(byteVector, armourBonus);
	std::cout << "armour bonus: " << armourBonus << std::endl;
}

int Armour::deserialise(char* buffer, int i)
{
	resistance = static_cast<DamageTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	weakness = static_cast<DamageTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	armourBonus = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Wearable::Wearable(EquipSlots _slot):
slot(_slot)
{

}

Wearable::Wearable()
{

}

Wearable::~Wearable()
{

}

void Wearable::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "wearable: " << std::endl;
	serialiseInt(byteVector, static_cast<int>(slot));
	std::cout << "slot: " << slot << std::endl;
}

int Wearable::deserialise(char* buffer, int i)
{
	slot = static_cast<EquipSlots>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	return i;
}


Body::Body()
{
  slots.insert({HEAD, nullptr});
  slots.insert({LEFTHAND, nullptr});
  slots.insert({RIGHTHAND, nullptr});
  slots.insert({BODY, nullptr});
  slots.insert({NECK, nullptr});
  slots.insert({BACK, nullptr});
}

Body::~Body()
{

}

void Body::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "body" << std::endl;
	std::map<EquipSlots, GameObject*>::iterator it;
	int empty = 0;

	for (it = slots.begin(); it != slots.end(); ++it){
		serialiseInt(byteVector, static_cast<int>(it->first));
		std::cout << "equipped item: " << it->first << std::endl;
		if (it->second != nullptr){
			serialiseInt(byteVector, it->second->m_uid);
			std::cout << "item: " << it->second->m_uid << std::endl;
		} else {
			serialiseInt(byteVector, empty);
			std::cout << "no item: " << empty << std::endl;
		}
	}
}

int Body::deserialise(char* buffer, int i)
{
	int headKey = 0;
	int headValue = 0;
	int leftKey = 0;
	int leftValue = 0;
	int rightKey = 0;
	int rightValue = 0;
	int bodyKey = 0;
	int bodyValue = 0;
	int neckKey = 0;
	int neckValue = 0;
	int backKey = 0;
	int backValue = 0;

	
	headKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	headValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	leftKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	leftValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	rightKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	rightValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	bodyKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	bodyValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	neckKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	neckValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	backKey = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	backValue = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	slotsMirror.insert({headKey, headValue});
	slotsMirror.insert({leftKey, leftValue});
	slotsMirror.insert({rightKey, rightValue});
	slotsMirror.insert({bodyKey, bodyValue});
	slotsMirror.insert({neckKey, neckValue});
	slotsMirror.insert({backKey, backValue});

	return i;
}


Useable::Useable(UseableFunctionEnums func, int _numUses):
funcToDo(func), numUses(_numUses)
{

}

Useable::Useable()
{
	numUses = 0;
}

Useable::~Useable()
{

}

void Useable::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "useable" << std::endl;
	serialiseInt(byteVector, static_cast<int>(funcToDo));
	std::cout << "function: " << funcToDo << std::endl;
	serialiseInt(byteVector, numUses);
	std::cout << "number of uses: " << numUses << std::endl;
}

int Useable::deserialise(char* buffer, int i)
{
	funcToDo = static_cast<UseableFunctionEnums>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	numUses = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Healing::Healing(int _roll):
roll(_roll)
{

}

Healing::Healing()
{
	roll = 0;
}

Healing::~Healing()
{

}

void Healing::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "healing" << std::endl;
	serialiseInt(byteVector, roll);
	std::cout << "hit die: " << roll << std::endl;
}

int Healing::deserialise(char* buffer, int i)
{
	roll = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}

Damage::Damage(int _radius, int _roll, DamageTypes _type, int _chance):
radius(_radius), roll(_roll), type(_type), chance(_chance)
{

}

Damage::Damage()
{
	radius = 0;
	roll = 0;
	chance = 0;
}

Damage::~Damage()
{

}

void Damage::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "damage" << std::endl;
	serialiseInt(byteVector, radius);
	std::cout << "radius: " << radius << std::endl;
	serialiseInt(byteVector, roll);
	std::cout << "dice: " << roll << std::endl;
	serialiseInt(byteVector, static_cast<int>(type));
	std::cout << "type: " << type << std::endl;
	serialiseInt(byteVector, chance);
	std::cout << "chance: " << chance << std::endl;
}

int Damage::deserialise(char* buffer, int i)
{
	radius = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	roll = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	type = static_cast<DamageTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	chance = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


AreaDamage::AreaDamage(int _radius, int _roll, int _splashRadius, DamageTypes _type, int _chance):
radius(_radius), roll(_roll), splashRadius(_splashRadius), type(_type), chance(_chance)
{

}

AreaDamage::AreaDamage()
{

}

AreaDamage::~AreaDamage()
{

}

void AreaDamage::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "AOE" << std::endl;
	serialiseInt(byteVector, radius);
	std::cout << "radius: " << radius << std::endl;
	serialiseInt(byteVector, roll);
	std::cout << "roll: " << roll << std::endl;
	serialiseInt(byteVector, splashRadius);
	std::cout << "splashRadius: " << splashRadius << std::endl;
	serialiseInt(byteVector, static_cast<int>(type));
	std::cout << "type: " << type << std::endl;
	serialiseInt(byteVector, chance);
	std::cout << "chance: " << chance << std::endl;
}

int AreaDamage::deserialise(char* buffer, int i)
{
	radius = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	roll = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	splashRadius = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	type = static_cast<DamageTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	chance = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Status::Status(int _radius, StatusTypes _status, int _splashRadius):
radius(_radius), statusType(_status), splashRadius(_splashRadius)
{

}

Status::Status()
{
	radius = 0;
	splashRadius = 0;
}

Status::~Status()
{

}

void Status::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "status" << std::endl;
	serialiseInt(byteVector, radius);
	std::cout << "radius: " << radius << std::endl;
	serialiseInt(byteVector, static_cast<int>(statusType));
	std::cout << "status type: " << statusType << std::endl;
	serialiseInt(byteVector, splashRadius);
	std::cout << "splash radius: " << splashRadius << std::endl;
}

int Status::deserialise(char* buffer, int i)
{
	radius = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	statusType = static_cast<StatusTypes>(deserialiseInt(buffer, i));
	i = advanceFourBytes(i);

	splashRadius = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	return i;
}


Consumable::Consumable()
{

}

Consumable::~Consumable()
{

}

void Consumable::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "consumable" << std::endl;
}

int Consumable::deserialise(char* buffer, int i)
{
	return i;
}

Stairs::Stairs()
{

}

Stairs::~Stairs()
{

}

void Stairs::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "stairs" << std::endl;
}

int Stairs::deserialise(char* buffer, int i)
{
	return i;
}

StatusContainer::StatusContainer()
{
	for (int i = 0; i <= static_cast<int>(BLEEDING); ++i){
		statuses.insert({static_cast<StatusTypes>(i), std::make_pair(0, 0)}); // pair<damage, duration>
	}
}

StatusContainer::~StatusContainer()
{
}

void StatusContainer::serialise(std::vector<uint8_t> &byteVector)
{
	std::cout << "status container" << std::endl;
	for (int i = 0; i <= static_cast<int>(BLEEDING); ++i){
		serialiseInt(byteVector, std::get<0>(statuses.at(static_cast<StatusTypes>(i))));
		std::cout << "type" << std::get<0>(statuses.at(static_cast<StatusTypes>(i))) << std::endl;
		serialiseInt(byteVector, std::get<1>(statuses.at(static_cast<StatusTypes>(i))));
		std::cout << "duration: " << std::get<1>(statuses.at(static_cast<StatusTypes>(i))) << std::endl;
	}
}

int StatusContainer::deserialise(char* buffer, int i)
{
	for (int k = 0; k <= static_cast<int>(BLEEDING); ++k){	
		statuses.at(static_cast<StatusTypes>(k)).first = deserialiseInt(buffer, i);
		i = advanceFourBytes(i);

		statuses.at(static_cast<StatusTypes>(k)).second = deserialiseInt(buffer, i);
		i = advanceFourBytes(i);
	}

	return i;
}
