#include <iostream>
#include <functional>
#include <atomic>
#include "threadshuffle.h"

void t()
{
  std::cout << "trun start from " << local << std::endl;
  for(int i = 0; i < 3; ++i)
  {
    DEBUG_SHUFFLE
    std::cout << "trun " << i << " from " << local << std::endl;
  }
}

std::atomic_bool b = false;
void s()
{
  std::cout << "srun start from " << local << std::endl;
  for(int i = 0; i < 4; ++i)
  {
    DEBUG_SHUFFLE
    std::cout << "srun " << i << " from " << local << std::endl;
    b = true;
  }
}
void r()
{
  std::cout << "rrun start from " << local << std::endl;
  b = false; //starting position needs to be guaranteed the same every repetition
  for(int i = 0; i < 7; ++i)
  {
    DEBUG_SHUFFLE
    std::cout << "rrun " << i << " from " << local << std::endl;
    if(b)
    {
      b = false;
      ++i;
    }
  }
}

int main()
{
  std::cout << "start" << std::endl;
#if 0
  Shuffler th();
  th.start(&t);
  th.notify();
  return 0;
#endif
  shuffle(&t, 3);
  std::vector<std::function<void()>> v = {r, s, t};
  unique_shuffle(v);
}

