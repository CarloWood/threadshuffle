#include "threadshuffle.h"

void shuffle(std::function<void()> target, int threads, int max_depth)
{
  std::vector<Shuffler> all_threads(threads, Shuffler(target)); //emplaced?
  for(auto &t : all_threads)
    t.start();
  if(iterate<Shuffler>(all_threads, max_depth))
  {
    for(auto &t : all_threads)
    {
      t.stop();
    }
  }
}

void unique_shuffle(std::vector<std::function<void()>> &targets, int max_depth)
{
  std::vector<Shuffler> all_threads;
  for(auto &f : targets)
    all_threads.emplace_back(f); //emplace using function
  for(auto &t : all_threads)
    t.start();
  if(iterate<Shuffler>(all_threads, max_depth, true)) //unique = true
  {
    for(auto &t : all_threads)
    {
      t.stop();
    }
  }
}

