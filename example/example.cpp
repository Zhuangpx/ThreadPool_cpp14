/*
 * @Autor: violet apricity ( Zhuangpx )
 * @Date: 2023-04-18 02:25:46
 * @LastEditors: violet apricity ( Zhuangpx )
 * @LastEditTime: 2023-04-18 11:36:26
 * @FilePath: \ThreadPool_cpp14\example\example.cpp
 * @Description:  Zhuangpx : Violet && Apricity:/ The warmth of the sun in the winter /
 */
#include <iostream>
#include <random>

#include "../include/z_thread_pool.h"

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);


void simulate_hard_computation() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

// Simple function that adds multiplies two numbers and prints the result
void multiply(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
}

// Same as before but now we have an output parameter
void multiply_output(int & out, const int a, const int b) {
  simulate_hard_computation();
  out = a * b;
  std::cout << a << " * " << b << " = " << out << std::endl;
}

// Same as before but now we have an output parameter
int multiply_return(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
  return res;
}


int main(int argc, char *argv[])
{
  std::cout << "example" << std::endl;
  // Create pool with 3 threads
  zThreadPool pool(3);

  // Submit (partial) multiplication table
  for (int i = 1; i < 3; ++i) {
    for (int j = 1; j < 10; ++j) {
      pool.submit(multiply, i, j);
    }
  }

  // Submit function with output parameter passed by ref
  int output_ref;
  auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);

  // Submit function with return parameter
  auto future2 = pool.submit(multiply_return, 5, 3);

  // Wait for multiplication output to finish
  future1.get();
  std::cout << "Last operation result is equals to " << output_ref << std::endl;

  // Wait for multiplication output to finish
  int res = future2.get();
  std::cout << "Last operation result is equals to " << res << std::endl;

  return 0;
}
