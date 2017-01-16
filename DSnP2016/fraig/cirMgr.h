/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr():PIList(NULL) { for (unsigned i = 0; i < 4; ++i) param[i] = 0; }
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist();
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&);
   void writeGate(ostream&, CirGate*) const;

private:
   CirGate* gArr(unsigned);
   bool netList(unsigned, unsigned&);
   void writeList(unsigned, vector<unsigned>&);
   void aigRemove(unsigned, unsigned); //aigId, faninLiteral
   bool fallInHole(unsigned);
   void aigMerge(unsigned, unsigned); // dominant, replaced

   vector<unsigned> floatList;
   vector<unsigned> unusedList;
   ofstream* _simLog;
   unsigned* PIList;
   unsigned param[4];
};

#endif // CIR_MGR_H
