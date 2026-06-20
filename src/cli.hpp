#ifndef CLI_HPP
#define CLI_HPP

#include <iostream>
#include <vector>
#include <string>

#define STDIN_FILENAME "<stdin>"

// Utility class to facility printing indented text
// when dumping the AST
struct Pad
{
  int n;

  Pad(int n) : n(n) {}
};

std::ostream &operator<<(std::ostream &os, Pad pad);

// converts command line arguments to a vector of strings for easier handling
std::vector<std::string> parse_args(int argc, char *argv[]);

#endif