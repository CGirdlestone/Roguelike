
#include <string>
#include "Utils.h"
#include "SDL.h"

#include "Message.h"

Message::Message(std::string msg, SDL_Color colour)
{
    m_msg = msg;
    m_colour = colour;
    m_lifetime = 10000; // 10 seconds 
}

Message::Message():
    m_msg(""), m_colour({0xff, 0xff, 0xff})
{
    
}

Message::~Message()
{

}

void Message::serialise(std::ofstream& file)
{
    utils::serialiseInt(file, m_colour.r);
    utils::serialiseInt(file, m_colour.g);
    utils::serialiseInt(file, m_colour.b);

    utils::serialiseString(file, m_msg);
}

int Message::deserialise(char* buffer, int i)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    red = (uint8_t)utils::deserialiseInt(buffer, i);
    i = utils::advanceFourBytes(i);

    green = (uint8_t)utils::deserialiseInt(buffer, i);
    i = utils::advanceFourBytes(i);

    blue = (uint8_t)utils::deserialiseInt(buffer, i);
    i = utils::advanceFourBytes(i);

    m_colour = { red, green, blue };

    i = utils::deserialiseString(buffer, i, m_msg);

    return i;
}
