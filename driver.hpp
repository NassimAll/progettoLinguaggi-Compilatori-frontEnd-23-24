#ifndef DRIVER_HPP
#define DRIVER_HPP
/************************* IR related modules ******************************/
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/GlobalVariable.h"

/**************** C++ modules and generic data types ***********************/
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <variant>

#include "parser.hpp"

using namespace llvm;

// Dichiarazione del prototipo yylex per Flex
// Flex va proprio a cercare YY_DECL perché
// deve espanderla (usando M4) nel punto appropriato
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// Per il parser è sufficiente una forward declaration
YY_DECL;

// Classe che organizza e gestisce il processo di compilazione
class driver
{
public:
  driver();
  std::map<std::string, AllocaInst*> NamedValues; // Tabella associativa in cui ogni 
            // chiave x è una variabile e il cui corrispondente valore è un'istruzione 
            // che alloca uno spazio di memoria della dimensione necessaria per 
            // memorizzare un variabile del tipo di x (nel nostro caso solo double)
  RootAST* root;      // A fine parsing "punta" alla radice dell'AST
  int parse (const std::string& f);
  std::string file;
  bool trace_parsing; // Abilita le tracce di debug el parser
  void scan_begin (); // Implementata nello scanner
  void scan_end ();   // Implementata nello scanner
  bool trace_scanning;// Abilita le tracce di debug nello scanner
  yy::location location; // Utillizata dallo scannar per localizzare i token
  void codegen();
};

typedef std::variant<std::string,double> lexval;
const lexval NONE = 0.0;

// Classe base dell'intera gerarchia di classi che rappresentano
// gli elementi del programma
class RootAST {
public:
  virtual ~RootAST() {};
  virtual lexval getLexVal() const {return NONE;};
  virtual Value *codegen(driver& drv) { return nullptr; };
};

// Classe che rappresenta la sequenza di statement
class SeqAST : public RootAST {
private:
  RootAST* first;
  RootAST* continuation;

public:
  SeqAST(RootAST* first, RootAST* continuation);
  Value *codegen(driver& drv) override;
};
/// StatementAST - Classe base per tutti gli Statement
class StatementAST : public RootAST {};

/// ExprAST - Classe base per tutti i nodi espressione
class ExprAST : public StatementAST {};

class BlockAST : public ExprAST {
  private:
    std::vector<VarBindingAST*> Def;
    std::vector<StatementAST*> Stmts;
  public:
  BlockAST(std::vector<VarBindingAST*> Def, std::vector<StatementAST*> Stmts);
  BlockAST(std::vector<StatementAST*> Stmts);
  Value *codegen(driver& drv) override;
};

/// NumberExprAST - Classe per la rappresentazione di costanti numeriche
class NumberExprAST : public ExprAST {
private:
  double Val;

public:
  NumberExprAST(double Val);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

/// GlobalVariable - classe per la definizione di una variable globale
class GlobalVariableAST : public StatementAST {
public:
    GlobalVariableAST(const std::string& name);
    Value* codegen(driver& drv) override;
    const std::string& getName() const;

private:
    std::string name;
};

/// VariableExprAST - Classe per la rappresentazione di riferimenti a variabili
class VariableExprAST : public ExprAST {
private:
  std::string Name;
  
public:
  VariableExprAST(const std::string &Name);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

/// BinaryExprAST - Classe per la rappresentazione di operatori binari
class BinaryExprAST : public ExprAST {
private:
  char Op;
  ExprAST* LHS;
  ExprAST* RHS;

public:
  BinaryExprAST(char Op, ExprAST* LHS, ExprAST* RHS);
  Value *codegen(driver& drv) override;
};

/// CallExprAST - Classe per la rappresentazione di chiamate di funzione
class CallExprAST : public ExprAST {
private:
  std::string Callee;
  std::vector<ExprAST*> Args;  // ASTs per la valutazione degli argomenti

public:
  CallExprAST(std::string Callee, std::vector<ExprAST*> Args);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

/// ForStmtAST - Classe per la rappresentazione del ciclo for
class ForStmtAST : public StatementAST {
private: 
  VarBindingAST* Init;
  ExprAST* Cond;
  VarBindingAST* Step;
  RootAST* Body;

public:
  ForStmtAST(VarBindingAST* Init, ExprAST* Cond, VarBindingAST* Step, RootAST* Body);
  Value *codegen(driver& drv) override;
};

/// IfStmtAST - Classe per la definizione di un construtto if-then else
class IfStmtAST : public StatementAST {
private:
  ExprAST* Cond;
  RootAST* TrueExp;
  RootAST* FalseExp;
public:
  IfStmtAST(ExprAST* Cond, RootAST* TrueExp, RootAST* FalseExp);
  Value *codegen(driver& drv) override;
};

/// IfExprAST - Classe per la definizione di un expression if
class IfExprAST : public ExprAST {
private:
  ExprAST* Cond;
  ExprAST* TrueExp;
  ExprAST* FalseExp;
public:
  IfExprAST(ExprAST* Cond, ExprAST* TrueExp, ExprAST* FalseExp);
  Value *codegen(driver& drv) override;
};

/// VarBindingAST - Classe che gestisce contemporaneamente sia la definizione che l'assegnazione
/// di variabili
class VarBindingAST : public StatementAST {
public:
  const std::string Name;
  ExprAST* Val;
public:
  VarBindingAST(const std::string Name, ExprAST* Val);
  Value *codegen(driver& drv) override;
  const std::string& getName() const;
};

/// PrototypeAST - Classe per la rappresentazione dei prototipi di funzione
/// (nome, numero e nome dei parametri; in questo caso il tipo è implicito
/// perché unico)
class PrototypeAST : public RootAST {
private:
  std::string Name;
  std::vector<std::string> Args;
  bool emitcode;

public:
  PrototypeAST(std::string Name, std::vector<std::string> Args);
  const std::vector<std::string> &getArgs() const;
  lexval getLexVal() const override;
  Function *codegen(driver& drv) override;
  void noemit();
};

/// FunctionAST - Classe che rappresenta la definizione di una funzione
class FunctionAST : public RootAST {
private:
  PrototypeAST* Proto;
  BlockAST* Body;
  bool external;
  
public:
  FunctionAST(PrototypeAST* Proto, BlockAST* Body);
  Function *codegen(driver& drv) override;
};

#endif // ! DRIVER_HH
