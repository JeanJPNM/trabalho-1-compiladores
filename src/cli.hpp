#ifndef CLI_HPP
#define CLI_HPP

#include <iostream>
#include <vector>
#include <string>

#define STDIN_FILENAME "<stdin>"

struct Pad
{
  int n;

  Pad(int n) : n(n) {}
};

std::ostream &operator<<(std::ostream &os, Pad pad);

std::vector<std::string> parse_args(int argc, char *argv[]);

#endif