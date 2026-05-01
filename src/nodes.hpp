#ifndef NODES_HPP
#define NODES_HPP

#include <string>
#include <vector>

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
    Node() {}
    virtual ~Node() {}
  };

  class Expression : public Node
  {
  public:
    virtual Identifier *asIdentifier() { return nullptr; }
    virtual NumberLiteral *asNumberLiteral() { return nullptr; }
    virtual UnaryExpression *asUnaryExpression() { return nullptr; }
    virtual BinaryExpression *asBinaryExpression() { return nullptr; }
  };

  class Identifier : public Expression
  {
  public:
    std::string name;
    Identifier(std::string name) : name(name) {}

    Identifier *asIdentifier() override { return this; }
  };

  class NumberLiteral : public Expression
  {
  public:
    double value;
    NumberLiteral(double value) : value(value) {}

    NumberLiteral *asNumberLiteral() override { return this; }
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

    UnaryExpression(UnaryOperator op, Expression *operand)
        : op(op), operand(operand) {}

    UnaryExpression *asUnaryExpression() override { return this; }
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

    BinaryExpression(Expression *left, BinaryOperator op, Expression *right)
        : left(left), op(op), right(right) {}

    BinaryExpression *asBinaryExpression() override { return this; }
  };

  class Declaration : public Node
  {
  public:
    virtual ConstantDeclaration *asConstantDeclaration() { return nullptr; }
    virtual VariableDeclaration *asVariableDeclaration() { return nullptr; }
    virtual ProcedureDeclaration *asProcedureDeclaration() { return nullptr; }
  };

  class ConstantDeclaration : public Declaration
  {
  public:
    Identifier *identifier;
    NumberLiteral *value;
    ConstantDeclaration(Identifier *identifier, NumberLiteral *value)
        : identifier(identifier), value(value) {}

    ConstantDeclaration *asConstantDeclaration() override { return this; }
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
    TypeAnnotation(VariableType type) : type(type) {}
  };

  class VariableDeclaration : public Declaration
  {
  public:
    std::vector<Identifier *> identifiers;
    TypeAnnotation *typeAnnotation;

    VariableDeclaration(std::vector<Identifier *> identifiers, TypeAnnotation *typeAnnotation)
        : identifiers(identifiers), typeAnnotation(typeAnnotation) {}

    VariableDeclaration *asVariableDeclaration() override { return this; }
  };

  // não precisa herdar de Declaration porque é usado
  // exclusivamente em ProcedureDeclaration
  class ParameterDeclaration : public Node
  {
  public:
    std::vector<Identifier *> identifiers;
    TypeAnnotation *typeAnnotation;

    ParameterDeclaration(std::vector<Identifier *> identifiers, TypeAnnotation *typeAnnotation)
        : identifiers(identifiers), typeAnnotation(typeAnnotation) {}
  };

  class ProcedureDeclaration : public Declaration
  {
  public:
    Identifier *name;
    std::vector<ParameterDeclaration *> parameters;
    std::vector<VariableDeclaration *> localVariables;
    Block *body;

    ProcedureDeclaration(
        Identifier *name,
        std::vector<ParameterDeclaration *> parameters,
        std::vector<VariableDeclaration *> localVariables,
        Block *body)
        : name(name),
          parameters(parameters),
          localVariables(localVariables),
          body(body) {}

    ProcedureDeclaration *asProcedureDeclaration() override { return this; }
  };

  class Statement : public Node
  {
  public:
    virtual ReadStatement *asReadStatement() { return nullptr; }
    virtual WriteStatement *asWriteStatement() { return nullptr; }
    virtual CallStatement *asCallStatement() { return nullptr; }
    virtual IdentifierStatement *asIdentifierStatement() { return nullptr; }
    virtual IfStatement *asIfStatement() { return nullptr; }
    virtual WhileStatement *asWhileStatement() { return nullptr; }
    virtual ForStatement *asForStatement() { return nullptr; }
    virtual VariableAssignment *asVariableAssignment() { return nullptr; }
    virtual Block *asBlock() { return nullptr; }
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

    Condition(Expression *left, ComparisonOperator op, Expression *right)
        : left(left), op(op), right(right) {}
  };

  class ReadStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    ReadStatement(std::vector<Identifier *> variables) : variables(variables) {}

    ReadStatement *asReadStatement() override { return this; }
  };

  // isso poderia ser uma chamada de função, mas a spec especifica ela como uma
  // coisa separada, sem falar que isso nos livra de ter que
  // lidar com o conceito de funções "nativas" no interpretador
  class WriteStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    WriteStatement(std::vector<Identifier *> variables) : variables(variables) {}

    WriteStatement *asWriteStatement() override { return this; }
  };

  class CallStatement : public Statement
  {
  public:
    Identifier *procedureName;
    std::vector<Identifier *> arguments;

    CallStatement(Identifier *procedureName, std::vector<Identifier *> arguments)
        : procedureName(procedureName), arguments(arguments) {}

    CallStatement *asCallStatement() override { return this; }
  };

  class IdentifierStatement : public Statement
  {
  public:
    Identifier *identifier;
    IdentifierStatement(Identifier *identifier) : identifier(identifier) {}

    IdentifierStatement *asIdentifierStatement() override { return this; }
  };

  class IfStatement : public Statement
  {
  public:
    Condition *condition;
    Statement *consequent;
    /// Pode ser nulo
    Statement *alternate;

    IfStatement(Condition *condition, Statement *consequent, Statement *alternate)
        : condition(condition), consequent(consequent), alternate(alternate) {}

    IfStatement *asIfStatement() override { return this; }
  };

  class WhileStatement : public Statement
  {
  public:
    Condition *condition;
    Statement *body;

    WhileStatement(Condition *condition, Statement *body)
        : condition(condition), body(body) {}

    WhileStatement *asWhileStatement() override { return this; }
  };

  class ForStatement : public Statement
  {
  public:
    VariableAssignment *initialization;
    Expression *target;
    Statement *body;

    ForStatement(
        VariableAssignment *initialization,
        Expression *target,
        Statement *body)
        : initialization(initialization),
          target(target),
          body(body) {}

    ForStatement *asForStatement() override { return this; }
  };

  class VariableAssignment : public Statement
  {
  public:
    Identifier *variable;
    Expression *value;

    VariableAssignment(Identifier *variable, Expression *value)
        : variable(variable), value(value) {}

    VariableAssignment *asVariableAssignment() override { return this; }
  };

  class Block : public Statement
  {
  public:
    std::vector<Statement *> statements;
    Block(std::vector<Statement *> statements) : statements(statements) {}

    Block *asBlock() override { return this; }
  };

  class Program : public Node
  {
  public:
    Identifier *name;
    std::vector<Declaration *> declarations;
    Block *block;

    Program(
        Identifier *name,
        std::vector<Declaration *> declarations,
        Block *block)
        : name(name),
          declarations(declarations),
          block(block) {}
  };
}

#endif