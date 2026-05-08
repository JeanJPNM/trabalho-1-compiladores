#ifndef EVAL_HPP
#define EVAL_HPP

#include <map>
#include <set>
#include <string>
#include <variant>
#include <functional>
#include <stdexcept>
#include "range.hpp"

namespace eval
{
  class Result
  {
    std::variant<std::monostate, double, long> value;

  public:
    Result() : value(std::monostate()) {}
    Result(double value) : value(value) {}
    Result(long value) : value(value) {}
    Result(bool value) : value(static_cast<long>(value)) {}

    bool is_empty() const { return std::holds_alternative<std::monostate>(value); }
    bool is_double() const { return std::holds_alternative<double>(value); }
    bool is_long() const { return std::holds_alternative<long>(value); }
    bool same_type(const Result &other) const
    {
      return value.index() == other.value.index();
    }

    double as_double() const;
    long as_long() const;
    const char *type_name() const;
    bool is_truthy() const;

    Result operator+(const Result &other) const;
    Result operator-(const Result &other) const;
    Result operator*(const Result &other) const;
    Result operator/(const Result &other) const;
    Result operator>(const Result &other) const;
    Result operator<(const Result &other) const;
    Result operator>=(const Result &other) const;
    Result operator<=(const Result &other) const;
    bool operator==(const Result &other) const;
    bool operator!=(const Result &other) const;
  };

  class Context;
  typedef std::function<Result(Context &, std::vector<Result>)> ProcedureRunner;

  class InterpreterError : public std::exception
  {
    std::string what_message;
    std::string filename;

  public:
    Range range;
    std::string message;

    InterpreterError(const std::string &message, Range range);

    void set_filename(const std::string &filename);

    const char *what() const noexcept override;
  };

  class Context
  {
    Context *parent = nullptr;
    std::set<std::string> constants;
    std::map<std::string, Result> variables;
    std::map<std::string, ProcedureRunner> procedures;

  public:
    Context(Context *parent = nullptr)
        : parent(parent) {}

    void define_constant(const std::string &name, Result value, const Range &range);
    void define_variable(const std::string &name, Result value, const Range &range);
    void assign_variable(const std::string &name, Result value, const Range &range);
    Result get_variable(const std::string &name, const Range &range) const;
    void define_procedure(const std::string &name, const Range &range, ProcedureRunner proc);
    Result call_procedure(const std::string &name, const std::vector<Result> &args, const Range &range);
  };

};

#endif