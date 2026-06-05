#include "eval.hpp"

#define RESULT_OPERATOR(op)                             \
  Result Result::operator op(const Result &other) const \
  {                                                     \
    if (is_empty() || other.is_empty())                 \
      return Result();                                  \
                                                        \
    if (is_double() || other.is_double())               \
    {                                                   \
      double left = as_double();                        \
      double right = other.as_double();                 \
      return Result(left op right);                     \
    }                                                   \
    else                                                \
    {                                                   \
      long left = as_long();                            \
      long right = other.as_long();                     \
      return Result(left op right);                     \
    }                                                   \
  }

namespace eval
{
  double Result::as_double() const
  {
    if (is_double())
      return std::get<double>(value);
    if (is_long())
      return std::get<long>(value);
    return 0;
  }

  long Result::as_long() const
  {
    if (is_long())
      return std::get<long>(value);
    if (is_double())
      return static_cast<long>(std::get<double>(value));
    return 0;
  }

  const char *Result::type_name() const
  {
    if (is_double())
      return "real";
    if (is_long())
      return "integer";
    return "null";
  }

  bool Result::is_truthy() const
  {
    if (is_empty())
      return false;
    if (is_double())
      return as_double() != 0.0;
    return as_long() != 0;
  }

  RESULT_OPERATOR(+)
  RESULT_OPERATOR(-)
  RESULT_OPERATOR(*)
  RESULT_OPERATOR(/)
  RESULT_OPERATOR(>)
  RESULT_OPERATOR(<)
  RESULT_OPERATOR(>=)
  RESULT_OPERATOR(<=)

  bool Result::operator==(const Result &other) const
  {
    if (!same_type(other))
      return false;

    if (is_empty())
      return true;

    if (is_double())
      return as_double() == other.as_double();

    return as_long() == other.as_long();
  }

  bool Result::operator!=(const Result &other) const
  {
    return !(*this == other);
  }

  InterpreterError::InterpreterError(const std::string &message, Range range)
      : range(range),
        message(message),
        filename("-"),
        what_message("InterpreterError: " + message + " at <stdin>:" + range.start.to_string())
  {
  }

  void InterpreterError::set_filename(const std::string &filename)
  {
    this->filename = filename;
    what_message = "InterpreterError: " + message + " at ";

    if (filename != "-")
      what_message += filename + ":";
    else
      what_message += "<stdin>:";

    what_message += range.start.to_string();
  }

  const char *InterpreterError::what() const noexcept
  {
    return what_message.c_str();
  }

  void Context::define_constant(const std::string &name, Result value, const Range &range)
  {
    if (constants.find(name) != constants.end())
      throw eval::InterpreterError("Constant already defined: " + name, range);
    if (variables.find(name) != variables.end())
      throw eval::InterpreterError("Variable already defined: " + name, range);
    constants.insert(name);
    variables[name] = value;
  }

  void Context::define_variable(const std::string &name, Result value, const Range &range)
  {
    if (constants.find(name) != constants.end())
      throw eval::InterpreterError("Constant already defined: " + name, range);
    if (variables.find(name) != variables.end())
      throw eval::InterpreterError("Variable already defined: " + name, range);
    variables[name] = value;
  }

  void Context::assign_variable(const std::string &name, Result value, const Range &range)
  {
    if (constants.find(name) != constants.end())
      throw eval::InterpreterError("Cannot assign to constant: " + name, range);
    auto it = variables.find(name);
    if (it != variables.end())
    {
      if (!it->second.same_type(value))
        throw eval::InterpreterError(
            "Cannot assign value of different type to variable: " +
                name + " (expected " + it->second.type_name() +
                ", got " + value.type_name() + ")",
            range);
      it->second = value;
    }
    else if (parent)
    {
      parent->assign_variable(name, value, range);
    }
    else
    {
      throw eval::InterpreterError("Undefined variable: " + name, range);
    }
  }

  Result Context::get_variable(const std::string &name, const Range &range) const
  {
    auto it = variables.find(name);
    if (it == variables.end())
    {
      if (parent)
        return parent->get_variable(name, range);
      throw eval::InterpreterError("Undefined variable: " + name, range);
    }
    return it->second;
  }

  void Context::define_procedure(const std::string &name, const Range &range, ProcedureRunner proc)
  {
    if (procedures.find(name) != procedures.end())
      throw eval::InterpreterError("Procedure already defined: " + name, range);
    procedures[name] = proc;
  }

  Result Context::call_procedure(const std::string &name, const std::vector<Result> &args, const Range &range)
  {
    auto it = procedures.find(name);
    if (it == procedures.end())
    {
      if (parent)
        return parent->call_procedure(name, args, range);
      throw eval::InterpreterError("Undefined procedure: " + name, range);
    }
    return it->second(*this, args);
  }
}