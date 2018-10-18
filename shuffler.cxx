#include <cassert>
#include "shuffler.h"

thread_local Shuffler* local = nullptr;
std::mutex Shuffler::mutex;

void Shuffler::start()
{
  ready_lk = std::unique_lock<std::mutex>(ready_mutex);
  th = std::thread(&Shuffler::run, this);
  ready_cv.wait(ready_lk, [this]{return running;});
}

void Shuffler::run() //the thread target
{
  lk = std::unique_lock<std::mutex>(mutex);
  local = this;

  do
  {
    {
      std::lock_guard<std::mutex> lg(ready_mutex);
      running = true;
      ready = false;
      con = false;
    }
    ready_cv.notify_one();
    task();
    ready = true;
    cv.wait(lk, [this]{return !ready;});
  }
  while(running);
  lk.unlock();
}

void Shuffler::stop()
{
  {
    std::lock_guard<std::mutex> lg(mutex);
    assert(running);
    assert(ready);
    assert(!con);
    running = false;
    ready = false;
  }
  cv.notify_one();
  th.join();
}

void Shuffler::repeat()
{
  {
    std::lock_guard<std::mutex> lg(mutex);
    assert(running);
    assert(ready);
    assert(!con);
    ready = false;
    con = true;
  }
  cv.notify_one();
  ready_cv.wait(ready_lk, [this]{return !con;});
}

void Shuffler::wait() //called in thread only, in task through DEBUG_SHUFFLE
{
  cv.wait(lk, [this]{return con;});
  {
    std::lock_guard<std::mutex> lg(ready_mutex);
    con = false;
  }
  ready_cv.notify_one();
}

void Shuffler::notify()
{
  {
    std::lock_guard<std::mutex> lg(mutex);
    assert(running);
    assert(!ready);
    assert(!con);
    con = true;
  }
  cv.notify_one();
  ready_cv.wait(ready_lk, [this]{return !con;});
}

bool Shuffler::is_ready()
{
  std::lock_guard<std::mutex> lg(mutex);
  return ready;
}

