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

class Base
{
  int const id;
  int const max;
  int m_index;
 public:
  Base(int max, int id): id(id), max(max), m_index(0) {}

  bool done() const
  {
#ifdef DEBUG
    std::cout << id << (m_index != max ? " ready" : " not ready") << std::endl;
#endif
    return m_index >= max;
  }

  void use()
  {
#ifdef DEBUG
    std::cout << id << " notify " << m_index << std::endl;
    assert(m_index < max);
#endif
    ++m_index;
#ifndef DEBUG
    std::cout << id;
#endif
    nl = true;
  }

  void repeat()
  {
#ifdef DEBUG
    std::cout << id << " repeat " << m_index << std::endl;
    assert(m_index == max);
#endif
    m_index = 0;
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
  std::vector<Base> all;
  for (int i = 0; i < n; ++i)
    all.emplace_back(m, i);
  iterate<Base>(std::ref(all), depth, unique);
  std::cout << std::endl << "total: " << total << std::endl;
}

