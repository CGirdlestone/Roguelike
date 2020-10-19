
#include "SDL.h"

#include "Components.h"
#include <string>
#include <iostream>
#include <fstream>

void serialiseInt(std::ofstream& file, int x)
{
	uint8_t bytes[4];
	bytes[0] = ((x >> 24) & 0xFF);
	bytes[1] = ((x >> 16) & 0xFF);
	bytes[2] = ((x >> 8) & 0xFF);
	bytes[3] = ((x >> 0) & 0xFF);

	for (int i = 0; i < 4; ++i) {
		file.write(reinterpret_cast<char*>(&bytes[i]), sizeof(reinterpret_cast<char*>(&bytes[i])));
	}
}

void serialiseString(std::ofstream& file, std::string stringToSerialise)
{
	int size = static_cast<int>(stringToSerialise.length());
	int letterCode;

	serialiseInt(file, size);
	
	for (int i = 0; i < size; ++i){
		letterCode = static_cast<int>(stringToSerialise[i]);
		serialiseInt(file, letterCode);
	}
}

int deserialiseInt(char* buffer, int i)
{
	int value = (	(unsigned char)buffer[i + 0] << 24 |
					(unsigned char)buffer[i + 1] << 16 |
					(unsigned char)buffer[i + 2] << 8 |
					(unsigned char)buffer[i + 3]
				);

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

Position::Position():
	x(0), y(0)
{

}

Position::~Position()
{

}

void Position::serialise(std::ofstream& file)
{
	serialiseInt(file, x);
	serialiseInt(file, y);
}

int Position::deserialise(char* buffer, int i)
{
	x = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);

	y = deserialiseInt(buffer, i);
	i = advanceFourBytes(i);
	return i;
}


Renderable::Renderable(char _chr, SDL_Color _colour, int _spriteX, int _spriteY, int _sheet):
	chr(_chr), colour(_colour), spriteX(_spriteX), spriteY(_spriteY), sheet(_sheet)
{

}

Renderable::Renderable():
	chr(' '), colour({0xff, 0xff, 0xff}), sheet(0), spriteX(0), spriteY(0)
{

}

Renderable::~Renderable()
{

}

void Renderable::serialise(std::ofstream& file)
{
	serialiseInt(file, static_cast<int>(chr));
	serialiseInt(file, colour.r);
	serialiseInt(file, colour.g);
	serialiseInt(file, colour.b);
	serialiseInt(file, spriteX);
	serialiseInt(file, spriteY);
	serialiseInt(file, sheet);
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

Fighter::Fighter():
	maxHealth(0), health(0), power(0), defence(0), isAlive(true)
{

}

Fighter::~Fighter()
{

}

void Fighter::serialise(std::ofstream& file)
{
	int _isAlive = isAlive ? 1 : 0;

	serialiseInt(file, health);
	serialiseInt(file, maxHealth);
	serialiseInt(file, power);
	serialiseInt(file, defence);
	serialiseInt(file, _isAlive);
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

void Actor::serialise(std::ofstream& file)
{

}

int Actor::deserialise(char* buffer, int i)
{
	return i;
}


Player::Player():
	level(1), exp(0), next(100)
{

}

Player::~Player()
{

}

void Player::serialise(std::ofstream& file)
{
	serialiseInt(file, level);
	serialiseInt(file, exp);
	serialiseInt(file, next);
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

Item::Item():
	description(""), level(0)
{

}

Item::~Item()
{

}

void Item::serialise(std::ofstream& file)
{
	serialiseString(file, description);
	serialiseInt(file, level);
}

int Item::deserialise(char* buffer, int i)
{
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

AI::AI():
	exp(0), level(0)
{

}

AI::~AI()
{

}

void AI::serialise(std::ofstream& file)
{
	serialiseInt(file, exp);
	serialiseInt(file, level);
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

Inventory::Inventory():
	capacity(0)
{
	
}

Inventory::~Inventory()
{

}

void Inventory::serialise(std::ofstream& file)
{
	int inventorySize = static_cast<int>(inventory.size());

	serialiseInt(file, capacity);
	serialiseInt(file, inventorySize);
	for (int i = 0; i < inventorySize; ++i){
		serialiseInt(file, inventory.at(i)->m_uid);
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

Weapon::Weapon():
	damageType(NODAMAGETYPE), sidedDie(0), twoHanded(false)
{

}

Weapon::~Weapon()
{

}

void Weapon::serialise(std::ofstream& file)
{
	serialiseInt(file, static_cast<int>(damageType));
	serialiseInt(file, sidedDie);
	serialiseInt(file, static_cast<int>(twoHanded));
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

Armour::Armour():
	resistance(NODAMAGETYPE), weakness(NODAMAGETYPE), armourBonus(0)
{

}

Armour::~Armour()
{

}

void Armour::serialise(std::ofstream& file)
{
	serialiseInt(file, static_cast<int>(resistance));
	serialiseInt(file, static_cast<int>(weakness));
	serialiseInt(file, armourBonus);
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

Wearable::Wearable():
	slot(HEAD)
{

}

Wearable::~Wearable()
{

}

void Wearable::serialise(std::ofstream& file)
{
	serialiseInt(file, static_cast<int>(slot));
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

void Body::serialise(std::ofstream& file)
{
	std::map<EquipSlots, GameObject*>::iterator it;
	int empty = 0;

	for (it = slots.begin(); it != slots.end(); ++it){
		serialiseInt(file, static_cast<int>(it->first));
		if (it->second != nullptr){
			serialiseInt(file, it->second->m_uid);
		} else {
			serialiseInt(file, empty);
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

Useable::Useable():
	funcToDo(HEALING), numUses(0)
{
	numUses = 0;
}

Useable::~Useable()
{

}

void Useable::serialise(std::ofstream& file)
{
	serialiseInt(file, static_cast<int>(funcToDo));
	serialiseInt(file, numUses);
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

Healing::Healing():
	roll(0)
{

}

Healing::~Healing()
{

}

void Healing::serialise(std::ofstream& file)
{
	serialiseInt(file, roll);
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

Damage::Damage():
	radius(0), roll(0), type(NODAMAGETYPE), chance(0)
{

}

Damage::~Damage()
{

}

void Damage::serialise(std::ofstream& file)
{
	serialiseInt(file, radius);
	serialiseInt(file, roll);
	serialiseInt(file, static_cast<int>(type));
	serialiseInt(file, chance);
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

AreaDamage::AreaDamage() :
	radius(0), roll(0), splashRadius(0), type(NODAMAGETYPE), chance(0)
{

}

AreaDamage::~AreaDamage()
{

}

void AreaDamage::serialise(std::ofstream& file)
{
	serialiseInt(file, radius);
	serialiseInt(file, roll);
	serialiseInt(file, splashRadius);
	serialiseInt(file, static_cast<int>(type));
	serialiseInt(file, chance);
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

Status::Status() :
	radius(0), statusType(BURNED), splashRadius(0)
{

}

Status::~Status()
{

}

void Status::serialise(std::ofstream& file)
{
	serialiseInt(file, radius);
	serialiseInt(file, static_cast<int>(statusType));
	serialiseInt(file, splashRadius);
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

void Consumable::serialise(std::ofstream& file)
{

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

void Stairs::serialise(std::ofstream& file)
{

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

void StatusContainer::serialise(std::ofstream& file)
{
	for (int i = 0; i <= static_cast<int>(BLEEDING); ++i){
		serialiseInt(file, std::get<0>(statuses.at(static_cast<StatusTypes>(i))));

		serialiseInt(file, std::get<1>(statuses.at(static_cast<StatusTypes>(i))));
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
