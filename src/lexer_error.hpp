#ifndef LEXER_ERROR_HPP
#define LEXER_ERROR_HPP
#include <string>

enum class LexerError
{
  UnexpectedCharacter,
  UnterminatedComment,
  MalformedIdentifier,
  MalformedNumber,
  OutOfRangeNumber
};

std::string to_string(LexerError error);
#endif