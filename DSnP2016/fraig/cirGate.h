/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate {
public:
   CirGate(unsigned l, unsigned len, unsigned id) :lineNo(l), FECNo(0) {
      fanin = new unsigned[len];
      fanin[0] = id;
      gateArr[id / 2] = this;
   }
   virtual ~CirGate() { delete [] fanin; }

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return lineNo; }
   virtual bool isAig() const = 0;

   // Printing functions
   /* virtual void printGate() const {} */
   void reportGate() const;
   void reportFanin(int level);
   void reportFanout(int level);

   static CirGate** gateArr;
   static unsigned* bitList;
   static vector<unsigned> flipped;
   static vector< vector<unsigned> > FECGSet;

   string symbol;
   vector<unsigned> fanout;
   unsigned* fanin;
   unsigned lineNo;
   unsigned FECNo;

   void dfsIn(int, int, char = '\0');
   void dfsOut(int, int, char = '\0');

private:
protected:
};

class CirPiGate :public CirGate {
public:
   CirPiGate(unsigned l, unsigned id) :CirGate(l, 1, id) {}
   string getTypeStr() const { return "PI"; }
   bool isAig() const { return false; }
};

class CirPoGate :public CirGate {
public:
   CirPoGate(unsigned l, unsigned id, unsigned ftn) :CirGate(l, 2, id) { fanin[1] = ftn; }
   string getTypeStr() const { return "PO"; }
   bool isAig() const { return false; }
};

class CirAigGate :public CirGate {
public:
   CirAigGate(unsigned l, unsigned id, unsigned ftn1, unsigned ftn2) :CirGate(l, 3, id) {
      fanin[1] = ftn1;
      fanin[2] = ftn2;
   }
   string getTypeStr() const { return "AIG"; }
   bool isAig() const { return true; }
};

class CirConstGate :public CirGate {
public:
   CirConstGate() :CirGate(0, 1, 0) {}
   string getTypeStr() const { return "CONST"; }
   bool isAig() const { return false; }
};

class CirUndefGate :public CirGate {
public:
   CirUndefGate(unsigned id) :CirGate(0, 1, id) {}
   string getTypeStr() const { return "UNDEF"; }
   bool isAig() const { return false; }
};

#endif // CIR_GATE_H
