#include "nodes.hpp"
#include "driver.hpp"

int main()
{

  Driver driver;
  driver.trace_tokens = true;

  driver.consume_all_tokens("input.txt");
}