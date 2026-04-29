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
  class DeclarationList;

  class Statement;
  class ReadStatement;
  class WriteStatement;
  class CallStatement;
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
    virtual Identifier *asIdentifier();
    virtual NumberLiteral *asNumberLiteral();
    virtual UnaryExpression *asUnaryExpression();
    virtual BinaryExpression *asBinaryExpression();
  };

  class Identifier : public Expression
  {
  public:
    std::string name;
    Identifier(std::string name) : name(name) {}

    Identifier *asIdentifier() override;
  };

  class NumberLiteral : public Expression
  {
  public:
    double value;
    NumberLiteral(double value) : value(value) {}

    NumberLiteral *asNumberLiteral() override;
  };

  enum class UnaryOperator
  {
    NEGATE,
    NOT
  };

  class UnaryExpression : public Expression
  {
  public:
    UnaryOperator op;
    Expression *operand;

    UnaryExpression(UnaryOperator op, Expression *operand)
        : op(op), operand(operand) {}

    ~UnaryExpression();

    UnaryExpression *asUnaryExpression() override;
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

  class BinaryExpression : public Expression
  {
  public:
    BinaryOperator op;
    Expression *left, *right;

    BinaryExpression(BinaryOperator op, Expression *left, Expression *right)
        : op(op), left(left), right(right) {}
    ~BinaryExpression();

    BinaryExpression *asBinaryExpression() override;
  };

  class Declaration : public Node
  {
  public:
    virtual ConstantDeclaration *asConstantDeclaration();
    virtual VariableDeclaration *asVariableDeclaration();
    virtual ProcedureDeclaration *asProcedureDeclaration();
  };

  class ConstantDeclaration : public Declaration
  {
  public:
    Identifier *identifier;
    NumberLiteral *value;
    ConstantDeclaration(Identifier *identifier, NumberLiteral *value)
        : identifier(identifier), value(value) {}
    ~ConstantDeclaration();

    ConstantDeclaration *asConstantDeclaration() override;
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
    TypeAnnotation *typeAnnotation;
    std::vector<Identifier *> identifiers;

    VariableDeclaration(TypeAnnotation *typeAnnotation, std::vector<Identifier *> identifiers)
        : typeAnnotation(typeAnnotation), identifiers(identifiers) {}
    ~VariableDeclaration();

    VariableDeclaration *asVariableDeclaration() override;
  };

  // não precisa herdar de Declaration porque é usado
  // exclusivamente em ProcedureDeclaration
  class ParameterDeclaration : public Node
  {
  public:
    Identifier *identifier;
    TypeAnnotation *typeAnnotation;

    ParameterDeclaration(Identifier *identifier, TypeAnnotation *typeAnnotation)
        : identifier(identifier), typeAnnotation(typeAnnotation) {}
    ~ParameterDeclaration();
  };

  class ProcedureDeclaration : public Declaration
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

    ProcedureDeclaration *asProcedureDeclaration() override;
  };

  class DeclarationList : public Node
  {
  public:
    std::vector<Declaration *> declarations;
    DeclarationList(std::vector<Declaration *> declarations) : declarations(declarations) {}
    ~DeclarationList();
  };

  class Statement : public Node
  {
  public:
    virtual ReadStatement *asReadStatement();
    virtual WriteStatement *asWriteStatement();
    virtual CallStatement *asCallStatement();
    virtual IfStatement *asIfStatement();
    virtual WhileStatement *asWhileStatement();
    virtual ForStatement *asForStatement();
    virtual VariableAssignment *asVariableAssignment();
    virtual Block *asBlock();
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

  class ReadStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    ReadStatement(std::vector<Identifier *> variables) : variables(variables) {}
    ~ReadStatement();

    ReadStatement *asReadStatement() override;
  };

  // isso poderia ser uma chamada de função, mas a spec especifica ela como uma
  // coisa separada, sem falar que isso nos livra de ter que
  // lidar com o conceito de funções "nativas" no interpretador
  class WriteStatement : public Statement
  {
  public:
    std::vector<Identifier *> variables;

    WriteStatement(std::vector<Identifier *> variables) : variables(variables) {}
    ~WriteStatement();

    WriteStatement *asWriteStatement() override;
  };

  class CallStatement : public Statement
  {
  public:
    Identifier *procedureName;
    std::vector<Identifier *> arguments;

    CallStatement(Identifier *procedureName, std::vector<Identifier *> arguments)
        : procedureName(procedureName), arguments(arguments) {}
    ~CallStatement();

    CallStatement *asCallStatement() override;
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
    ~IfStatement();

    IfStatement *asIfStatement() override;
  };

  class WhileStatement : public Statement
  {
  public:
    Condition *condition;
    Statement *body;

    WhileStatement(Condition *condition, Statement *body)
        : condition(condition), body(body) {}
    ~WhileStatement();

    WhileStatement *asWhileStatement() override;
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

    ~ForStatement();

    ForStatement *asForStatement() override;
  };

  class VariableAssignment : public Statement
  {
  public:
    Identifier *variable;
    Expression *value;

    VariableAssignment(Identifier *variable, Expression *value)
        : variable(variable), value(value) {}

    ~VariableAssignment();

    VariableAssignment *asVariableAssignment() override;
  };

  class Block : public Statement
  {
  public:
    std::vector<Statement *> statements;
    Block(std::vector<Statement *> statements) : statements(statements) {}
    ~Block();

    Block *asBlock();
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