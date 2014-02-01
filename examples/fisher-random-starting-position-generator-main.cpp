#include <iostream>
#include <ctime>
#include <cstdlib>

#include "fisher-random-starting-position-generator.hpp"
using namespace std;

int main()
{
  srand(time(NULL));
  char buffer[8];
  for (int j = 0; j < 10 ; ++j)
  {
    FisherRandomSetup::Create(buffer);
    for (int  i = 0; i < 8; ++i)
      cout << buffer[i];
    cout << endl;
  }
}
