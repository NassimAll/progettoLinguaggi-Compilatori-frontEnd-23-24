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
  - La grammatica di secondo livello prevedeva l'aggiunta di If-Statement e del ciclo For.
    - L'If statement prevede una sintassi pressoché simile a quella di un if expression, tranne per il fatto che i rami condizionali non contengono banali espressioni ma possono contenere uno o più statements.
      
            class IfStmtAST : public StatementAST {
              private:
                ExprAST* Cond;
                RootAST* TrueExp;
                RootAST* FalseExp;
              public:
                IfStmtAST(ExprAST* Cond, StatementAST* TrueExp, StatementAST* FalseExp);
                Value *codegen(driver& drv) override;
              };
    - Il for viene gestito dalla classe ForStmtAST, che riceve le componenti dell'header loop e del body loop e genera il codice corrispondente

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
- GRAMMATICA LIVELLO 3:
  - Il terzo livello di grammatica aggiunge la possibilità di utilizzare gli operatori di algebra booleana (AND,OR,NOT) all'interno delle espressioni condizionali.
    La loro implementazione non richiedeva lo sviluppo di classi oggiuntive ma è sufficiente l'aggiunta dei token delle parole chiave e delle righe per la generazione del codice nella funzione che gia gestiva gli          operatori.
    '''
    
        if (Op == 'n') {
          Value *L = LHS->codegen(drv);
          if(!L) return nullptr;
          else return builder->CreateNot(L,"notres");
        }
         case 'a':
          return builder->CreateLogicalAnd(L,R,"andres");
         case 'o':
          return builder->CreateLogicalOr(L,R,"orres");

