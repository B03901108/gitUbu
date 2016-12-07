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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(unsigned l, unsigned len, unsigned id) :lineNo(l) {
      fanin = new unsigned[len];
      fanin[0] = id;
      gateArr[id / 2] = this;
   }
   virtual ~CirGate() { delete [] fanin; }

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return lineNo; }

   // Printing functions
   //virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   static CirGate** gateArr;
   static vector<unsigned> flipped;

   string symbol;
   vector<unsigned> fanout;
   unsigned* fanin;
   unsigned lineNo;
};

class CirPiGate :public CirGate {
public:
   CirPiGate(unsigned l, unsigned id) :CirGate(l, 1, id) {}
   string getTypeStr() const { return "PI"; }
};

class CirPoGate :public CirGate {
public:
   CirPoGate(unsigned l, unsigned id, unsigned ftn) :CirGate(l, 2, id) { fanin[1] = ftn; }
   string getTypeStr() const { return "PO"; }
};

class CirAigGate :public CirGate {
public:
   CirAigGate(unsigned l, unsigned id, unsigned ftn1, unsigned ftn2) :CirGate(l, 3, id) {
      fanin[1] = ftn1;
      fanin[2] = ftn2;
   }
   string getTypeStr() const { return "AIG"; }
};

class CirConstGate :public CirGate {
public:
   CirConstGate() :CirGate(0, 1, 0) {}
   string getTypeStr() const { return "CONST"; }
};

class CirUndefGate :public CirGate {
public:
   CirUndefGate(unsigned id) :CirGate(0, 1, id) {}
   string getTypeStr() const { return "UNDEF"; }
};

#endif // CIR_GATE_H
