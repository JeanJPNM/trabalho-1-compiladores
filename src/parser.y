%require "3.8.1"

%skeleton "lalr1.cc"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <vector>
  #include <utility>
  #include "nodes.hpp"
  #include "lexer_error.hpp"

  class Driver;
}

%code {
  #include "driver.hpp"

  namespace yy {
    // utility to convert token ranges into AST ranges
    Range range_from(const yy::location &start, const yy::location &end);
  };
}

%param { Driver& driver }

%define parse.trace
%define parse.error custom
// needed for next expected token list to actually work
%define parse.lac full

%locations


%token <LexerError> INVALID "invalid token"
%token <AST::Identifier *> IDENTIFIER "id"
%token <AST::NumberLiteral *> NUMBER "num"
%token K_PROGRAM "program"
%token K_BEGIN "begin"
%token K_END "end"
%token K_CONST "const"
%token K_VAR "var"
%token K_PROCEDURE "procedure"
%token K_REAL "real"
%token K_INTEGER "integer"
%token K_READ "read"
%token K_WRITE "write"
%token K_WHILE "while"
%token K_DO "do"
%token K_IF "if"
%token K_THEN "then"
%token K_ELSE "else"
%token K_FOR "for"
%token K_TO "to"
%token SEMI ";"
%token L_PAREN "("
%token R_PAREN ")"
%token COMMA ","
%token COLON ":"
%token DOT "."
%token SINGLE_EQUALS "="
%token OP_MUL "*"
%token OP_DIV "/"
%token OP_ADD "+"
%token OP_SUB "-"
%token OP_ASSIGN ":="
%token OP_EQ "=="
%token OP_NE "<>"
%token OP_LT "<"
%token OP_LE "<="
%token OP_GT ">"
%token OP_GE ">="

// fix dangling else shift/reduce conflict
%precedence K_THEN
%precedence K_ELSE

%nterm program
%nterm <std::vector<AST::Declaration *>> program_declarations
%nterm <std::vector<AST::ConstantDeclaration *>> constant_declarations
%nterm <AST::ConstantDeclaration *> constant_declaration
%nterm <std::vector<AST::VariableDeclaration *>> variable_declarations
%nterm <AST::VariableDeclaration *> variable_declaration
%nterm <std::vector<AST::Identifier *>> identifiers
%nterm <AST::TypeAnnotation *> type_annotation
%nterm <AST::VariableType> variable_type
%nterm <std::vector<AST::ProcedureDeclaration *>> procedure_declarations
%nterm <AST::ProcedureDeclaration *> procedure_declaration
%nterm <std::vector<AST::ParameterDeclaration *>> parameter_list
%nterm <std::vector<AST::ParameterDeclaration *>> parameters
%nterm <AST::ParameterDeclaration *> parameter_group
%nterm <AST::Block *> block
%nterm <std::vector<AST::Statement *>> statements
%nterm <AST::Statement *> statement
%nterm <AST::Statement *> if_statement
%nterm <std::vector<AST::Identifier *>> argument_list
%nterm <AST::Condition *> condition
%nterm <AST::ComparisonOperator> relational_operator
%nterm <AST::Expression *> expression
%nterm <AST::BinaryOperator> additive_operator
%nterm <AST::Expression *> term
%nterm <AST::UnaryOperator> unary_operator
%nterm <AST::BinaryOperator> multiplicative_operator
%nterm <AST::Expression *> factor
%nterm <AST::VariableAssignment *> assignment

%%

program:
  K_PROGRAM IDENTIFIER ";" program_declarations block "." {
    Range range = range_from(@1, @6);
    driver.program = new AST::Program(range, $2, std::move($4), $5);
    driver.track_node(driver.program);
  };

program_declarations:
  constant_declarations variable_declarations procedure_declarations {
    std::vector<AST::Declaration *> result;
    result.insert(result.end(), $1.begin(), $1.end());
    result.insert(result.end(), $2.begin(), $2.end());
    result.insert(result.end(), $3.begin(), $3.end());

    $$ = std::move(result);
  };

constant_declarations:
  {
    $$ = std::vector<AST::ConstantDeclaration *>();
  }
  | constant_declarations constant_declaration {
    $1.push_back($2);
    $$ = std::move($1);
  };

