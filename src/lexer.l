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

%%

%{
  yy::location& loc = driver.location;
  loc.step();
%}

"{"[^\}]*"}" { loc.step(); };
[ \t] { loc.step(); };
\n+ { loc.lines(yyleng); loc.step(); };

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
  return driver.make_token(yytext);
}

. {
  return yy::parser::make_YYerror(loc);
}

<<EOF>> {
  return yy::parser::make_YYEOF(loc);
};

%%

void Driver::scan_begin ()
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

void Driver::scan_end ()
{
  fclose(yyin);
}
