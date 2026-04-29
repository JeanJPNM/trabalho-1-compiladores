#ifndef NODES_HPP
#define NODES_HPP

#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace AST
{
  // Forward declarations
  class ConstantDeclaration;
  class VariableDeclaration;
  class DeclarationList;
  class BinaryExpression;
  class UnaryExpression;
  class Expression;
  class ReadStatement;
  class WriteStatement;
  class CallStatement;
  class IfStatement;
  class WhileStatement;
  class ForStatement;
  class VariableAssignment;
  class Statement;
  class Block;
  class ParameterDeclaration;
  class ProcedureDeclaration;
  class Program;

  class Node
  {
  public:
    Node() {}
    virtual ~Node() {}
  };

  class Identifier : public Node
  {
  public:
    std::string name;
    Identifier(std::string name) : name(name) {}
  };

  class NumberLiteral : public Node
  {
  public:
    double value;
    NumberLiteral(double value) : value(value) {}
  };

  class ConstantDeclaration : public Node
  {
  public:
    Identifier *identifier;
    NumberLiteral *value;
    ConstantDeclaration(Identifier *identifier, NumberLiteral *value)
        : identifier(identifier), value(value) {}
    ~ConstantDeclaration();
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

  class VariableDeclaration : public Node
  {
  public:
    TypeAnnotation *typeAnnotation;
    std::vector<Identifier *> identifiers;

    VariableDeclaration(TypeAnnotation *typeAnnotation, std::vector<Identifier *> identifiers)
        : typeAnnotation(typeAnnotation), identifiers(identifiers) {}
    ~VariableDeclaration();
  };

  class Declaration
  {
    std::variant<
        ConstantDeclaration *,
        VariableDeclaration *,
        ProcedureDeclaration *>
        declaration;

  public:
    ConstantDeclaration *asConstantDeclaration();
    VariableDeclaration *asVariableDeclaration();
    ProcedureDeclaration *asProcedureDeclaration();
  };

  class DeclarationList : public Node
  {
  public:
    std::vector<Declaration *> declarations;
    DeclarationList(std::vector<Declaration *> declarations) : declarations(declarations) {}
    ~DeclarationList();
  };

  class Expression : public Node
  {
    std::variant<
        Identifier *,
        NumberLiteral *,
        UnaryExpression *,
        BinaryExpression *>
        expression;

  public:
    Expression(Identifier *identifier) : expression(identifier) {}
    Expression(NumberLiteral *numberLiteral) : expression(numberLiteral) {}
    Expression(UnaryExpression *unaryExpression) : expression(unaryExpression) {}
    Expression(BinaryExpression *binaryExpression) : expression(binaryExpression) {}

    ~Expression();

    Identifier *asIdentifier();
    NumberLiteral *asNumberLiteral();
    UnaryExpression *asUnaryExpression();
    BinaryExpression *asBinaryExpression();
  };

  enum class UnaryOperator
  {
    NEGATE,
    NOT
  };

  class UnaryExpression : public Node
  {
  public:
    UnaryOperator op;
    Expression *operand;

    UnaryExpression(UnaryOperator op, Expression *operand)
        : op(op), operand(operand) {}

    ~UnaryExpression();
  };

  enum class BinaryOperator
  {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    AND,
    OR
  };

  class BinaryExpression : public Node
  {
  public:
    BinaryOperator op;
    Expression *left, *right;

    BinaryExpression(BinaryOperator op, Expression *left, Expression *right)
        : op(op), left(left), right(right) {}
    ~BinaryExpression();
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

    ~Condition();
  };

  class ReadStatement : public Node
  {
  public:
    std::vector<Identifier *> variables;

    ReadStatement(std::vector<Identifier *> variables) : variables(variables) {}
    ~ReadStatement();
  };

  // isso poderia ser uma chamada de função, mas a spec especifica ela como uma
  // coisa separada, sem falar que isso nos livra de ter que
  // lidar com o conceito de funções "nativas" no interpretador
  class WriteStatement : public Node
  {
  public:
    std::vector<Identifier *> variables;

    WriteStatement(std::vector<Identifier *> variables) : variables(variables) {}
    ~WriteStatement();
  };

  class CallStatement : public Node
  {
  public:
    Identifier *procedureName;
    std::vector<Identifier *> arguments;

    CallStatement(Identifier *procedureName, std::vector<Identifier *> arguments)
        : procedureName(procedureName), arguments(arguments) {}
    ~CallStatement();
  };

  class IfStatement : public Node
  {
  public:
    Condition *condition;
    Statement *consequent;
    /// Pode ser nulo
    Statement *alternate;

    IfStatement(Condition *condition, Statement *consequent, Statement *alternate)
        : condition(condition), consequent(consequent), alternate(alternate) {}
    ~IfStatement();
  };

  class WhileStatement : public Node
  {
  public:
    Condition *condition;
    Statement *body;

    WhileStatement(Condition *condition, Statement *body)
        : condition(condition), body(body) {}
    ~WhileStatement();
  };

  class ForStatement : public Node
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

    ~ForStatement();
  };

  class VariableAssignment : public Node
  {
  public:
    Identifier *variable;
    Expression *value;

    VariableAssignment(Identifier *variable, Expression *value)
        : variable(variable), value(value) {}

    ~VariableAssignment();
  };

  class Statement : public Node
  {
    std::variant<
        ReadStatement *,
        WriteStatement *,
        IfStatement *,
        WhileStatement *,
        ForStatement *,
        VariableAssignment *,
        CallStatement *,
        Block *>
        statement;

  public:
    Statement(ReadStatement *readStatement) : statement(readStatement) {}
    Statement(WriteStatement *writeStatement) : statement(writeStatement) {}
    Statement(CallStatement *callStatement) : statement(callStatement) {}
    Statement(IfStatement *ifStatement) : statement(ifStatement) {}
    Statement(WhileStatement *whileStatement) : statement(whileStatement) {}
    Statement(ForStatement *forStatement) : statement(forStatement) {}
    Statement(VariableAssignment *variableAssignment) : statement(variableAssignment) {}
    Statement(Block *block) : statement(block) {}

    ~Statement();

    ReadStatement *asReadStatement();

    WriteStatement *asWriteStatement();

    CallStatement *asCallStatement();

    IfStatement *asIfStatement();

    WhileStatement *asWhileStatement();

    ForStatement *asForStatement();

    VariableAssignment *asVariableAssignment();

    Block *asBlock();
  };

  class Block : public Node
  {
  public:
    std::vector<Statement *> statements;
    Block(std::vector<Statement *> statements) : statements(statements) {}
    ~Block();
  };

  class ParameterDeclaration : public Node
  {
  public:
    Identifier *identifier;
    TypeAnnotation *typeAnnotation;

    ParameterDeclaration(Identifier *identifier, TypeAnnotation *typeAnnotation)
        : identifier(identifier), typeAnnotation(typeAnnotation) {}
    ~ParameterDeclaration();
  };

  class ProcedureDeclaration : public Node
  {
  public:
    std::vector<ParameterDeclaration *> parameters;
    std::vector<VariableDeclaration *> localVariables;
    Block *body;

    ProcedureDeclaration(
        std::vector<ParameterDeclaration *> parameters,
        std::vector<VariableDeclaration *> localVariables,
        Block *body)
        : parameters(parameters),
          localVariables(localVariables),
          body(body) {}

    ~ProcedureDeclaration();
  };

  class Program : public Node
  {
  public:
    Identifier *name;
    DeclarationList *declarations;
    Block *block;

    Program(
        Identifier *name,
        DeclarationList *declarations,
        Block *block)
        : name(name),
          declarations(declarations),
          block(block) {}
    ~Program();
  };
}

#endif