constant_declaration:
  K_CONST IDENTIFIER "=" NUMBER ";" {
    Range range = range_from(@1, @5);
    $$ = new AST::ConstantDeclaration(range, $2, $4);
    driver.track_node($$);
  };

variable_declarations:
  {
    $$ = std::vector<AST::VariableDeclaration *>();
  }
  | variable_declarations variable_declaration {
    $1.push_back($2);
    $$ = std::move($1);
  };

variable_declaration: 
  K_VAR identifiers type_annotation ";" {
    Range range = range_from(@1, @4);
    $$ = new AST::VariableDeclaration(range, std::move($2), $3);
    driver.track_node($$);
  };

identifiers:
  IDENTIFIER {
    $$ = std::vector<AST::Identifier *>();
    $$.push_back($1);
  }
  | identifiers "," IDENTIFIER {
    $1.push_back($3);
    $$ = std::move($1);
  };


type_annotation:
  ":" variable_type {
    Range range = range_from(@1, @2);
    $$ = new AST::TypeAnnotation(range, $2);
    driver.track_node($$);
  }

variable_type:
  K_REAL { $$ = AST::VariableType::FLOAT; }
  | K_INTEGER { $$ = AST::VariableType::INT; };

procedure_declarations:
  {
    $$ = std::vector<AST::ProcedureDeclaration *>();
  }
  | procedure_declarations procedure_declaration {
    $1.push_back($2);
    $$ = std::move($1);
  };

procedure_declaration:
  K_PROCEDURE IDENTIFIER parameter_list ";" variable_declarations block ";" {
    Range range = range_from(@1, @7);
    $$ = new AST::ProcedureDeclaration(range, $2, std::move($3), std::move($5), $6);
    driver.track_node($$);
  };

parameter_list:
  "(" parameters ")" {
    $$ = std::move($2);
  };

parameters: 
  parameter_group {
    $$ = std::vector<AST::ParameterDeclaration *>();
    $$.push_back($1);
  }
  | parameters ";" parameter_group {
    $1.push_back($3);
    $$ = std::move($1);
  }
  | {
    $$ = std::vector<AST::ParameterDeclaration *>();
  };

parameter_group:
  identifiers type_annotation {
    Range range = range_from(@1, @2);
    $$ = new AST::ParameterDeclaration(range, std::move($1), $2);
    driver.track_node($$);
  };

block:
  K_BEGIN statements K_END {
    Range range = range_from(@1, @3);
    $$ = new AST::Block(range, std::move($2));
    driver.track_node($$);
  };

statements:
  {
    $$ = std::vector<AST::Statement *>();
  }
  | statements statement ";" {
    $1.push_back($2);
    $$ = std::move($1);
  };

statement:
  K_READ "(" identifiers ")" {
    Range range = range_from(@1, @4);
    $$ = new AST::ReadStatement(range, std::move($3));
    driver.track_node($$);
  }
  | K_WRITE "(" identifiers ")" {
    Range range = range_from(@1, @4);
    $$ = new AST::WriteStatement(range, std::move($3));
    driver.track_node($$);
  }
  | K_WHILE "(" condition ")" K_DO statement {
    Range range = range_from(@1, @6);
    $$ = new AST::WhileStatement(range, $3, $6);
    driver.track_node($$);
  }
  | if_statement {
    $$ = $1;
  }
  | IDENTIFIER "(" argument_list ")" {
    Range range = range_from(@1, @4);
    $$ = new AST::CallStatement(range, $1, std::move($3));
    driver.track_node($$);
  }
  | IDENTIFIER {
    // since the grammar doesn't support calls
    // that don't have at least one parameter
    // we can use this explicitly defined, but odd
    // syntax to support that behavior
    // specially since the alternative (identifier statements)
    // would just be useless

    Range range = range_from(@1, @1);
    $$ = new AST::CallStatement(range, $1, std::vector<AST::Identifier *>());
    driver.track_node($$);
  }
  | K_FOR assignment K_TO expression K_DO statement {
    Range range = range_from(@1, @6);
    $$ = new AST::ForStatement(range, $2, $4, $6);
    driver.track_node($$);
  }
  | assignment {
    $$ = $1;
  }
  | block {
    $$ = $1;
  };

