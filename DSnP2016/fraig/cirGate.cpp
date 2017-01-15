/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

char uIntLen(unsigned numIn) {
   char digit = 0;
   if (numIn == 0) return ((char)1);
   while (numIn != 0) { numIn /= 10; ++digit; }
   return digit;
}

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate** CirGate::gateArr = NULL;
vector<unsigned> CirGate::flipped = vector<unsigned>();

void
CirGate::reportGate() const {
   char j = 12 + (char(getTypeStr().length())) + uIntLen(fanin[0] / 2) + uIntLen(lineNo);
   if (symbol.length() != 0) j += (symbol.length() + 2);
   for (char i = 0; i < 50; ++i) cout << '=';
   cout << "\n= " << getTypeStr() << '(' << (fanin[0] / 2) << ')';
   if (symbol.length() != 0) cout << '\"' << symbol << '\"';
   cout << ", line " << lineNo;
   for (; j < 50; ++j) cout << ' '; cout << "=\n";
   for (char i = 0; i < 50; ++i) cout << '=';
}

void
CirGate::dfsIn(int nowLevel, int level, char info) {
   size_t ptrCast;
   unsigned n = fanin[0] / 2;
   string s = getTypeStr();
   for (int i = 2 * nowLevel; i > 0; --i) cout << ' ';
   if (info % 2) cout << '!';
   cout << s << ' ' << n;
   if (info > 1) { cout << " (*)" << endl; return; }
   cout << endl;
   if (nowLevel == level) return;

   if ((s == "PO") || (s == "AIG")) {
      gateArr[n] = (CirGate*)(((size_t)gateArr[n]) + 1);
      flipped.push_back(n);
      
      for (char i = 1, j = ((s == "PO") ?(2) :(3)); i < j; ++i) {
         info = ((fanin[i] % 2) ?(1) :(0));
         ptrCast = (size_t)(gateArr[fanin[i] / 2]);
         if (ptrCast % 2) { info += 2; --ptrCast; }
         ((CirGate*)ptrCast)->dfsIn(nowLevel + 1, level, info);
      }
   }
}

void
CirGate::dfsOut(int nowLevel, int level, char info) {
   size_t ptrCast, round;
   unsigned n = fanin[0] / 2;
   for (int i = 2 * nowLevel; i > 0; --i) cout << ' ';
   if (info % 2) cout << '!';
   cout << getTypeStr() << ' ' << n;
   if (info > 1) { cout << " (*)" << endl; return; }
   cout << endl;
   if (nowLevel == level) return;

   round = fanout.size();
   if (round != 0) {
      gateArr[n] = (CirGate*)(((size_t)gateArr[n]) + 1);
      flipped.push_back(n);
      
      for (size_t i = 0; i < round; ++i) {
         info = ((fanout[i] % 2) ?(1) :(0));
         ptrCast = (size_t)(gateArr[fanout[i] / 2]);
         if (ptrCast % 2) { info += 2; --ptrCast; }
         ((CirGate*)ptrCast)->dfsOut(nowLevel + 1, level, info);
      }
   }
}

void
CirGate::reportFanin(int level) {
   assert (level >= 0);
   unsigned j;
   dfsIn(0, level);
   for (size_t i = flipped.size(); i > 0; --i) {
      j = flipped[i - 1];
      gateArr[j] = (CirGate*)(((size_t)gateArr[j]) - 1);
   }
   flipped.clear();
}

void
CirGate::reportFanout(int level) {
   assert (level >= 0);
   unsigned j;
   dfsOut(0, level);
   for (size_t i = flipped.size(); i > 0; --i) {
      j = flipped[i - 1];
      gateArr[j] = (CirGate*)(((size_t)gateArr[j]) - 1);
   }
   flipped.clear();
}
