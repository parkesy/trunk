#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "fisher-random-starting-position-generator.hpp"
using namespace std;

char spf_buffer[] = 
  "%02i:%02i:%02i.%1i %02i:%02i:%02i.%01i"
  "inc:%-04i     inc:%-04i"
  "%-21s"
  "960:%8s Beep:%-3s"
  "<By Parkesy>  Vol:%-3s";

char buffer[5*22]={0};

enum {
  P_HOUR = 0,
  P_MINUTE,
  P_SECOND,
  P_MILLISECOND,
  P_INC,
  P_MAX_PLAYER_NUM_PARAMETERS,
  P_FISHER_RANDOM = P_MAX_PLAYER_NUM_PARAMETERS * 2,
  P_MAX_DISPLAY_PARAMETERS,
  BUTTON_P1 = 2,
  BUTTON_P2 = 3,
  BUTTON_START = 4,
  BUTTON_NEXT = 5,
  BUTTON_PLUS = 6,
  BUTTON_MINUS = 7
};

int values[] = {0,0,0,0,0, 0,0,0,0,0};

int mods[] = {24,60,60,1,200, 24,60,60,1,200};
char fisherRandom[9] = {'-','-','-','-','-','-','-','-',0};
int current = 0;
bool started = false;
int activePlayer = 0;

void PrintBuffer()
{
  for (int j = 0; j < 5; ++j)
  {
    for (int i = 0 ; i < 21; ++i)
      std::cout << buffer[j*21 + i];
    std::cout << std::endl;
  }
}


void Handler(int sig)
{
  printf("nexiting...(%d)n", sig);
  exit(0);
}

void perror_exit(char* error)
{
  perror(error);
  Handler(9);
}

void HandleIncrement(int index)
{
  if (!started)
    if (index < 10)
      values[index] = (values[index] + 1) % mods[current];
    else
      FisherRandomSetup::Create(fisherRandom);
}

void HandleDecrement(int index)
{
  if (!started)
    values[current] = (values[current] - 1) % mods[current]; 
}

void ToggleStart()
{
  started = !started;
}

void IncrementClock(int playerId, int milliDiff)
{
  int offset = playerId*(P_INC + P_MAX_PLAYER_NUM_PARAMETERS);

  values[P_MILLISECOND + offset] += milliDiff;
  while(values[P_MILLISECOND + offset] >= 10)
  {
    values[P_MILLISECOND + offset] -= 10;
    ++values[P_SECOND + offset];
  }
  
  while(values[P_SECOND + offset] >= 60)
  {
    values[P_SECOND + offset] -= 60;
    ++values[P_MINUTE + offset];
  }
  while(values[P_MINUTE + offset] >= 60)
  {
    values[P_MINUTE + offset] -= 60;
    ++values[P_HOUR + offset];
  }
  values[P_HOUR + offset] = values[P_HOUR + offset] % 24;
}

void DecrementClock(int playerId, int millidiff)
{
  int offset = playerId*(P_MAX_PLAYER_NUM_PARAMETERS);

  values[P_MILLISECOND + offset] -= millidiff;
  while(values[P_MILLISECOND + offset] < 0)
  {
    values[P_MILLISECOND + offset] += 10;
    --values[P_SECOND + offset];
  }
  
  while(values[P_SECOND + offset] < 0)
  {
    values[P_SECOND + offset] += 60;
    --values[P_MINUTE + offset];
  }

  while(values[P_MINUTE + offset] < 0)
  {
    values[P_MINUTE + offset] += 60;
    --values[P_HOUR + offset];
  }
  if (values[P_HOUR + offset] < 0)
  {
    values[P_HOUR + offset] = 0;
    values[P_MINUTE + offset] = 0;
    values[P_SECOND + offset] = 0;
    values[P_MILLISECOND + offset] = 0;
  }
}

void TogglePlayer(int playerId)
{
  if (started)
    IncrementClock(playerId, values[P_INC + playerId * (P_INC +1)] * 10);
  activePlayer = playerId ^ 1;
}

void OnButtonPress(int value)
{
  switch(value)
  {
  case BUTTON_P1: printf("Playerone button\n"); TogglePlayer(0); break;
  case BUTTON_P2: printf("Playertwo button\n"); TogglePlayer(1); break;
  case BUTTON_START: printf("Start button\n"); ToggleStart(); break;
  case BUTTON_NEXT: printf("Next button\n"); current = (current + 1) % (P_MAX_DISPLAY_PARAMETERS); break;
  case BUTTON_PLUS: printf("Plus button\n"); HandleIncrement(current);  break;
  case BUTTON_MINUS: printf("Minus button\n"); HandleDecrement(current); break;
  } 
}

int main()
{
  struct input_event ev[64];
  int fd, rd, value, size = sizeof(struct input_event);
  char name[256] = "Unknown";
  char device[] = "/dev/input/by-id/usb-Dell_Dell_USB_Keyboard-event-kbd";
  if (getuid())
  {
    printf("Need to be root");
    exit(1);
  }

  if ((fd = open(device, O_RDONLY)) == -1)
  {
    printf("%s is not a vaild device.\n", device);
    exit(1);
  }

  ioctl(fd, EVIOCGNAME(sizeof(name)), name);
  printf("Reading from: %s (%s)\n", device, name);
  

  for(int i = 0 ;1; ++i)
  {
    if ((rd = read(fd, ev, size*64)) < size)
      perror("Read()");

    system("clear");
    value = ev[0].value;
    if (value != ' ' && ev[1].value == 1 && ev[1].type == 1)
    {
      OnButtonPress(ev[1].code);
    }
    if (started)
      DecrementClock(activePlayer, 1);

    sprintf(buffer, spf_buffer, 
	    values[P_HOUR],
	    values[P_MINUTE],
	    values[P_SECOND],
	    values[P_MILLISECOND],
	    values[P_HOUR + P_MAX_PLAYER_NUM_PARAMETERS],
	    values[P_MINUTE + P_MAX_PLAYER_NUM_PARAMETERS],
	    values[P_SECOND + P_MAX_PLAYER_NUM_PARAMETERS],
	    values[P_MILLISECOND + P_MAX_PLAYER_NUM_PARAMETERS],
	    values[P_INC],
	    values[P_INC + P_MAX_PLAYER_NUM_PARAMETERS],
	    !activePlayer ? "<<<<<<<<<<<<<<<<<<<<<" : ">>>>>>>>>>>>>>>>>>>>>",
	    fisherRandom,
	    started ? "Y" : "N",
	    "0");

    PrintBuffer();
  }
}
