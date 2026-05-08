#ifndef RANGE_HPP
#define RANGE_HPP

#include <string>

class Position
{
public:
  int line, column;
  Position(int line, int column) : line(line), column(column) {}

  std::string to_string() const;
};

class Range
{
public:
  Position start, end;
  Range(Position start, Position end) : start(start), end(end) {}
};

#endif