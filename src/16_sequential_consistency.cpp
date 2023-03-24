#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>

/* 这段代码中的assert将永远不会触发。这是因为发生在线程a中的时序也将同步到线程b中。 */
std::atomic<bool>
    x, y;
std::atomic<int> z;

void write_x_then_y()
{
  x.store(true); // ①
  y.store(true); // ②
}

void read_y_then_x()
{
  while (!y.load())
    ;           // ③
  if (x.load()) // ④
    ++z;        // ⑤
}

int main()
{
  x = false;
  y = false;
  z = 0;
  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);
  a.join();
  b.join();
  assert(z.load() != 0); // ⑥
}