if_statement:
  K_IF condition K_THEN statement K_ELSE statement {
    Range range = range_from(@1, @6);
    $$ = new AST::IfStatement(range, $2, $4, $6);
    driver.track_node($$);
  }
  | K_IF condition K_THEN statement {
    Range range = range_from(@1, @4);
    $$ = new AST::IfStatement(range, $2, $4, nullptr);
    driver.track_node($$);
  };

argument_list:
  IDENTIFIER {
    $$ = std::vector<AST::Identifier *>();
    $$.push_back($1);
  }
  | argument_list ";" IDENTIFIER {
    $1.push_back($3);
    $$ = std::move($1);
  };

condition:
  expression relational_operator expression {
    Range range = range_from(@1, @3);
    $$ = new AST::Condition(range, $1, $2, $3);
    driver.track_node($$);
  };

relational_operator:
  OP_EQ { $$ = AST::ComparisonOperator::EQUAL; }
  | OP_NE { $$ = AST::ComparisonOperator::NOT_EQUAL; }
  | OP_LT { $$ = AST::ComparisonOperator::LESS_THAN; }
  | OP_LE { $$ = AST::ComparisonOperator::LESS_THAN_OR_EQUAL; }
  | OP_GT { $$ = AST::ComparisonOperator::GREATER_THAN; }
  | OP_GE { $$ = AST::ComparisonOperator::GREATER_THAN_OR_EQUAL; };

expression:
  term {
    $$ = $1;
  }
  | expression additive_operator term {
    Range range = range_from(@1, @3);
    $$ = new AST::BinaryExpression(range, $1, $2, $3);
    driver.track_node($$);
  };

additive_operator:
  "+" { $$ = AST::BinaryOperator::ADD; }
  | "-" { $$ = AST::BinaryOperator::SUBTRACT; };

term:
  factor {
    $$ = $1;
  }
  | unary_operator factor {
    Range range = range_from(@1, @2);
    $$ = new AST::UnaryExpression(range, $1, $2);
    driver.track_node($$);
  }
  | term multiplicative_operator factor {
    Range range = range_from(@1, @3);
    $$ = new AST::BinaryExpression(range, $1, $2, $3);
    driver.track_node($$);
  };

unary_operator:
  "+" { $$ = AST::UnaryOperator::PLUS; }
  | "-" { $$ = AST::UnaryOperator::MINUS; };

multiplicative_operator:
  "*" { $$ = AST::BinaryOperator::MULTIPLY; } 
  | "/" { $$ = AST::BinaryOperator::DIVIDE; };

factor:
  IDENTIFIER { $$ = $1; }
  | NUMBER { $$ = $1; }
  | "(" expression ")" { $$ = $2; };

assignment:
  IDENTIFIER OP_ASSIGN expression {
    Range range = range_from(@1, @3);
    $$ = new AST::VariableAssignment(range, $1, $3);
    driver.track_node($$);
  };
%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}

Range yy::range_from(const yy::location &first, const yy::location &last)
{
  return Range(
    Position(first.begin.line, first.begin.column),
    Position(last.end.line, last.end.column)
  );
}

void yy::parser::report_syntax_error(const yy::parser::context& ctx) const
{
  std::cerr << ctx.location() << " - ";

  const symbol_type &lookahead = ctx.lookahead();
  std::cerr << "syntax error: unexpected " << symbol_name(lookahead.kind());

  // Exibe o código de erro léxico original propagado pelo token
  if (lookahead.kind() == symbol_kind::S_INVALID)
  {
    auto error_code = lookahead.value.as<LexerError>();
    std::cerr << " (" << to_string(error_code) << ")";
  }

  // show expected tokens
  const int TOKEN_LIMIT = 10;
  symbol_kind_type expected[TOKEN_LIMIT];
  int num_expected = ctx.expected_tokens(expected, TOKEN_LIMIT);

  if (num_expected > 0) {
      std::cerr << ", expecting ";
      for (int i = 0; i < num_expected; ++i) {
          if (i > 0) std::cerr << " or ";
          std::cerr << symbol_name(expected[i]);
      }
  }
  std::cerr << "\n";
}