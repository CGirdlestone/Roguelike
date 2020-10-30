
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "SDL.h"

#include "MessageLog.h"
#include "Message.h"
#include "GameObject.h"




MessageLog::MessageLog(int x_buffer, int y_buffer, int log_size, EventManager* eventManager, std::map<int, GameObject*> *entities):
    m_x_buffer(x_buffer), m_y_buffer(y_buffer), m_log_size(log_size), m_eventManager(eventManager), m_entities(entities), m_i(0)
{
    m_x_buffer = x_buffer;
    m_y_buffer = y_buffer;
    m_eventManager = eventManager;
    m_entities = entities;
    m_eventManager->registerSystem(ONHIT, this);
    m_eventManager->registerSystem(CRITICALHIT, this);
    m_eventManager->registerSystem(ONMISS, this);
    m_eventManager->registerSystem(DAMAGE, this);
    m_eventManager->registerSystem(DEAD, this);
    m_eventManager->registerSystem(EXPGAIN, this);
    m_eventManager->registerSystem(ONPICKUP, this);
	m_eventManager->registerSystem(DROP, this);
	m_eventManager->registerSystem(MESSAGE, this);
}

MessageLog::~MessageLog()
{

}

void MessageLog::addMessage(std::string msg, SDL_Color colour)
{
    //
    Message _msg = Message(msg, colour);

    m_messageQueue.push_back(_msg);

    if (static_cast<int>(m_messageQueue.size()) > m_y_buffer) {
        m_i++;
    }

    if(static_cast<int>(m_messageQueue.size()) >= m_log_size){
       m_messageQueue.erase(m_messageQueue.begin());
       m_i--;
    }
}

void MessageLog::addMessage(std::string msg)
{
    SDL_Color colour = {0xFF, 0xFF, 0xFF};

    addMessage(msg, colour);
}

std::vector<Message> MessageLog::getMessages()
{
    std::vector<Message> messages;

    if (static_cast<int>(m_messageQueue.size()) < m_y_buffer) {

        return m_messageQueue;
    }

    for (int n = m_i; n < m_i + m_y_buffer; ++n) {
        messages.push_back(m_messageQueue[n]);
    }

    return messages;
}

void MessageLog::ageMessages(Uint32 ticks)
{
    std::vector<Message> aliveMessages;

    if(static_cast<int>(m_messageQueue.size()) > 0){
        for(Message msg : m_messageQueue){
            if (msg.m_lifetime < ticks){
                msg.m_lifetime = 0;
            } else {
                msg.m_lifetime -= ticks;
            }

            if (msg.m_lifetime > 0){
                aliveMessages.push_back(msg);
            }
        }
    }

    m_messageQueue = aliveMessages;
}

void MessageLog::serialise(std::ofstream& file)
{
    utils::serialiseInt(file, m_i);

    utils::serialiseInt(file, static_cast<int>(m_messageQueue.size()));

    if (static_cast<int>(m_messageQueue.size() > 0)){
        for (auto& msg : m_messageQueue) {
            msg.serialise(file);
        }
    }
}

int MessageLog::deserialise(char* buffer, int i)
{
    m_i = utils::deserialiseInt(buffer, i);
    i = utils::advanceFourBytes(i);

    int numMessages = utils::deserialiseInt(buffer, i);
    i = utils::advanceFourBytes(i);

    if (numMessages > 0) {
        for (int j = 0; j < numMessages; ++j) {
            Message msg;
            i = msg.deserialise(buffer, i);
            m_messageQueue.push_back(msg);
        }
    }

    return i;
}

void MessageLog::notify(AttackEvent event)
{
  // to do
}

void MessageLog::notify(OnHitEvent event)
{
  std::string attackerName = m_entities->at(event.m_attacker_uid)->m_name;
  std::string defenderName = m_entities->at(event.m_defender_uid)->m_name;
  std::string msg = attackerName + " hits " + defenderName;
  addMessage(msg);
}


void MessageLog::notify(OnCriticalHitEvent event)
{
  std::string attackerName = m_entities->at(event.m_attacker_uid)->m_name;
  std::string defenderName = m_entities->at(event.m_defender_uid)->m_name;
  std::string msg = attackerName + " hits " + defenderName + " - CRITICAL HIT";
  addMessage(msg);
}

void MessageLog::notify(OnMissEvent event)
{
  std::string attackerName = m_entities->at(event.m_attacker_uid)->m_name;
  std::string defenderName = m_entities->at(event.m_defender_uid)->m_name;
  std::string msg = attackerName + " misses " + defenderName;
  addMessage(msg);
}


void MessageLog::notify(DamageEvent event)
{
	std::string msg;  
	std::string defenderName = m_entities->at(event.m_uid)->m_name;
  
	int dmg = event.m_damage;
	
 	if (dmg >= 0){
		msg = defenderName + " suffers " + std::to_string(dmg) + " points of damage!";
  } else {
		msg = defenderName + " heals for " + std::to_string(-1 * dmg) + " health points!";
	}
	addMessage(msg);
}

void MessageLog::notify(DeadEvent event)
{
  std::string defenderName = m_entities->at(event.m_uid)->m_name;
  std::string msg = defenderName + " has been slain!";
  addMessage(msg);
}

void MessageLog::notify(ExpGainEvent event)
{
    std::string expGain = "+" + std::to_string(event.m_xp) + " xp";
    SDL_Color colour = { 0x5e, 0xe9, 0xe9 };
    addMessage(expGain, colour);
}

void MessageLog::notify(OnPickUpEvent event)
{
  std::string name = m_entities->at(event.m_uid)->m_name;
  std::string msg = name + " has picked up " + event.m_name;
  addMessage(msg);
}

void MessageLog::notify(DropEvent event)
{
	std::string name = m_entities->at(event.m_item_uid)->m_name;
	std::string msg = name + " dropped!";
	addMessage(msg);
}

void MessageLog::notify(MessageEvent event)
{
	addMessage(event.m_msg);
}
