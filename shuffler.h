#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

class Shuffler
{
  static std::mutex mutex;
  std::mutex ready_mutex;
  std::unique_lock<std::mutex> lk; //used thread local
  std::unique_lock<std::mutex> ready_lk; //used in main
  std::condition_variable cv;
  std::condition_variable ready_cv;
  std::thread th;
  std::function<void()> &task;
  bool con; //signal next debug point
  bool ready; //ready for next run
  bool running; //currently executing

 public:
  Shuffler(std::function<void()> &target): task(target),
      con(false), ready(true), running(false) {}

  Shuffler(Shuffler const &other): task(other.task),
      con(false), ready(true), running(false)  {} //copies target nothing else

  void start();

  void stop();

  void repeat();

  void run(); //the thread target

  void wait(); //called in thread only

  void notify();

  bool is_ready();
};

extern thread_local Shuffler* local;

