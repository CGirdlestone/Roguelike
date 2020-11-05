#include "Utils.h"
#include "SDL.h"

#include "Components.h"
#include <string>
#include <iostream>
#include <fstream>



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
	utils::serialiseInt(file, x);
	utils::serialiseInt(file, y);
}

int Position::deserialise(char* buffer, int i)
{
	x = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	y = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
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
	utils::serialiseInt(file, static_cast<int>(chr));
	utils::serialiseInt(file, colour.r);
	utils::serialiseInt(file, colour.g);
	utils::serialiseInt(file, colour.b);
	utils::serialiseInt(file, spriteX);
	utils::serialiseInt(file, spriteY);
	utils::serialiseInt(file, sheet);
}

int Renderable::deserialise(char* buffer, int i)
{
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	chr = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	red = (uint8_t)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);
	
	green = (uint8_t)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	blue = (uint8_t)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	colour = {red, green, blue};

	spriteX = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	spriteY = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	sheet = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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

	utils::serialiseInt(file, health);
	utils::serialiseInt(file, maxHealth);
	utils::serialiseInt(file, power);
	utils::serialiseInt(file, defence);
	utils::serialiseInt(file, _isAlive);
}

int Fighter::deserialise(char* buffer, int i)
{
	health = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	maxHealth = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	power = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	defence = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	isAlive = utils::deserialiseInt(buffer, i) == 1 ? true : false;
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, level);
	utils::serialiseInt(file, exp);
	utils::serialiseInt(file, next);
}

int Player::deserialise(char* buffer, int i)
{
	level = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	exp = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	next = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseString(file, description);
	utils::serialiseInt(file, level);
}

int Item::deserialise(char* buffer, int i)
{
	i = utils::deserialiseString(buffer, i, description);

	level = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	return i;
}

AI::AI(int _exp, int _level, std::string _damage):
exp(_exp), level(_level), damage(_damage)
{

}

AI::AI():
	exp(0), level(0), damage("")
{

}

AI::~AI()
{

}

void AI::serialise(std::ofstream& file)
{
	utils::serialiseInt(file, exp);
	utils::serialiseInt(file, level);
	utils::serialiseString(file, damage);
}

int AI::deserialise(char* buffer, int i)
{
	exp = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	level = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	i = utils::deserialiseString(buffer, i, damage);

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

	utils::serialiseInt(file, capacity);
	utils::serialiseInt(file, inventorySize);
	for (int i = 0; i < inventorySize; ++i){
		utils::serialiseInt(file, inventory.at(i)->m_uid);
	}
}

int Inventory::deserialise(char* buffer, int i)
{
	capacity = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	int inventorySize = utils::deserialiseInt(buffer, i);	
	i = utils::advanceFourBytes(i);

	for (int k = 0; k < inventorySize; ++k){
		inventoryMirror.push_back(utils::deserialiseInt(buffer, i));
		i = utils::advanceFourBytes(i);
	}

	return i;
}


Weapon::Weapon(DamageTypes _damageType, std::string _damage, bool _twoHanded):
	damageType(_damageType), damage(_damage), twoHanded(_twoHanded)
{

}

Weapon::Weapon():
	damageType(NODAMAGETYPE), damage(""), twoHanded(false)
{

}

Weapon::~Weapon()
{

}

void Weapon::serialise(std::ofstream& file)
{
	utils::serialiseInt(file, static_cast<int>(damageType));
	utils::serialiseString(file, damage);
	utils::serialiseInt(file, static_cast<int>(twoHanded));
}

int Weapon::deserialise(char* buffer, int i)
{
	damageType = static_cast<DamageTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	i = utils::deserialiseString(buffer, i, damage);

	twoHanded = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, static_cast<int>(resistance));
	utils::serialiseInt(file, static_cast<int>(weakness));
	utils::serialiseInt(file, armourBonus);
}

int Armour::deserialise(char* buffer, int i)
{
	resistance = static_cast<DamageTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	weakness = static_cast<DamageTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	armourBonus = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, static_cast<int>(slot));
}

