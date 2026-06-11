#ifndef NODES_HPP
#define NODES_HPP

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include "cli.hpp"
#include "eval.hpp"
#include "range.hpp"

namespace AST
{
  // Forward declarations
  class Expression;
  class Identifier;
  class NumberLiteral;
  class BinaryExpression;
  class UnaryExpression;

  class Declaration;
  class ConstantDeclaration;
  class VariableDeclaration;
  class ParameterDeclaration;
  class ProcedureDeclaration;

  class Statement;
  class ReadStatement;
  class WriteStatement;
  class CallStatement;
  class IdentifierStatement;
  class IfStatement;
  class WhileStatement;
  class ForStatement;
  class VariableAssignment;
  class Block;

  class Program;

  class Node
  {
  public:
    Range range;
    Node(Range range) : range(range) {}
    virtual ~Node() {}
    virtual eval::Result evaluate(eval::Context &ctx) { return eval::Result(); }

    virtual void dump(std::ostream &os, int indent) const = 0;
  };

  class Expression : public Node
  {
  public:
    Expression(Range range) : Node(range) {}
    virtual Identifier *as_identifier() { return nullptr; }
    virtual NumberLiteral *as_number_literal() { return nullptr; }
    virtual UnaryExpression *as_unary_expression() { return nullptr; }
    virtual BinaryExpression *as_binary_expression() { return nullptr; }
  };

  class Identifier : public Expression
  {
  public:
    std::string name;
    Identifier(Range range, std::string name) : name(name), Expression(range) {}

