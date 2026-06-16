#include "lexer_error.hpp"

std::string to_string(LexerError error)
{
  switch (error)
  {
  case LexerError::UnexpectedCharacter:
    return "unexpected character";
  case LexerError::UnterminatedComment:
    return "unterminated comment";
  case LexerError::MalformedIdentifier:
    return "malformed identifier";
  case LexerError::MalformedNumber:
    return "malformed number";
  case LexerError::OutOfRangeNumber:
    return "out of range number";
  default:
    return "unknown error";
  }
}