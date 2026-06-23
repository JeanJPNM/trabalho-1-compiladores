#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "nodes.hpp"
#include "parser.hpp"

// manual declaration of yylex to receive a reference to the Driver
#define YY_DECL yy::parser::symbol_type yylex(Driver &driver)
YY_DECL;

class Driver
{
  // log a token if trace_tokens is true
  void log_token(const yy::parser::symbol_type &token, const std::string &source);
  // manages allocated nodes to ensure they are properly
  // deallocated when the Driver is destroyed
  std::vector<std::unique_ptr<AST::Node>> nodes;
  // current yy::location converted into a range
  Range range();

public:
  // map keywords to their corresponding token
  // constructors, used by the lexer
  std::map<
      std::string,
      std::function<yy::parser::symbol_type(yy::location)>>
      keywords;
  // map punctuation characters to their
  // corresponding token constructors, used by the lexer
  std::map<std::string, std::function<yy::parser::symbol_type(yy::location)>> tokens;
  // current parsing location, updated by the lexer
  yy::location location;
  std::string filename;
  // sets bison's debug level
  bool trace_parsing;
  // sets flex's debug level
  bool trace_scanning;
  // when true, logs tokens as they are scanned
  bool trace_tokens;
  // set whenever a parsing error occurs, regardless
  // if whether the error is recoverable or not
  bool has_error;
  AST::Program *program;

  Driver();

  // passes ownership of the node to the Driver
  void track_node(AST::Node *node);
  yy::parser::symbol_type make_keyword_or_identifier(const std::string &s);
  yy::parser::symbol_type make_double_literal(const std::string &s);
  yy::parser::symbol_type make_integer_literal(const std::string &s);
  yy::parser::symbol_type make_punctuation_token(const std::string &s);
  yy::parser::symbol_type make_error(const std::string &s, LexerError code);

  int parse(const std::string &filename);
  void dump_tokens(const std::string &filename);
  void scan_begin();
  void scan_end();
};

#endif