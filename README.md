# progettoLinguaggi-Compilatori-frontEnd-23-24

PROGETTO DI LINGUAGGI E COMPILATORI - FRONT END 

STUDENTE: Allagui Nassim

Il progetto da me consegnato è sviluppato fino alla grammatica di terzo livello completa.
In seguito spiego le effettive modifiche introdotte per la realizzazione di ogni livello di grammatica. 

- GRAMMATICA LIVELLO 1: 
  La grammatica di primo livello prevede l'aggiunta di: 
  - Varibili globali
    - Per l'implementazione delle variabili globali è stata implementata la classe GlobalVariableAST
      
          class GlobalVariableAST : public StatementAST {
            public:
              GlobalVariableAST(const std::string& name);
              Value* codegen(driver& drv) override;
              const std::string& getName() const;
      
            private:
              std::string name;
          };
  - Statement singoli e a blocchi
    - Sono state implementate le classi StatementAST, che è una classe base da cui ereditano tutti gli ogetti che rappresentano statement e la classe BlockAST che gestisce l'insieme di più statement.

          class StatementAST : public RootAST {};
    
           class BlockAST : public ExprAST {
                private:
                  std::vector<VarBindingAST*> Def;
                  std::vector<StatementAST*> Stmts;
                public:
                  BlockAST(std::vector<VarBindingAST*> Def, std::vector<StatementAST*> Stmts);
                  BlockAST(std::vector<StatementAST*> Stmts);
                  Value *codegen(driver& drv) override;
              };
  - Definizioni e assegnazioni di variabili
    - La definizione e l'assegnazione di variabile vengono gestite contemporaneamente da un unica classe VarBindingAST, che valutando lo scope della variabile in esame capisce che operazione eseguire su di essa
  
              class VarBindingAST : public StatementAST {
                public:
                  const std::string Name;
                  ExprAST* Val;
                public:
                  VarBindingAST(const std::string Name, ExprAST* Val);
                  Value *codegen(driver& drv) override;
                  const std::string& getName() const;
                };
  - Operazioni aritmetiche
  - Expression If
 
- GRAMMATICA LIVELLO 2:

- GRAMMATICA LIVELLO 3:
