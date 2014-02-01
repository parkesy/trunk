#pragma once
#include <cstring>
/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** fisher-random-starting-position-generator.hpp
** This header provides a very simple fisher random starting game position.
** The generating algrithm comes from http://en.wikipedia.org/wiki/Chess960_starting_position (Polyhedral dice method)
**
** Author: Adam Parkes 2014
** -------------------------------------------------------------------------*/

// Rules for 960 setup based off 
// http://en.wikipedia.org/wiki/Chess960_starting_position
// kinda like the Polyhedral dice method

struct FisherRandomSetup
{
  static void Create(char* buffer)
  { 
    memset(buffer, 0, 8);
    // bishops get placed first, they need to be on opposite colours
    buffer[((rand() % 4) * 2)] = 'B';
    buffer[((rand() % 4) * 2) + 1] = 'B';
  
    // next the queen gets placed on one of the remaining 6 empty squars
    Place('Q', (rand() % 6) + 1, buffer);

    // next the knights get placed on two of the remaining 5 empty squars
    // Note: IMOP there is no good reason two random numbers couldn't be
    // used to place the knights... see the wiki page. 
    const int knight = ((rand() % 18) + 1);
    Place('N', (knight / 4) + 1, buffer); 
    Place('N', (knight % 4) + 1, buffer);

    // now the rooks get place around the king on the remaining three squars
    Place('R', 1, buffer);
    Place('K', 1, buffer);
    Place('R', 1, buffer);
  }
private:
  static void Place(char character, int atEmptyIndex, char* buffer)
  {
    // empty squar hunter
    int placement = 0;
    for(int i = atEmptyIndex; i > 0;)
    {
      if (buffer[placement % 8] != 0)
	++placement;
      
      if (buffer[placement % 8] == 0 && i)
      {
	--i;
	++placement;
      }
    }
    --placement;
    buffer[(placement) % 8] = character;
  }
};
