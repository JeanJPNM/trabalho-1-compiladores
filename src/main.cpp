#include "nodes.hpp"
#include "driver.hpp"
#include "cli.hpp"

void show_help();
void dump_ast(const std::string &filename);
void dump_tokens(const std::string &filename);
void run(const std::string &filename);

int main(int argc, char *argv[])
{
  auto args = parse_args(argc, argv);
  if (args.size() == 0)
  {
    show_help();
    return 0;
  }

  auto subcommand = args[0];
  std::string filename = args.size() > 1 ? args[1] : "-";

  if (subcommand == "run")
  {
    run(filename);
  }
  else if (subcommand == "dump-tokens")
  {
    dump_tokens(filename);
  }
  else if (subcommand == "dump-ast")
  {
    dump_ast(filename);
  }
  else
  {
    std::cerr << "Unknown subcommand: " << subcommand << std::endl;
    show_help();
    return 1;
  }
}

void show_help()
{
  std::cout << "Available subcommands:\n"
               "  run [file]            Reads and executes LALG code from the file or stdin\n"
               "  dump-tokens [file]    Dumps the tokens from the file or stdin\n"
               "  dump-ast [file]       Dumps the AST from the file or stdin\n";
}

void run(const std::string &filename)
{
  Driver driver;
  driver.parse(filename);
  if (driver.program)
  {
    try
    {
      eval::Context root_ctx;
      driver.program->evaluate(root_ctx);
    }
    catch (eval::InterpreterError &e)
    {
      e.set_filename(driver.filename);
      std::cerr << e.what() << std::endl;
    }
  }
}

void dump_ast(const std::string &filename)
{
  Driver driver;
  driver.parse(filename);
  if (driver.program)
  {
    driver.program->dump(std::cout, 0);
  }
}

void dump_tokens(const std::string &filename)
{
  Driver driver;
  driver.dump_tokens(filename);
}