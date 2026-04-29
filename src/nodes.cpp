#include "nodes.hpp"

namespace AST
{
  ConstantDeclaration::~ConstantDeclaration()
  {
    delete identifier;
    delete value;
  }

  VariableDeclaration::~VariableDeclaration()
  {
    delete typeAnnotation;
    for (Identifier *identifier : identifiers)
      delete identifier;
  }

  ConstantDeclaration *Declaration::asConstantDeclaration()
  {
    if (std::holds_alternative<ConstantDeclaration *>(declaration))
      return std::get<ConstantDeclaration *>(declaration);
    return nullptr;
  }

  VariableDeclaration *Declaration::asVariableDeclaration()
  {
    if (std::holds_alternative<VariableDeclaration *>(declaration))
      return std::get<VariableDeclaration *>(declaration);
    return nullptr;
  }

  ProcedureDeclaration *Declaration::asProcedureDeclaration()
  {
    if (std::holds_alternative<ProcedureDeclaration *>(declaration))
      return std::get<ProcedureDeclaration *>(declaration);
    return nullptr;
  }

  DeclarationList::~DeclarationList()
  {
    for (Declaration *declaration : declarations)
      delete declaration;
  }

  Expression::~Expression()
  {
    if (auto identifier = asIdentifier())
      delete identifier;
    else if (auto numberLiteral = asNumberLiteral())
      delete numberLiteral;
    else if (auto unaryExpression = asUnaryExpression())
      delete unaryExpression;
    else if (auto binaryExpression = asBinaryExpression())
      delete binaryExpression;
  }

  Identifier *Expression::asIdentifier()
  {
    if (std::holds_alternative<Identifier *>(expression))
      return std::get<Identifier *>(expression);
    return nullptr;
  }

  NumberLiteral *Expression::asNumberLiteral()
  {
    if (std::holds_alternative<NumberLiteral *>(expression))
      return std::get<NumberLiteral *>(expression);
    return nullptr;
  }

  UnaryExpression *Expression::asUnaryExpression()
  {
    if (std::holds_alternative<UnaryExpression *>(expression))
      return std::get<UnaryExpression *>(expression);
    return nullptr;
  }

  BinaryExpression *Expression::asBinaryExpression()
  {
    if (std::holds_alternative<BinaryExpression *>(expression))
      return std::get<BinaryExpression *>(expression);
    return nullptr;
  }

  UnaryExpression::~UnaryExpression()
  {
    delete operand;
  }

  BinaryExpression::~BinaryExpression()
  {
    delete left;
    delete right;
  }

  Condition::~Condition()
  {
    delete left;
    delete right;
  }

  ReadStatement::~ReadStatement()
  {
    for (Identifier *variable : variables)
      delete variable;
  }

  WriteStatement::~WriteStatement()
  {
    for (Identifier *variable : variables)
      delete variable;
  }

  CallStatement::~CallStatement()
  {
    delete procedureName;
    for (Identifier *argument : arguments)
      delete argument;
  }
  IfStatement::~IfStatement()
  {
    delete condition;
    delete consequent;

    if (alternate != nullptr)
      delete alternate;
  }

  WhileStatement::~WhileStatement()
  {
    delete condition;
    delete body;
  }

  ForStatement::~ForStatement()
  {
    delete initialization;
    delete target;
    delete body;
  }

  VariableAssignment::~VariableAssignment()
  {
    delete variable;
    delete value;
  }

  Statement::~Statement()
  {
    if (auto readStatement = asReadStatement())
      delete readStatement;
    else if (auto writeStatement = asWriteStatement())
      delete writeStatement;
    else if (auto callStatement = asCallStatement())
      delete callStatement;
    else if (auto ifStatement = asIfStatement())
      delete ifStatement;
    else if (auto whileStatement = asWhileStatement())
      delete whileStatement;
    else if (auto forStatement = asForStatement())
      delete forStatement;
    else if (auto variableAssignment = asVariableAssignment())
      delete variableAssignment;
    else if (auto block = asBlock())
      delete block;
  }

  ReadStatement *Statement::asReadStatement()
  {
    if (std::holds_alternative<ReadStatement *>(statement))
      return std::get<ReadStatement *>(statement);
    return nullptr;
  }

  WriteStatement *Statement::asWriteStatement()
  {
    if (std::holds_alternative<WriteStatement *>(statement))
      return std::get<WriteStatement *>(statement);
    return nullptr;
  }

  CallStatement *Statement::asCallStatement()
  {
    if (std::holds_alternative<CallStatement *>(statement))
      return std::get<CallStatement *>(statement);
    return nullptr;
  }

  IfStatement *Statement::asIfStatement()
  {
    if (std::holds_alternative<IfStatement *>(statement))
      return std::get<IfStatement *>(statement);
    return nullptr;
  }

  WhileStatement *Statement::asWhileStatement()
  {
    if (std::holds_alternative<WhileStatement *>(statement))
      return std::get<WhileStatement *>(statement);
    return nullptr;
  }

  ForStatement *Statement::asForStatement()
  {
    if (std::holds_alternative<ForStatement *>(statement))
      return std::get<ForStatement *>(statement);
    return nullptr;
  }

  VariableAssignment *Statement::asVariableAssignment()
  {
    if (std::holds_alternative<VariableAssignment *>(statement))
      return std::get<VariableAssignment *>(statement);
    return nullptr;
  }

  Block *Statement::asBlock()
  {
    if (std::holds_alternative<Block *>(statement))
      return std::get<Block *>(statement);
    return nullptr;
  }

  Block::~Block()
  {
    for (Statement *statement : statements)
      delete statement;
  }

  ParameterDeclaration::~ParameterDeclaration()
  {
    delete identifier;
    delete typeAnnotation;
  }

  ProcedureDeclaration::~ProcedureDeclaration()
  {
    for (ParameterDeclaration *parameter : parameters)
      delete parameter;
    for (VariableDeclaration *localVariable : localVariables)
      delete localVariable;
    delete body;
  }

  Program::~Program()
  {
    delete declarations;
    delete block;
  }
};