    Identifier *as_identifier() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class NumberLiteral : public Expression
  {
  public:
    std::variant<double, long> value;
    NumberLiteral(Range range, double value) : value(value), Expression(range) {}
    NumberLiteral(Range range, long value) : value(value), Expression(range) {}

    NumberLiteral *as_number_literal() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  enum class UnaryOperator
  {
    PLUS,
    MINUS
  };

  class UnaryExpression : public Expression
  {
  public:
    UnaryOperator op;
    Expression *operand;

    UnaryExpression(Range range, UnaryOperator op, Expression *operand)
        : op(op), operand(operand), Expression(range) {}

    UnaryExpression *as_unary_expression() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  enum class BinaryOperator
  {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
  };

  class BinaryExpression : public Expression
  {
  public:
    BinaryOperator op;
    Expression *left, *right;

    BinaryExpression(Range range, Expression *left, BinaryOperator op, Expression *right)
        : left(left), op(op), right(right), Expression(range) {}

    BinaryExpression *as_binary_expression() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class Declaration : public Node
  {
  public:
    Declaration(Range range) : Node(range) {}
    virtual ConstantDeclaration *as_constant_declaration() { return nullptr; }
    virtual VariableDeclaration *as_variable_declaration() { return nullptr; }
    virtual ProcedureDeclaration *as_procedure_declaration() { return nullptr; }
  };

  class ConstantDeclaration : public Declaration
  {
  public:
    Identifier *identifier;
    NumberLiteral *value;
    ConstantDeclaration(Range range, Identifier *identifier, NumberLiteral *value)
        : identifier(identifier), value(value), Declaration(range) {}

    ConstantDeclaration *as_constant_declaration() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  enum class VariableType
  {
    INT,
    FLOAT,
  };

  class TypeAnnotation : public Node
  {
  public:
    VariableType type;
    TypeAnnotation(Range range, VariableType type) : type(type), Node(range) {}
    void dump(std::ostream &os, int indent) const override;
  };

  class VariableDeclaration : public Declaration
  {
  public:
    std::vector<Identifier *> identifiers;
    TypeAnnotation *type_annotation;

    VariableDeclaration(
        Range range,
        std::vector<Identifier *> &&identifiers,
        TypeAnnotation *type_annotation)
        : identifiers(std::move(identifiers)),
          type_annotation(type_annotation),
          Declaration(range) {}

    VariableDeclaration *as_variable_declaration() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  // não precisa herdar de Declaration porque é usado
  // exclusivamente em ProcedureDeclaration
  class ParameterDeclaration : public Node
  {
  public:
    std::vector<Identifier *> identifiers;
    TypeAnnotation *type_annotation;

    ParameterDeclaration(
        Range range,
        std::vector<Identifier *> &&identifiers,
        TypeAnnotation *type_annotation)
        : identifiers(std::move(identifiers)),
          type_annotation(type_annotation),
          Node(range) {}

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class ProcedureDeclaration : public Declaration
  {
  public:
    Identifier *name;
    std::vector<ParameterDeclaration *> parameters;
    std::vector<VariableDeclaration *> local_variables;
    Block *body;

    ProcedureDeclaration(
        Range range,
        Identifier *name,
        std::vector<ParameterDeclaration *> &&parameters,
        std::vector<VariableDeclaration *> &&local_variables,
        Block *body)
        : name(name),
          parameters(std::move(parameters)),
          local_variables(std::move(local_variables)),
          body(body),
          Declaration(range) {}

    ProcedureDeclaration *as_procedure_declaration() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class Statement : public Node
  {
  public:
    Statement(Range range) : Node(range) {}

    virtual ReadStatement *as_read_statement() { return nullptr; }
    virtual WriteStatement *as_write_statement() { return nullptr; }
    virtual CallStatement *as_call_statement() { return nullptr; }
    virtual IdentifierStatement *as_identifier_statement() { return nullptr; }
    virtual IfStatement *as_if_statement() { return nullptr; }
    virtual WhileStatement *as_while_statement() { return nullptr; }
    virtual ForStatement *as_for_statement() { return nullptr; }
    virtual VariableAssignment *as_variable_assignment() { return nullptr; }
    virtual Block *as_block() { return nullptr; }
  };

  enum class ComparisonOperator
  {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL
  };

  class Condition : public Node
  {
  public:
    Expression *left, *right;
    ComparisonOperator op;

    Condition(Range range, Expression *left, ComparisonOperator op, Expression *right)
        : left(left), op(op), right(right), Node(range) {}

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class ReadStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    ReadStatement(Range range, std::vector<Identifier *> &&variables)
        : variables(std::move(variables)), Statement(range) {}

    ReadStatement *as_read_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  // isso poderia ser uma chamada de função, mas a spec especifica ela como uma
  // coisa separada, sem falar que isso nos livra de ter que
  // lidar com o conceito de funções "nativas" no interpretador
  class WriteStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    WriteStatement(Range range, std::vector<Identifier *> &&variables)
        : variables(std::move(variables)), Statement(range) {}

    WriteStatement *as_write_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class CallStatement : public Statement
  {
  public:
    Identifier *procedure_name;
    std::vector<Identifier *> arguments;

    CallStatement(Range range, Identifier *procedure_name, std::vector<Identifier *> &&arguments)
        : procedure_name(procedure_name), arguments(std::move(arguments)), Statement(range) {}

    CallStatement *as_call_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class IdentifierStatement : public Statement
  {
  public:
    Identifier *identifier;
    IdentifierStatement(Range range, Identifier *identifier) : identifier(identifier), Statement(range) {}

    IdentifierStatement *as_identifier_statement() override { return this; }
    void dump(std::ostream &os, int indent) const override;
  };

  class IfStatement : public Statement
  {
  public:
    Condition *condition;
    Statement *consequent;
    /// Pode ser nulo
    Statement *alternate;

    IfStatement(Range range, Condition *condition, Statement *consequent, Statement *alternate)
        : condition(condition), consequent(consequent), alternate(alternate), Statement(range) {}

    IfStatement *as_if_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class WhileStatement : public Statement
  {
  public:
    Condition *condition;
    Statement *body;

    WhileStatement(Range range, Condition *condition, Statement *body)
        : condition(condition), body(body), Statement(range) {}

    WhileStatement *as_while_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class ForStatement : public Statement
  {
  public:
    VariableAssignment *initialization;
    Expression *target;
    Statement *body;

    ForStatement(Range range,
                 VariableAssignment *initialization,
                 Expression *target,
                 Statement *body)
        : initialization(initialization),
          target(target),
          body(body),
          Statement(range) {}

    ForStatement *as_for_statement() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class VariableAssignment : public Statement
  {
  public:
    Identifier *variable;
    Expression *value;

    VariableAssignment(Range range, Identifier *variable, Expression *value)
        : variable(variable), value(value), Statement(range) {}

    VariableAssignment *as_variable_assignment() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class Block : public Statement
  {
  public:
    std::vector<Statement *> statements;
    Block(Range range, std::vector<Statement *> &&statements)
        : statements(std::move(statements)), Statement(range) {}

    Block *as_block() override { return this; }

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };

  class Program : public Node
  {
  public:
    Identifier *name;
    std::vector<Declaration *> declarations;
    Block *block;

    Program(
        Range range,
        Identifier *name,
        std::vector<Declaration *> &&declarations,
        Block *block)
        : name(name),
          declarations(std::move(declarations)),
          block(block),
          Node(range) {}

    eval::Result evaluate(eval::Context &ctx) override;
    void dump(std::ostream &os, int indent) const override;
  };
}

#endif