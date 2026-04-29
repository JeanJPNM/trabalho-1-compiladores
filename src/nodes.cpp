#include "nodes.hpp"

namespace AST
{
  Identifier *Expression::asIdentifier() { return nullptr; }
  NumberLiteral *Expression::asNumberLiteral() { return nullptr; }
  UnaryExpression *Expression::asUnaryExpression() { return nullptr; }
  BinaryExpression *Expression::asBinaryExpression() { return nullptr; }

  Identifier *Identifier::asIdentifier() { return this; }

  NumberLiteral *NumberLiteral::asNumberLiteral() { return this; }

  UnaryExpression::~UnaryExpression()
  {
    delete operand;
  }

  UnaryExpression *UnaryExpression::asUnaryExpression() { return this; }

  BinaryExpression::~BinaryExpression()
  {
    delete left;
    delete right;
  }

  BinaryExpression *BinaryExpression::asBinaryExpression() { return this; }

  ConstantDeclaration *Declaration::asConstantDeclaration() { return nullptr; }
  VariableDeclaration *Declaration::asVariableDeclaration() { return nullptr; }
  ProcedureDeclaration *Declaration::asProcedureDeclaration() { return nullptr; }

  ConstantDeclaration::~ConstantDeclaration()
  {
    delete identifier;
    delete value;
  }

  ConstantDeclaration *ConstantDeclaration::asConstantDeclaration() { return this; }

  VariableDeclaration::~VariableDeclaration()
  {
    delete typeAnnotation;
    for (auto identifier : identifiers)
      delete identifier;
  }

  VariableDeclaration *VariableDeclaration::asVariableDeclaration() { return this; }

  ParameterDeclaration::~ParameterDeclaration()
  {
    delete identifier;
    delete typeAnnotation;
  }

  ProcedureDeclaration::~ProcedureDeclaration()
  {
    for (auto parameter : parameters)
      delete parameter;
    for (auto localVariable : localVariables)
      delete localVariable;
    delete body;
  }

  ProcedureDeclaration *ProcedureDeclaration::asProcedureDeclaration() { return this; }

  DeclarationList::~DeclarationList()
  {
    for (auto declaration : declarations)
      delete declaration;
  }

  ReadStatement *asReadStatement() { return nullptr; }
  WriteStatement *asWriteStatement() { return nullptr; }
  CallStatement *asCallStatement() { return nullptr; }
  IfStatement *asIfStatement() { return nullptr; }
  WhileStatement *asWhileStatement() { return nullptr; }
  ForStatement *asForStatement() { return nullptr; }
  VariableAssignment *asVariableAssignment() { return nullptr; }
  Block *asBlock() { return nullptr; }

  Condition::~Condition()
  {
    delete left;
    delete right;
  }

  ReadStatement::~ReadStatement()
  {
    for (auto variable : variables)
      delete variable;
  }

  ReadStatement *ReadStatement::asReadStatement() { return this; }

  WriteStatement::~WriteStatement()
  {
    for (auto variable : variables)
      delete variable;
  }

  WriteStatement *WriteStatement::asWriteStatement() { return this; }

  CallStatement::~CallStatement()
  {
    delete procedureName;
    for (auto argument : arguments)
      delete argument;
  }

  CallStatement *CallStatement::asCallStatement() { return this; }

  IfStatement::~IfStatement()
  {
    delete condition;
    delete consequent;
    if (alternate)
      delete alternate;
  }

  IfStatement *IfStatement::asIfStatement() { return this; }

  WhileStatement::~WhileStatement()
  {
    delete condition;
    delete body;
  }

  WhileStatement *WhileStatement::asWhileStatement() { return this; }

  ForStatement::~ForStatement()
  {
    delete initialization;
    delete target;
    delete body;
  }

  ForStatement *ForStatement::asForStatement() { return this; }

  VariableAssignment::~VariableAssignment()
  {
    delete variable;
    delete value;
  }

  VariableAssignment *VariableAssignment::asVariableAssignment() { return this; }

  Block::~Block()
  {
    for (auto statement : statements)
      delete statement;
  }

  Block *Block::asBlock() { return this; }

  Program::~Program()
  {
    delete name;
    delete declarations;
    delete block;
  }
};