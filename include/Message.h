#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <fstream>
#include "SDL.h"

class Message
{
public:
      Message(std::string msg, SDL_Color colour);
      Message();
      ~Message();

      void serialise(std::ofstream& file);
      int deserialise(char* buffer, int i);

      std::string m_msg;
      SDL_Color m_colour;
      Uint32 m_lifetime;
};

#endif
