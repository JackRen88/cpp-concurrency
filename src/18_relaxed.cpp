#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>

/* 这是最弱的内存模型。 */

/* 这个模型下唯一可以保证的是：对于特定原子变量存在全局一致的修改顺序，除此以外不再有其他保证。
这意味着，即便是同样的代码，不同的线程可能会看到不同的执行顺序。 */

/* 对于下面这段代码：即便线程a认为，x是在y之前已经设置为true，但是对于线程b来说，并不一定能看到同样的结果，它在确认y已经为true之后，可能还会看到x为false，于是第③处的++ z没有执行，于是导致了assert触发。
 */

/* a，b两个线程是并行执行的，发生在线程a的事件并不要求同步到线程b。
 */

/* 总结一下，对于memory_order_relaxed来说：

尽管所有操作都是原子的，但是所有的事件不要求存在一个全局顺序
同一个线程内部有happens - before规则，但是线程之间可能会看到不同的顺序 */

/* relaxed memory order 约束太小，与Fence来一起使用
 */

std::atomic<bool>
    x,
    y;
std::atomic<int> z;

void write_x_then_y() // a
{
  x.store(true, std::memory_order_relaxed); // ①
  y.store(true, std::memory_order_relaxed); // ②
}

void read_y_then_x() // b
{
  while (!y.load(std::memory_order_relaxed))
    ;                                    // ③
  if (x.load(std::memory_order_relaxed)) // ④
    ++z;                                 // ⑤
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