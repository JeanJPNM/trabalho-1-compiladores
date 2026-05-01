#include "nodes.hpp"
#include "driver.hpp"

int main()
{

  Driver driver;
  int res = driver.parse("input.txt");
  if (res == 0)
    std::cout << "Parsing successful!" << std::endl;
  else
    std::cout << "Parsing failed with code " << res << std::endl;

  return res;
}