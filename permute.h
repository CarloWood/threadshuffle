#pragma once
#include <vector>
#include <functional>

// depthwise iteration over permutations of the order in which shuffler objects are notified
// amount of notifies are consistant for the same order in wich they are called every time,
// but not neccesarily for each permutation
// options availability are checked at the start of each permutation branch
// shuffler objects are deemed identical, if one permutation contains n notifies, the same permutation
// with every notify swapped with another shuffler object's they are considered a duplicate
// objects will have an is_ready() function that returns true when it should no longer be notified
// all objects need to get called with repeat() after they are all is_ready() before notifying them
// if an object is_ready() it won't be for the rest of the permutation

//T should have is_ready()
template<class T>
class iter
{
  using cont_t = typename std::vector<T>;
  using cont_it = typename cont_t::iterator;
  using refs_t = typename std::vector<cont_it>;
  using refs_it = typename refs_t::iterator;
  cont_it max_end;
  cont_it max;
  refs_t choice;
  refs_it cur;
  void max_up() {if(max < max_end) choice.push_back(max++);} //max has not been notified yet
 public:
  iter(cont_t &cont)
  {
    max = cont.begin();
    max_end = cont.end();
    max_up();
    cur = choice.begin();
  }

  iter(refs_t &&refs)
  {
    choice = std::move(refs);
    cur = choice.begin();
  }

  iter(iter &old, int i) //construct based on old, not a copy, i is not used
  {
    if(old.get()->is_ready())
    {
      choice = refs_t(old.choice.begin(), old.cur);
      choice.insert(choice.end(), old.cur + 1, old.choice.end());
    }
    else
    {
      choice = old.choice;
    }
    max = old.max;
    max_end = old.max_end;
    if(old.cur == old.choice.end() - 1)
      max_up();
    cur = choice.begin();
  }

  void next()
  {
    ++cur;
  }

  bool end() const
  {
    return cur == choice.end();
  }

  cont_it get() const
  {
    return *cur;
  }
};

template<typename T>
bool iterate(std::vector<T> &threads, unsigned int max_depth, bool unique = false)
{
  if(threads.empty())
    return false;
  std::vector<iter<T>> path;
  if(unique)
  {
    using cont_it = typename std::vector<T>::iterator;
    std::vector<cont_it> active_threads;
    for(auto i = threads.begin(); i < threads.end(); ++i)
    {
      if(!i->is_ready())
        active_threads.push_back(i);
    }
    if(active_threads.empty())
      return true;
    path.emplace_back(std::move(active_threads)); //construct with refs to not ready objects, unique
  }
  else
  {
    if(threads.front().is_ready()) //objects are identical, if first is not waiting, none are
      return true;
    path.emplace_back(std::ref(threads)); //construct with referance to container
  }
  path.back().get()->notify();
  while(path.size() != max_depth)
  {
    path.emplace_back(std::ref(path.back()), 1); //not a copy
    if(path.back().end()) //we constructed an empty object
    {
      do
      {
        path.pop_back();
        if(path.empty())
          return true; //stop and clean up
        path.back().next();
      }
      while(path.back().end());
      for(auto &t : threads) //reset
        t.repeat();
      for(auto p : path)
        p.get()->notify(); //rewalk path
    }
    else //ready
    {
      path.back().get()->notify();
    }
  }
  return false; //reached max_depth, dump everything
}

