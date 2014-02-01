#include <iostream>
#include <ctime>
#include <cstdlib>

#include "fisher-random-starting-position-generator.hpp"
using namespace std;

int main()
{
  srand(time(NULL));
  char buffer[9] = {0,0,0,0,0,0,0,0,0};
  for (int j = 0; j < 10 ; ++j)
  {
    FisherRandomSetup::Create(buffer);
    cout << buffer << endl;
  }
}
