#pragma once
#include <vector>
#include <functional>

// depthwise iteration over permutations of the order in which shuffler objects are used 
// amount of uses are consistant for the same order in wich they are called every time,
// but not neccesarily for each permutation
// options availability are checked at the start of each permutation branch
// shuffler objects are deemed identical, if one permutation contains n uses, the same permutation
// with every use swapped with another shuffler object's they are considered a duplicate
// objects will have an done() function that returns true when it should no longer be used
// all objects need to get called with repeat() after they are all done() before using them
// if an object done() it won't be for the rest of the permutation

//T should have done()
template<class T>
class iter
{
  using cont_t = typename std::vector<T>;
  using cont_it = typename cont_t::iterator;
  using refs_t = typename std::vector<cont_it>;
  using refs_it = typename refs_t::iterator;
  cont_it m_max_end;
  cont_it m_max;
  refs_t m_choice;
  refs_it m_cur;
  void max_up() {if (m_max < m_max_end) m_choice.push_back(m_max++);} //max has not been used yet
 public:
  iter(cont_t& cont)
  {
    m_max = cont.begin();
    m_max_end = cont.end();
    max_up();
    m_cur = m_choice.begin();
  }

  iter(refs_t&& refs)
  {
    m_choice = std::move(refs);
    m_cur = m_choice.begin();
  }

  iter(iter& old, int i) //construct based on old, not a copy, i is not used
  {
    if (old.get()->done())
    {
      m_choice = refs_t(old.m_choice.begin(), old.m_cur);
      m_choice.insert(m_choice.end(), old.m_cur + 1, old.m_choice.end());
    }
    else
    {
      m_choice = old.m_choice;
    }
    m_max = old.m_max;
    m_max_end = old.m_max_end;
    if (old.m_cur == old.m_choice.end() - 1)
      max_up();
    m_cur = m_choice.begin();
  }

  void next()
  {
    ++m_cur;
  }

  bool end() const
  {
    return m_cur == m_choice.end();
  }

  cont_it get() const
  {
    return *m_cur;
  }
};

template<typename T>
bool iterate(std::vector<T>& threads, unsigned int max_depth, bool unique = false)
{
  if (threads.empty())
    return false;
  std::vector<iter<T>> path;
  if (unique)
  {
    using cont_it = typename std::vector<T>::iterator;
    std::vector<cont_it> active_threads;
    for (auto i = threads.begin(); i < threads.end(); ++i)
    {
      if (!i->done())
        active_threads.push_back(i);
    }
    if (active_threads.empty())
      return true;
    path.emplace_back(std::move(active_threads)); //construct with refs to not ready objects, unique
  }
  else
  {
    if (threads.front().done()) //objects are identical, if first is not waiting, none are
      return true;
    path.emplace_back(std::ref(threads)); //construct with referance to container
  }
  path.back().get()->use();
  while (path.size() != max_depth)
  {
    path.emplace_back(std::ref(path.back()), 1); //not a copy
    if (path.back().end()) //we constructed an empty object
    {
      do
      {
        path.pop_back();
        if (path.empty())
          return true; //stop and clean up
        path.back().next();
      }
      while (path.back().end());
      for (auto &t : threads) //reset
        t.repeat();
      for (auto p : path)
        p.get()->use(); //rewalk path
    }
    else //ready
    {
      path.back().get()->use();
    }
  }
  return false; //reached max_depth, dump everything
}

