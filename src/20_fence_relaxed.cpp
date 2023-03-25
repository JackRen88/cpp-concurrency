/*
 * Copyright (c) 2023 by GAC, All Rights Reserved.
 * @Author: renchengjin
 * @Date: 2023-03-25 19:43:17
 * @LastEditors: JackRen88
 * @LastEditTime: 2023-03-25 19:47:23
 * @Description:
 */
#include <thread>
#include <atomic>
#include <assert.h>
#include <iostream>

/* 这里我们仍然保留了原先读写操作的memory_order_relaxed。
但是我们在原先的①和②，以及③和④之间增加了一个fence。并且，分别使用了memory_order_release和memory_order_acquire。
它们位于中间来阻隔前后代码的乱序，由此阻止了问题的发生。 */

std::atomic<bool>
    x,
    y;
std::atomic<int> z;

void write_x_then_y()
{
  x.store(true, std::memory_order_relaxed); // ①
  std::atomic_thread_fence(std::memory_order_release);
  y.store(true, std::memory_order_relaxed); // ②
}

void read_y_then_x()
{
  while (!y.load(std::memory_order_relaxed))
    ; // ③
  std::atomic_thread_fence(std::memory_order_acquire);
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
  assert(z.load() != 0); // ⑥
}