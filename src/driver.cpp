#include "driver.hpp"

Driver::Driver()
    : trace_parsing(false),
      trace_scanning(false),
      trace_tokens(false),
      program(nullptr)
{
  using p = yy::parser;
  keywords["program"] = p::make_K_PROGRAM;
  keywords["begin"] = p::make_K_BEGIN;
  keywords["end"] = p::make_K_END;
  keywords["const"] = p::make_K_CONST;
  keywords["var"] = p::make_K_VAR;
  keywords["procedure"] = p::make_K_PROCEDURE;
  keywords["real"] = p::make_K_REAL;
  keywords["integer"] = p::make_K_INTEGER;
  keywords["read"] = p::make_K_READ;
  keywords["write"] = p::make_K_WRITE;
  keywords["while"] = p::make_K_WHILE;
  keywords["do"] = p::make_K_DO;
  keywords["if"] = p::make_K_IF;
  keywords["then"] = p::make_K_THEN;
  keywords["else"] = p::make_K_ELSE;
  keywords["for"] = p::make_K_FOR;
  keywords["to"] = p::make_K_TO;

  tokens[";"] = p::make_SEMI;
  tokens["("] = p::make_L_PAREN;
  tokens[")"] = p::make_R_PAREN;
  tokens[","] = p::make_COMMA;
  tokens[":"] = p::make_COLON;
  tokens["="] = p::make_SINGLE_EQUALS;
  tokens["*"] = p::make_OP_MUL;
  tokens["/"] = p::make_OP_DIV;
  tokens["+"] = p::make_OP_ADD;
  tokens["-"] = p::make_OP_SUB;
  tokens["."] = p::make_DOT;
  tokens[":="] = p::make_OP_ASSIGN;
  tokens["=="] = p::make_OP_EQ;
  tokens["<>"] = p::make_OP_NE;
  tokens["<"] = p::make_OP_LT;
  tokens["<="] = p::make_OP_LE;
  tokens[">"] = p::make_OP_GT;
  tokens[">="] = p::make_OP_GE;
}

int Driver::parse(const std::string &filename)
{
  this->filename = filename;
  location.initialize(&this->filename);

  scan_begin();
  yy::parser parser(*this);
  parser.set_debug_level(trace_parsing);
  int res = parser();
  scan_end();

  return res;
}

void Driver::dump_tokens(const std::string &filename)
{
  this->filename = filename;
  this->trace_tokens = true;
  location.initialize(&this->filename);

  scan_begin();

  while (true)
  {
    auto token = yylex(*this);
    if (token.kind() == yy::parser::symbol_kind::S_YYEOF)
      break;
  }

  scan_end();
}

void Driver::track_node(AST::Node *node)
{
  nodes.emplace_back(node);
}

void Driver::log_token(const yy::parser::symbol_type &token, const std::string &source)
{
  if (!trace_tokens)
    return;

  const char *name;
  if (token.kind() == yy::parser::symbol_kind::S_YYUNDEF)
    name = "erro";
  else
    name = token.name();
  std::cout << source << " - " << name << std::endl;
}

yy::parser::symbol_type Driver::make_keyword_or_identifier(const std::string &s)
{
  auto it = keywords.find(s);
  if (it != keywords.end())
  {
    auto token = it->second(location);
    log_token(token, s);
    return token;
  }

  auto node = new AST::Identifier(s);
  track_node(node);

  auto token = yy::parser::make_IDENTIFIER(node, location);
  log_token(token, s);
  return token;
}

yy::parser::symbol_type Driver::make_double_literal(const std::string &s)
{
  auto node = new AST::NumberLiteral(std::stod(s));
  track_node(node);
  auto token = yy::parser::make_NUMBER(node, location);

  log_token(token, s);
  return token;
}

yy::parser::symbol_type Driver::make_integer_literal(const std::string &s)
{
  auto node = new AST::NumberLiteral(std::stol(s));
  track_node(node);
  auto token = yy::parser::make_NUMBER(node, location);

  log_token(token, s);
  return token;
}

yy::parser::symbol_type Driver::make_punctuation_token(const std::string &s)
{
  auto it = tokens.find(s);
  yy::parser::symbol_type token =
      it != tokens.end() ? it->second(location)
                         : yy::parser::make_YYerror(location);

  log_token(token, s);
  return token;
}

yy::parser::symbol_type Driver::make_error(const std::string &s)
{
  auto token = yy::parser::make_YYUNDEF(location);
  log_token(token, s);

  return token;
}
