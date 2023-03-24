#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>

/* memory_order_release对应了写操作，memory_order_acquire对应了读操作，memory_order_acq_rel对应了既读又写。
同一个原子变量上的acquire和release操作将引入synchronizes - with关系 */

/* acq - rel 模型有如下保证：

1).同一个对象上的原子操作不允许被乱序。
2).release操作禁止了所有在它之前的读写操作与在它之后的写操作乱序。
3).acquire操作禁止了所有在它之前的读操作与在它之后的读写操作乱序。 */

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y()
{
  x.store(true, std::memory_order_relaxed); // ①
  y.store(true, std::memory_order_release); // ②
}

void read_y_then_x()
{
  while (!y.load(std::memory_order_acquire))
    ; // ③
  if (x.load(std::memory_order_relaxed))
    ++z; // ④
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
  assert(z.load() != 0); // ⑤
}
