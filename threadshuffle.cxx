#include "threadshuffle.h"

void shuffle(std::function<void()> target, int threads, int max_depth)
{
  std::vector<Shuffler> all_threads(threads, Shuffler(target)); //emplaced?
  for (auto& thread : all_threads)
    thread.start();
  if (iterate<Shuffler>(all_threads, max_depth))
  {
    for (auto& thread : all_threads)
      thread.stop();
  }
}

void unique_shuffle(std::vector<std::function<void()>>& targets, int max_depth)
{
  std::vector<Shuffler> all_threads;
  for (auto& fun : targets)
    all_threads.emplace_back(fun); //emplace using function
  for (auto& thread : all_threads)
    thread.start();
  if (iterate<Shuffler>(all_threads, max_depth, true)) //unique = true
  {
    for (auto& thread : all_threads)
      thread.stop();
  }
}

