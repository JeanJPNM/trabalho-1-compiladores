#include "nodes.hpp"

namespace AST
{
  eval::Result Identifier::evaluate(eval::Context &ctx)
  {
    return ctx.get_variable(name);
  }

  eval::Result NumberLiteral::evaluate(eval::Context &ctx)
  {
    if (std::holds_alternative<double>(value))
      return eval::Result(std::get<double>(value));
    else
      return eval::Result(std::get<long>(value));
  }

  eval::Result UnaryExpression::evaluate(eval::Context &ctx)
  {
    auto operandValue = operand->evaluate(ctx);
    if (op == UnaryOperator::PLUS)
      return operandValue;

    return eval::Result(0L) - operandValue;
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
      return l / r;
    }

    return eval::Result();
  }

  eval::Result ConstantDeclaration::evaluate(eval::Context &ctx)
  {
    auto valueResult = value->evaluate(ctx);
    ctx.define_constant(identifier->name, valueResult);
    return eval::Result();
  }

  eval::Result VariableDeclaration::evaluate(eval::Context &ctx)
  {
    auto init = typeAnnotation->type == VariableType::INT
                    ? eval::Result(0L)
                    : eval::Result(0.0);
    for (auto identifier : identifiers)
    {
      ctx.define_variable(identifier->name, init);
    }

    return eval::Result();
  }

  eval::Result ParameterDeclaration::evaluate(eval::Context &ctx)
  {
    auto init = typeAnnotation->type == VariableType::INT
                    ? eval::Result(0L)
                    : eval::Result(0.0);

    for (auto identifier : identifiers)
    {
      ctx.define_variable(identifier->name, init);
    }

    return eval::Result();
  }

  eval::Result ProcedureDeclaration::evaluate(eval::Context &ctx)
  {
    ctx.define_procedure(name->name, [this](auto &ctx, const auto &args) -> eval::Result
                         {
        if (args.size() != parameters.size())
          throw std::runtime_error("Expected " + std::to_string(parameters.size()) +
                                   " arguments but got " + std::to_string(args.size()));

        eval::Context child_ctx(&ctx);


        for (size_t i = 0; i < parameters.size(); i++)
        {
          auto param = parameters[i];
          auto arg = args[i];

        
          // declara a variável no escopo do procedimento
          param->evaluate(child_ctx);
          for (auto identifier : param->identifiers)
          {
            child_ctx.assign_variable(identifier->name, arg);
          }
        }

        for (auto localVar : localVariables)
        {
          localVar->evaluate(child_ctx);
        }

        return body->evaluate(child_ctx); });

    return eval::Result();
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

  eval::Result ReadStatement::evaluate(eval::Context &ctx)
  {
    for (const auto variable : variables)
    {
      auto &name = variable->name;
      auto value = ctx.get_variable(name);

      if (value.is_double())
      {
        double input;
        std::cin >> input;
        ctx.assign_variable(name, eval::Result(input));
      }
      else
      {
        long input;
        std::cin >> input;
        ctx.assign_variable(name, eval::Result(input));
      }
    }

    return eval::Result();
  }

  eval::Result WriteStatement::evaluate(eval::Context &ctx)
  {
    for (size_t i = 0; i < variables.size(); i++)
    {
      auto variable = variables[i];
      auto value = ctx.get_variable(variable->name);

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

  eval::Result CallStatement::evaluate(eval::Context &ctx)
  {
    std::vector<eval::Result> argValues;

    for (auto arg : arguments)
    {
      argValues.push_back(arg->evaluate(ctx));
    }

    return ctx.call_procedure(procedureName->name, argValues);
  }

  eval::Result IfStatement::evaluate(eval::Context &ctx)
  {
    auto conditionValue = condition->evaluate(ctx);

    if (conditionValue.is_truthy())
      return consequent->evaluate(ctx);
    else if (alternate != nullptr)
      return alternate->evaluate(ctx);

    return eval::Result();
  }

  eval::Result WhileStatement::evaluate(eval::Context &ctx)
  {
    eval::Result conditionValue = condition->evaluate(ctx);

    while (conditionValue.is_truthy())
    {
      body->evaluate(ctx);
      conditionValue = condition->evaluate(ctx);
    }

    return eval::Result();
  }

  eval::Result ForStatement::evaluate(eval::Context &ctx)
  {
    initialization->evaluate(ctx);
    const std::string &varName = initialization->variable->name;

    eval::Result initialValue = ctx.get_variable(varName);
    if (!initialValue.is_long())
      throw std::runtime_error("For loop variable must be of type integer");

    eval::Result targetValue = target->evaluate(ctx);
    eval::Result step((initialValue <= targetValue).is_truthy() ? 1L : -1L);
    eval::Result currentValue = initialValue;

    while (currentValue != targetValue)
    {
      body->evaluate(ctx);
      ctx.assign_variable(varName, currentValue + step);
      currentValue = ctx.get_variable(varName);
    }

    return eval::Result();
  }

  eval::Result VariableAssignment::evaluate(eval::Context &ctx)
  {
    ctx.assign_variable(variable->name, value->evaluate(ctx));

    return eval::Result();
  }

  eval::Result Block::evaluate(eval::Context &ctx)
  {
    for (auto statement : statements)
    {
      statement->evaluate(ctx);
    }

    return eval::Result();
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
}
