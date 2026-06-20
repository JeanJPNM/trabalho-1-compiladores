%{
  #include "cli.hpp"
  #include "nodes.hpp"
  #include "driver.hpp"
  #include "parser.hpp"
%}

%option yylineno
%option noyywrap nounput noinput batch
%option debug

%{
  // sync column with size of token
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
  // unterminated comment found,
  // return to the initial state to emit EOF token
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
[0-9]+[0-9a-zA-Z.@#$]+ {
  // match malformed numbers,
  // has lower priority than the previous rules
  return driver.make_error(yytext, LexerError::MalformedNumber);
}

[a-zA-Z][a-zA-Z0-9]* {
  std::string name(yytext);
  return driver.make_keyword_or_identifier(name);
}
[a-zA-Z][a-zA-Z0-9@#$]+ {
  // match malformed identifiers,
  // has lower priority than the previous rules
  return driver.make_error(yytext, LexerError::MalformedIdentifier);
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

// basic flex setup needed before scanning
void Driver::scan_begin()
{
  yy_flex_debug = trace_scanning;
  if (filename.empty() || filename == STDIN_FILENAME)
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
