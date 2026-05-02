#include "cli.hpp"

std::ostream &operator<<(std::ostream &os, Pad pad)
{
  for (int i = 0; i < pad.n; i++)
    os << ' ';
  return os;
}

std::vector<std::string> parse_args(int argc, char *argv[])
{
  std::vector<std::string> args;

  for (int i = 1; i < argc; i++)
  {
    args.emplace_back(argv[i]);
  }

  return args;
}