int Wearable::deserialise(char* buffer, int i)
{
	slot = static_cast<EquipSlots>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

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
		utils::serialiseInt(file, static_cast<int>(it->first));
		if (it->second != nullptr){
			utils::serialiseInt(file, it->second->m_uid);
		} else {
			utils::serialiseInt(file, empty);
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

	
	headKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	headValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	leftKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	leftValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	rightKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	rightValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	bodyKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	bodyValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	neckKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	neckValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	backKey = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	backValue = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	slotsMirror.insert({headKey, headValue});
	slotsMirror.insert({leftKey, leftValue});
	slotsMirror.insert({rightKey, rightValue});
	slotsMirror.insert({bodyKey, bodyValue});
	slotsMirror.insert({neckKey, neckValue});
	slotsMirror.insert({backKey, backValue});

	return i;
}


Useable::Useable(std::string func, int _numUses, bool _ranged, bool _AOE):
	funcToDo(func), numUses(_numUses), ranged(_ranged), AOE(_AOE)
{

}

Useable::Useable():
	funcToDo(""), numUses(0), ranged(false), AOE(false)
{

}

Useable::~Useable()
{

}

void Useable::serialise(std::ofstream& file)
{
	utils::serialiseString(file, funcToDo);
	//utils::serialiseInt(file, static_cast<int>(funcToDo));
	utils::serialiseInt(file, numUses);
	utils::serialiseInt(file, (int)ranged);
	utils::serialiseInt(file, (int)AOE);
}

int Useable::deserialise(char* buffer, int i)
{
	i = utils::deserialiseString(buffer, i, funcToDo);
	//funcToDo = static_cast<UseableFunctionEnums>(utils::deserialiseInt(buffer, i));
	//i = utils::advanceFourBytes(i);

	numUses = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	ranged = (bool)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	AOE = (bool)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, roll);
}

int Healing::deserialise(char* buffer, int i)
{
	roll = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, radius);
	utils::serialiseInt(file, roll);
	utils::serialiseInt(file, static_cast<int>(type));
	utils::serialiseInt(file, chance);
}

int Damage::deserialise(char* buffer, int i)
{
	radius = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	roll = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	type = static_cast<DamageTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	chance = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, radius);
	utils::serialiseInt(file, roll);
	utils::serialiseInt(file, splashRadius);
	utils::serialiseInt(file, static_cast<int>(type));
	utils::serialiseInt(file, chance);
}

int AreaDamage::deserialise(char* buffer, int i)
{
	radius = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	roll = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	splashRadius = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	type = static_cast<DamageTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	chance = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
	utils::serialiseInt(file, radius);
	utils::serialiseInt(file, static_cast<int>(statusType));
	utils::serialiseInt(file, splashRadius);
}

int Status::deserialise(char* buffer, int i)
{
	radius = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	statusType = static_cast<StatusTypes>(utils::deserialiseInt(buffer, i));
	i = utils::advanceFourBytes(i);

	splashRadius = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

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
		utils::serialiseInt(file, std::get<0>(statuses.at(static_cast<StatusTypes>(i))));

		utils::serialiseInt(file, std::get<1>(statuses.at(static_cast<StatusTypes>(i))));
	}
}

int StatusContainer::deserialise(char* buffer, int i)
{
	for (int k = 0; k <= static_cast<int>(BLEEDING); ++k){	
		statuses.at(static_cast<StatusTypes>(k)).first = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);

		statuses.at(static_cast<StatusTypes>(k)).second = utils::deserialiseInt(buffer, i);
		i = utils::advanceFourBytes(i);
	}

	return i;
}

Animation::Animation():
	lifetime(150), current_lifetime(0)
{

}

Animation::~Animation()
{

}

void Animation::serialise(std::ofstream& file)
{
	utils::serialiseInt(file, (int)lifetime);
	utils::serialiseInt(file, (int)current_lifetime);
	utils::serialiseQueueInts(file, spriteSheets);
	utils::serialiseQueueInts(file, sprite_x);
	utils::serialiseQueueInts(file, sprite_y);
}

int Animation::deserialise(char* buffer, int i)
{
	lifetime = (Uint32)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	current_lifetime = (Uint32)utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	i = utils::deserialiseQueueInts(buffer, i, spriteSheets);
	i = utils::deserialiseQueueInts(buffer, i, sprite_x);
	i = utils::deserialiseQueueInts(buffer, i, sprite_y);

	return i;
}
