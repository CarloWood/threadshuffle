#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

class Shuffler
{
  static std::mutex m_th_mutex;
  std::mutex m_mn_mutex;
  std::unique_lock<std::mutex> m_th_lock; //used thread local
  std::unique_lock<std::mutex> m_mn_lock; //used in main
  std::condition_variable m_th_convar;
  std::condition_variable m_mn_convar;
  std::thread m_thread;
  std::function<void()> &m_task;
  bool m_con; //signal next debug point
  bool m_done; //ready for next run
  bool m_running; //currently executing

 public:
  Shuffler(std::function<void()>& target): m_task(target),
      m_con(false), m_done(true), m_running(false) {}

  Shuffler(Shuffler const& other): m_task(other.m_task),
      m_con(false), m_done(true), m_running(false) {} //copies target nothing else

  void start();

  void stop();

  void repeat();

  void run(); //the thread target

  void wait(); //called in thread only

  void use();

  bool done();
};

extern thread_local Shuffler* local;

