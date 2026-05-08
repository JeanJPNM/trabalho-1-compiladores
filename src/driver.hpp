#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "nodes.hpp"
#include "parser.hpp"

#define YY_DECL yy::parser::symbol_type yylex(Driver &driver)
YY_DECL;

class Driver
{
  void log_token(const yy::parser::symbol_type &token, const std::string &source);
  std::vector<std::unique_ptr<AST::Node>> nodes;
  Range range();

public:
  std::map<
      std::string,
      std::function<yy::parser::symbol_type(yy::location)>>
      keywords;
  std::map<std::string, std::function<yy::parser::symbol_type(yy::location)>> tokens;
  yy::location location;
  std::string filename;
  bool trace_parsing;
  bool trace_scanning;
  bool trace_tokens;
  AST::Program *program;

  Driver();

  void track_node(AST::Node *node);
  yy::parser::symbol_type make_keyword_or_identifier(const std::string &s);
  yy::parser::symbol_type make_double_literal(const std::string &s);
  yy::parser::symbol_type make_integer_literal(const std::string &s);
  yy::parser::symbol_type make_punctuation_token(const std::string &s);
  yy::parser::symbol_type make_error(const std::string &s);

  int parse(const std::string &filename);
  void dump_tokens(const std::string &filename);
  void scan_begin();
  void scan_end();
};

#endif