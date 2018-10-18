#include <vector>
#include <iostream>
#include <cassert>
#include <functional>
#include "permute.h"

int const n = 3;
int const m = 3;
bool const unique = false;
int const depth = 1000;

bool nl = false;
int total = 1;

class base
{
  const int id;
  const int max;
  int i;
 public:
  base(int max, int id): id(id), max(max), i(0) {}

  bool is_ready() const
  {
#ifdef DEBUG
    std::cout << id << (i != max ? " ready" : " not ready") << std::endl;
#endif
    return i >= max;
  }

  void notify()
  {
#ifdef DEBUG
    std::cout << id << " notify " << i << std::endl;
    assert(i < max);
#endif
    ++i;
#ifndef DEBUG
    std::cout << id;
#endif
    nl = true;
  }

  void repeat()
  {
#ifdef DEBUG
    std::cout << id << " repeat " << i << std::endl;
    assert(i == max);
#endif
    i = 0;
    if(nl)
    {
      nl = false;
#ifndef DEBUG
      std::cout << std::endl;
#endif
      ++total;
    }
  }
};

int main()
{
  std::vector<base> all;
  for(int i = 0; i < n; ++i)
    all.push_back(base(m, i));
  iterate<base>(std::ref(all), depth, unique);
  std::cout << std::endl << "total: " << total << std::endl;
  return  0;
}

