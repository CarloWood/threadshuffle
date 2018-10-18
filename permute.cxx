#include <iostream>
#include <cassert>
#include "permute.h"

template<typename T>
iter<T>::iter(cont_t const& cont)
{
  max = cont.begin();
  choice.push_back(cont.begin());
  cur = choice.begin();
#if 0
  for(cont_t::iterator t = cont.begin(); t < cont.end(); ++t)
  {
    if(t->is_ready())
    {
      cur = choice.push_back(t);
      break;
    }
  }
#endif
}

template<typename T>
iter<T>::iter(iter const& old, iter::cont_t const& cont) //construct based on old, not a copy
{
  choice = old.choice;
  max = old.max;
  bool last = old.cur == old.choice.end() - 1;
  if(!old.cur->is_ready())
    choice.erase(old.cur);
  if(last)
  {
    while(max < cont.end() - 1)
    {
      ++max;
      if(max->is_ready())
      {
        choice.push_back(max);
        break;
      }
    }
  }
}

//T should have is_ready, notify() and repeat()
template<typename T>
bool iterate(std::vector<T> const& threads, unsigned int max_depth)
{
  if(threads.empty())
    return false;
  if(!threads.first.is_ready()) //objects are identical, if first is not ready, none are
    return true;
  std::vector<iter<T>> path();
  path.emplace_back(threads); //construct with referance to container
  path.back.notify();
  while(path.size() < max_depth)
  {
    path.emplace_back(path.back, threads); //not a copy
    if(path.back.end()) //we constructed an empty object
    {
      auto r = path.rbegin(); //iterate reverse
      do
      {
        ++r;
        if(r < path.rend())
          return true; //stop and clean up
        r->next();
      }
      while(r->end());
      path.erase(r, path.end());
      for(auto t : threads)
        t.repeat();
      for(auto p : path)
        p.get()->notify(); //rewalk path
    }
    else //ready
    {
      path.back.get()->notify();
    }
  }
  return false; //reached max_depth, dump everything
}

