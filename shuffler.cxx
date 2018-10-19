#include <cassert>
#include "shuffler.h"

thread_local Shuffler* local = nullptr;
std::mutex Shuffler::m_th_mutex;

void Shuffler::start()
{
  m_mn_lock = std::unique_lock<std::mutex>(m_mn_mutex);
  m_thread = std::thread(&Shuffler::run, this);
  m_mn_convar.wait(m_mn_lock, [this]{return m_running;});
}

void Shuffler::run() //the thread target
{
  m_th_lock = std::unique_lock<std::mutex>(m_th_mutex);
  local = this;
  do
  {
    {
      std::lock_guard<std::mutex> lg(m_mn_mutex);
      m_running = true;
      m_done = false;
      m_con = false;
    }
    m_mn_convar.notify_one();
    m_task();
    m_done = true;
    m_th_convar.wait(m_th_lock, [this]{return !m_done;});
  }
  while (m_running);
  m_th_lock.unlock();
}

void Shuffler::stop()
{
  {
    std::lock_guard<std::mutex> lg(m_th_mutex);
    assert(m_running);
    assert(m_done);
    assert(!m_con);
    m_running = false;
    m_done = false;
  }
  m_th_convar.notify_one();
  m_thread.join();
}

void Shuffler::repeat()
{
  {
    std::lock_guard<std::mutex> lg(m_th_mutex);
    assert(m_running);
    assert(m_done);
    assert(!m_con);
    m_done = false;
    m_con = true;
  }
  m_th_convar.notify_one();
  m_mn_convar.wait(m_mn_lock, [this]{return !m_con;});
}

void Shuffler::wait() //called in thread only, in task through DEBUG_SHUFFLE
{
  m_th_convar.wait(m_th_lock, [this]{return m_con;});
  {
    std::lock_guard<std::mutex> lg(m_mn_mutex);
    m_con = false;
  }
  m_mn_convar.notify_one();
}

void Shuffler::use()
{
  {
    std::lock_guard<std::mutex> lg(m_th_mutex);
    assert(m_running);
    assert(!m_done);
    assert(!m_con);
    m_con = true;
  }
  m_th_convar.notify_one();
  m_mn_convar.wait(m_mn_lock, [this]{return !m_con;});
}

bool Shuffler::done()
{
  std::lock_guard<std::mutex> lg(m_th_mutex);
  return m_done;
}

