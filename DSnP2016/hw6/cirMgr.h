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

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
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

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist();
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&);

private:
   bool netList(unsigned, unsigned&);
   void writeList(unsigned, vector<unsigned>&);
   vector<unsigned> floatList;
   vector<unsigned> unusedList;
   unsigned* PIList;
   unsigned param[4];
};

#endif // CIR_MGR_H
