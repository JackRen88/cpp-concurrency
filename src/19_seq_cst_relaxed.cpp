#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

const int kLoopCount = 100000000;

using namespace std;

void increment(atomic<int> *value, memory_order order)
{
  for (int i = 0; i < kLoopCount; i++)
  {
    value->fetch_add(1, order);
  }
}

void thread_worker(atomic<int> *value, memory_order order)
{
  thread t1(increment, value, order);
  thread t2(increment, value, order);
  thread t3(increment, value, order);
  t1.join();
  t2.join();
  t3.join();
}

int main()
{

  atomic<int> a(0);
  atomic<int> b(0);

  auto start = chrono::steady_clock::now();
  thread_worker(&a, memory_order_relaxed);
  auto end = chrono::steady_clock::now();
  auto time1 = chrono::duration_cast<chrono::milliseconds>(end - start);
  std::cout << "memory_order_relaxed a:" << a << std::endl;

  start = chrono::steady_clock::now();
  thread_worker(&b, memory_order_seq_cst);
  end = chrono::steady_clock::now();
  auto time2 = chrono::duration_cast<chrono::milliseconds>(end - start);
  std::cout << "memory_order_seq_cst b:" << b << std::endl;

  cout << "Relaxed order cost " << time1.count() << "ms" << endl;
  cout << "Seq_cts order cost " << time2.count() << "ms" << endl;

  return 0;
}