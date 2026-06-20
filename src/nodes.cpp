#include "nodes.hpp"

namespace AST
{
  eval::Result Identifier::evaluate(eval::Context &ctx)
  {
    return ctx.get_variable(name, range);
  }

  void Identifier::dump(std::ostream &os, int indent = 0) const
  {
    os << "Identifier(" << name << ")" << std::endl;
  }

  eval::Result NumberLiteral::evaluate(eval::Context &ctx)
  {
    if (std::holds_alternative<double>(value))
      return eval::Result(std::get<double>(value));
    else
      return eval::Result(std::get<long>(value));
  }

  void NumberLiteral::dump(std::ostream &os, int indent) const
  {
    os << "NumberLiteral(";
    if (std::holds_alternative<double>(value))
    {
      os << "real: " << std::get<double>(value);
    }
    else
    {
      os << "integer: " << std::get<long>(value);
    }
    os << ")" << std::endl;
  }

  eval::Result UnaryExpression::evaluate(eval::Context &ctx)
  {
    auto operand_value = operand->evaluate(ctx);
    if (op == UnaryOperator::PLUS)
      return operand_value;

    return eval::Result(0L) - operand_value;
  }

  void UnaryExpression::dump(std::ostream &os, int indent) const
  {
    os << "UnaryExpression(";
    int nested_indent = indent + 2;
    os << Pad(nested_indent) << "operator: " << (op == UnaryOperator::PLUS ? "+" : "-") << std::endl;
    os << Pad(nested_indent) << "operand: ";
    operand->dump(os, nested_indent);
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result BinaryExpression::evaluate(eval::Context &ctx)
  {
    auto l = left->evaluate(ctx);
    auto r = right->evaluate(ctx);
    switch (op)
    {
    case BinaryOperator::ADD:
      return l + r;
    case BinaryOperator::SUBTRACT:
      return l - r;
    case BinaryOperator::MULTIPLY:
      return l * r;
    case BinaryOperator::DIVIDE:
      if (r.as_double() == 0)
        throw eval::InterpreterError("Division by zero", range);
      return l / r;
    }

    return eval::Result();
  }
  void BinaryExpression::dump(std::ostream &os, int indent) const
  {
    os << "BinaryExpression(" << std::endl;
    int nested_indent = indent + 2;
    os << Pad(nested_indent) << "operator: ";
    switch (op)
    {
    case BinaryOperator::ADD:
      os << "+" << std::endl;
      break;
    case BinaryOperator::SUBTRACT:
      os << "-" << std::endl;
      break;
    case BinaryOperator::MULTIPLY:
      os << "*" << std::endl;
      break;
    case BinaryOperator::DIVIDE:
      os << "/" << std::endl;
      break;
    }
    os << Pad(nested_indent) << "left: ";
    left->dump(os, nested_indent);
    os << Pad(nested_indent) << "right: ";
    right->dump(os, nested_indent);
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result ConstantDeclaration::evaluate(eval::Context &ctx)
  {
    auto value_result = value->evaluate(ctx);
    ctx.define_constant(identifier->name, value_result, range);
    return eval::Result();
  }

  void ConstantDeclaration::dump(std::ostream &os, int indent) const
  {
    os << "ConstantDeclaration(" << std::endl;
    os << "name: ";
    identifier->dump(os, indent + 2);
    os << "value: ";
    value->dump(os, indent + 2);
    os << Pad(indent) << ")" << std::endl;
  }

  void TypeAnnotation::dump(std::ostream &os, int indent) const
  {
    os << "TypeAnnotation(";
    if (type == VariableType::INT)
      os << "integer";
    else
      os << "real";
    os << ")" << std::endl;
  }

  eval::Result VariableDeclaration::evaluate(eval::Context &ctx)
  {
    auto init = type_annotation->type == VariableType::INT
                    ? eval::Result(0L)
                    : eval::Result(0.0);
    for (auto identifier : identifiers)
    {
      ctx.define_variable(identifier->name, init, identifier->range);
    }

    return eval::Result();
  }

  void VariableDeclaration::dump(std::ostream &os, int indent) const
  {
    os << "VariableDeclaration(" << std::endl;
    os << Pad(indent + 2) << "type: ";
    type_annotation->dump(os, indent + 2);

    os << Pad(indent + 2) << "identifiers: [" << std::endl;
    for (auto identifier : identifiers)
    {
      os << Pad(indent + 4);
      identifier->dump(os, indent + 4);
    }

    os << Pad(indent + 2) << "]" << std::endl;
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result ParameterDeclaration::evaluate(eval::Context &ctx)
  {
    auto init = type_annotation->type == VariableType::INT
                    ? eval::Result(0L)
                    : eval::Result(0.0);

    for (auto identifier : identifiers)
    {
      ctx.define_variable(identifier->name, init, identifier->range);
    }

    return eval::Result();
  }
  void ParameterDeclaration::dump(std::ostream &os, int indent) const
  {
    os << "ParameterDeclaration(" << std::endl;
    os << Pad(indent + 2) << "type: ";
    type_annotation->dump(os, indent + 2);
    os << Pad(indent + 2) << "identifiers: [" << std::endl;

    for (auto identifier : identifiers)
    {
      os << Pad(indent + 4);
      identifier->dump(os, indent + 4);
    }

    os << Pad(indent + 2) << "]" << std::endl;
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result ProcedureDeclaration::evaluate(eval::Context &ctx)
  {
    ctx.define_procedure(name->name, range, [this](auto &ctx, const auto &args, const auto &call_range) -> eval::Result
                         {
        size_t parameter_count = 0;
        for(auto param : parameters)
          parameter_count += param->identifiers.size();

        if (args.size() != parameter_count)
          throw eval::InterpreterError("Expected " + std::to_string(parameter_count) +
                                        " arguments but got " + std::to_string(args.size()),     
                                        call_range);

        eval::Context child_ctx(&ctx);

        // assign arguments to parameters in the new context
        size_t arg_index = 0;
        for (auto param : parameters)
        {
          param->evaluate(child_ctx);
          for (auto identifier : param->identifiers)
          {
            auto arg = args[arg_index];
            arg_index++;
            child_ctx.assign_variable(identifier->name, arg, call_range);
          }
        }

        for (auto local_var : local_variables)
        {
          local_var->evaluate(child_ctx);
        }

        return body->evaluate(child_ctx); });

    return eval::Result();
  }

  void ProcedureDeclaration::dump(std::ostream &os, int indent) const
  {
    os << "ProcedureDeclaration(" << std::endl;

    os << Pad(indent + 2) << "name: ";
    name->dump(os, indent + 2);

    os << Pad(indent + 2) << "parameters: [" << std::endl;
    for (auto param : parameters)
    {
      os << Pad(indent + 4);
      param->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;

    os << Pad(indent + 2) << "local_variables: [" << std::endl;
    for (auto local_var : local_variables)
    {
      os << Pad(indent + 4);
      local_var->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;

    os << Pad(indent + 2) << "body: ";
    body->dump(os, indent + 2);

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result Condition::evaluate(eval::Context &ctx)
  {
    auto l = left->evaluate(ctx);
    auto r = right->evaluate(ctx);

    switch (op)
    {
    case ComparisonOperator::EQUAL:
      return eval::Result(l == r);
    case ComparisonOperator::NOT_EQUAL:
      return eval::Result(l != r);
    case ComparisonOperator::LESS_THAN:
      return l < r;
    case ComparisonOperator::LESS_THAN_OR_EQUAL:
      return l <= r;
    case ComparisonOperator::GREATER_THAN:
      return l > r;
    case ComparisonOperator::GREATER_THAN_OR_EQUAL:
      return l >= r;
    }

    return eval::Result();
  }

  void Condition::dump(std::ostream &os, int indent) const
  {
    os << "Condition(" << std::endl;
    os << Pad(indent + 2) << "operator: ";
    switch (op)
    {
    case ComparisonOperator::EQUAL:
      os << "==" << std::endl;
      break;
    case ComparisonOperator::NOT_EQUAL:
      os << "<>" << std::endl;
      break;
    case ComparisonOperator::LESS_THAN:
      os << "<" << std::endl;
      break;
    case ComparisonOperator::LESS_THAN_OR_EQUAL:
      os << "<=" << std::endl;
      break;
    case ComparisonOperator::GREATER_THAN:
      os << ">" << std::endl;
      break;
    case ComparisonOperator::GREATER_THAN_OR_EQUAL:
      os << ">=" << std::endl;
      break;
    }

    os << Pad(indent + 2) << "left: ";
    left->dump(os, indent + 2);
    os << Pad(indent + 2) << "right: ";
    right->dump(os, indent + 2);
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result ReadStatement::evaluate(eval::Context &ctx)
  {
    for (const auto variable : variables)
    {
      auto &name = variable->name;
      auto value = variable->evaluate(ctx);

      // read the right type of input based on the variable's type
      if (value.is_double())
      {
        double input;
        std::cin >> input;
        ctx.assign_variable(name, eval::Result(input), range);
      }
      else
      {
        long input;
        std::cin >> input;
        ctx.assign_variable(name, eval::Result(input), range);
      }
    }

    return eval::Result();
  }

  void ReadStatement::dump(std::ostream &os, int indent) const
  {
    os << "ReadStatement(" << std::endl;
    os << Pad(indent + 2) << "variables: [" << std::endl;
    for (const auto variable : variables)
    {
      os << Pad(indent + 4);
      variable->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result WriteStatement::evaluate(eval::Context &ctx)
  {
    for (size_t i = 0; i < variables.size(); i++)
    {
      auto variable = variables[i];
      auto value = variable->evaluate(ctx);

      if (i != 0)
        std::cout << ' ';

      if (value.is_empty())
        std::cout << value.type_name();
      else if (value.is_double())
        std::cout << value.as_double();
      else
        std::cout << value.as_long();
    }
    std::cout << std::endl;

    return eval::Result();
  }

  void WriteStatement::dump(std::ostream &os, int indent) const
  {
    os << "WriteStatement(" << std::endl;
    os << Pad(indent + 2) << "variables: [" << std::endl;
    for (const auto variable : variables)
    {
      os << Pad(indent + 4);
      variable->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;
    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result CallStatement::evaluate(eval::Context &ctx)
  {
    std::vector<eval::Result> argValues;

    for (auto arg : arguments)
    {
      argValues.push_back(arg->evaluate(ctx));
    }

    return ctx.call_procedure(procedure_name->name, argValues, range);
  }

  void CallStatement::dump(std::ostream &os, int indent) const
  {
    os << "CallStatement(" << std::endl;
    os << Pad(indent + 2) << "procedure_name: ";
    procedure_name->dump(os, indent + 2);

    os << Pad(indent + 2) << "arguments: [" << std::endl;
    for (const auto arg : arguments)
    {
      os << Pad(indent + 4);
      arg->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result IfStatement::evaluate(eval::Context &ctx)
  {
    auto condition_value = condition->evaluate(ctx);

    if (condition_value.is_truthy())
      return consequent->evaluate(ctx);
    else if (alternate != nullptr)
      return alternate->evaluate(ctx);

    return eval::Result();
  }
  void IfStatement::dump(std::ostream &os, int indent) const
  {
    os << "IfStatement(" << std::endl;
    os << Pad(indent + 2) << "condition: ";
    condition->dump(os, indent + 2);

    os << Pad(indent + 2) << "consequent: ";
    consequent->dump(os, indent + 2);

    os << Pad(indent + 2) << "alternate: ";
    if (alternate != nullptr)
      alternate->dump(os, indent + 2);
    else
      os << "null" << std::endl;

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result WhileStatement::evaluate(eval::Context &ctx)
  {
    eval::Result condition_value = condition->evaluate(ctx);

    while (condition_value.is_truthy())
    {
      body->evaluate(ctx);
      condition_value = condition->evaluate(ctx);
    }

    return eval::Result();
  }

  void WhileStatement::dump(std::ostream &os, int indent) const
  {
    os << "WhileStatement(" << std::endl;
    os << Pad(indent + 2) << "condition: ";
    condition->dump(os, indent + 2);

    os << Pad(indent + 2) << "body: ";
    body->dump(os, indent + 2);

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result ForStatement::evaluate(eval::Context &ctx)
  {
    initialization->evaluate(ctx);
    const std::string &var_name = initialization->variable->name;
    Range var_range = initialization->variable->range;

    eval::Result initial_value = ctx.get_variable(var_name, var_range);
    eval::Result target_value = target->evaluate(ctx);
    // support both forward and backward loops
    bool is_forward = (initial_value <= target_value).is_truthy();
    eval::Result step(is_forward ? 1L : -1L);
    eval::Result current_value = initial_value;

    while (is_forward ? current_value.as_double() < target_value.as_double()
                      : current_value.as_double() > target_value.as_double())
    {
      body->evaluate(ctx);
      current_value = ctx.get_variable(var_name, var_range) + step;
      ctx.assign_variable(var_name, current_value, var_range);
    }

    return eval::Result();
  }

  void ForStatement::dump(std::ostream &os, int indent) const
  {
    os << "ForStatement(" << std::endl;
    os << Pad(indent + 2) << "init: ";
    initialization->dump(os, indent + 2);

    os << Pad(indent + 2) << "target: ";
    target->dump(os, indent + 2);

    os << Pad(indent + 2) << "body: ";
    body->dump(os, indent + 2);

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result VariableAssignment::evaluate(eval::Context &ctx)
  {
    ctx.assign_variable(variable->name, value->evaluate(ctx), range);

    return eval::Result();
  }

  void VariableAssignment::dump(std::ostream &os, int indent) const
  {
    os << "VariableAssignment(" << std::endl;
    os << Pad(indent + 2) << "variable: ";
    variable->dump(os, indent + 2);

    os << Pad(indent + 2) << "value: ";
    value->dump(os, indent + 2);

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result Block::evaluate(eval::Context &ctx)
  {
    for (auto statement : statements)
    {
      statement->evaluate(ctx);
    }

    return eval::Result();
  }

  void Block::dump(std::ostream &os, int indent) const
  {
    os << "Block(" << std::endl;

    for (auto statement : statements)
    {
      os << Pad(indent + 2);
      statement->dump(os, indent + 2);
    }

    os << Pad(indent) << ")" << std::endl;
  }

  eval::Result Program::evaluate(eval::Context &ctx)
  {
    for (auto declaration : declarations)
    {
      declaration->evaluate(ctx);
    }

    block->evaluate(ctx);

    return eval::Result();
  }

  void Program::dump(std::ostream &os, int indent) const
  {
    os << "Program(" << std::endl;

    os << Pad(indent + 2) << "name: ";
    name->dump(os, indent + 2);

    os << Pad(indent + 2) << "declarations: [" << std::endl;
    for (auto declaration : declarations)
    {
      os << Pad(indent + 4);
      declaration->dump(os, indent + 4);
    }
    os << Pad(indent + 2) << "]" << std::endl;

    os << Pad(indent + 2) << "block: ";
    block->dump(os, indent + 2);

    os << Pad(indent) << ")" << std::endl;
  }
}
