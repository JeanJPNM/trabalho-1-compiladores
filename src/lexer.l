%{
  #include "nodes.hpp"
  #include "driver.hpp"
  #include "parser.hpp"
%}

%option yylineno
%option noyywrap nounput noinput batch
%option debug

%{
  #define YY_USER_ACTION loc.columns(yyleng);
%}

%x COMMENT

%%

%{
  yy::location& loc = driver.location;
  loc.step();
%}

"{" { BEGIN(COMMENT); }
<COMMENT>"}" { loc.step(); BEGIN(INITIAL); }
<COMMENT>\r\n|\n { loc.lines(1); loc.step(); }
<COMMENT>[^\}\n]+ { }
<COMMENT><<EOF>> {
  BEGIN(INITIAL);

  return driver.make_error("{", LexerError::UnterminatedComment);
}

[ \t]+ { loc.step(); };
\r\n|\n { loc.lines(1); loc.step(); };

[0-9]+\.[0-9]+ {
  return driver.make_double_literal(yytext);
}
[0-9]+ {
  return driver.make_integer_literal(yytext);
}

[a-zA-Z][a-zA-Z0-9]* {
  std::string name(yytext);
  return driver.make_keyword_or_identifier(name);
}

[;(),:=*/+\-<>\.]|:=|==|<>|<=|>= {
  return driver.make_punctuation_token(yytext);
}

. {
  return driver.make_error(yytext, LexerError::UnexpectedCharacter);
}

<<EOF>> {
  return yy::parser::make_YYEOF(loc);
};

%%

void Driver::scan_begin()
{
  yy_flex_debug = trace_scanning;
  if (filename.empty() || filename == "-")
  {
    yyin = stdin;
  }
  else if (!(yyin = fopen(filename.c_str(), "r")))
  {
    std::cerr << "cannot open " << filename << ": " << strerror (errno) << '\n';
    exit(EXIT_FAILURE);
  }
}

void Driver::scan_end()
{
  fclose(yyin);
}
