#include "range.hpp"

std::string Position::to_string() const
{
  return std::to_string(line) + ":" + std::to_string(column);
}
