#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include "SDL.h"

#include "Message.h"
#include "EventManager.h"
#include "System.h"
#include "Components.h"
#include "GameObject.h"

class MessageLog : System
{
public:
    MessageLog(int x_buffer, int y_buffer, int log_size, EventManager* eventManager, std::map<int, GameObject*> *entities);
    virtual ~MessageLog();
    void addMessage(std::string msg, SDL_Color colour);
    void addMessage(std::string msg);
    std::vector<Message> getMessages();
    int getm_y_buffer(){return m_y_buffer;};
    void ageMessages(Uint32 ticks);
    void purgeLog() { m_messageQueue.clear(); m_i = 0; };
    void scrollUp() { m_i = std::min(static_cast<int>(m_messageQueue.size()) - m_y_buffer, m_i + 1); };
    void scrollDown() { m_i = std::max(0, m_i - 1); };

    int getNumMessages() { return static_cast<int>(m_messageQueue.size()); };
    int getLogIndex() { return m_i; };
    void setLogIndex(int i) { m_i = i; };

    void serialise(std::ofstream& file);
    int deserialise(char* buffer, int i);

    virtual void notify(AttackEvent event);
    virtual void notify(OnHitEvent event);
    virtual void notify(OnCriticalHitEvent event);
    virtual void notify(OnMissEvent event);
    virtual void notify(DamageEvent event);
    virtual void notify(DeadEvent event);
    virtual void notify(ExpGainEvent event);
    virtual void notify(OnPickUpEvent event);
	virtual void notify(DropEvent event);
	virtual void notify(MessageEvent event);

protected:


private:
    std::vector<Message> m_messageQueue;
    int m_x_buffer;
    int m_y_buffer;
    int m_log_size;
    EventManager* m_eventManager;
    std::map<int, GameObject*> *m_entities;
    int m_i;
};

#endif
