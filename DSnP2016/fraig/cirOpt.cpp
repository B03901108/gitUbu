/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions
CirGate*
CirMgr::gArr(unsigned index) {
   CirGate* recovered = CirGate::gateArr[index];
   if (((size_t)recovered) % 2) recovered = (CirGate*)(((size_t)recovered) - 1);
   return recovered;
}
/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep() {
   unsigned k, cursorFL = 0;
   vector<unsigned> dummyArr;
   unsigned i = unusedList.size() + 1;
   unsigned j = param[0] + param[2] + 1;
   CirGate** g = CirGate::gateArr;
   do { --i; if (i == 0) return; } while (g[unusedList[i - 1]]->getTypeStr() == "PI");

   unusedList.clear();
   for (i = param[0] + 1; i < j; ++i) { writeList(i, dummyArr); g[i] = (CirGate*)(((size_t)g[i]) - 1); }
   param[3] = dummyArr.size();
   dummyArr.clear();
   while (floatList.size()) { dummyArr.push_back(floatList.back()); floatList.pop_back(); }

   for (i = param[0]; i > 0; --i) {
      if (!g[i]) continue;

      if (((size_t)g[i]) % 2) {
         g[i] = (CirGate*)(((size_t)g[i]) - 1);
         for (j = 0; j < g[i]->fanout.size(); ) {
            k = ((g[i]->fanout)[j]) / 2;
            if ((((size_t)g[k]) % 2) || ((k > i) && (g[k]))) { ++j; continue; }
            if (j != g[i]->fanout.size() - 1) (g[i]->fanout)[j] = g[i]->fanout.back();
            g[i]->fanout.pop_back();
         }
      } else if (g[i]->getTypeStr() == "PI") {
         unusedList.push_back(i);
         g[i]->fanout.clear();
      } else {
         cout << "Sweeping: " << g[i]->getTypeStr() << '(' << i << ") removed..." << endl;
         delete g[i]; g[i] = NULL;
         while ((cursorFL < dummyArr.size()) && (i <= dummyArr[cursorFL])) {
            if (i == dummyArr[cursorFL]) dummyArr[cursorFL] = 0;
            ++cursorFL;
         }
      }
   }
   if (((size_t)g[0]) % 2) {
      g[0] = (CirGate*)(((size_t)g[0]) - 1);
      for (j = 0; j < g[0]->fanout.size(); ) {
         k = ((g[0]->fanout)[j]) / 2;
         if (g[k]) { ++j; continue; }
         if (j != g[0]->fanout.size() - 1) (g[0]->fanout)[j] = g[0]->fanout.back();
         g[0]->fanout.pop_back();
      }
   } else g[0]->fanout.clear();

   while (dummyArr.size()) {
      if (dummyArr.back() != 0) floatList.push_back(dummyArr.back());
      dummyArr.pop_back();
   }

   CirGate::flipped.clear();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::aigRemove(unsigned aigVar, unsigned eqLit) {
   unsigned i, x, j;
   CirGate* hh;
   CirGate* gg = gArr(aigVar);
   CirGate* ff = gArr(eqLit / 2);
   for (i = 0, x = gg->fanout.size(); i < x; ++i) {
      j = gg->fanout[i];
      hh = gArr(j / 2);
      if ((hh->fanin[1] / 2 == aigVar) && (j % 2 == hh->fanin[1] % 2)) {
         hh->fanin[1] = (eqLit / 2) * 2 + ((eqLit + j) % 2);
         ff->fanout.push_back((j / 2) * 2 + (hh->fanin[1] % 2));
      } else {
         hh->fanin[2] = (eqLit / 2) * 2 + ((eqLit + j) % 2);
         ff->fanout.push_back((j / 2) * 2 + (hh->fanin[2] % 2));
      }
   }
   
   x = ((gg->fanin[1] / 2 == gg->fanin[2] / 2) ?(2) :(3));
   for (i = 1; i < x; ++i) {
      hh = gArr(gg->fanin[i] / 2);
      for (j = 0; j < hh->fanout.size(); ) {
         if (hh->fanout[j] / 2 != aigVar) { ++j; continue; }
         if (j != hh->fanout.size() - 1) hh->fanout[j] = hh->fanout.back();
         hh->fanout.pop_back();
      }
      if ((!j) && (hh->getTypeStr() == "UNDEF")) {
         delete hh;
         CirGate::gateArr[gg->fanin[i] / 2] = NULL;
      }
   }

   delete gg; --param[3];
   CirGate::gateArr[aigVar] = NULL;
}

void
CirMgr::optimize() {
   unsigned i, j, y;
   CirGate* gg;
   string tmpType;
   vector<unsigned> aigOrdered;
   unsigned x =  param[0] + param[2] + 1;
   CirGate** g = CirGate::gateArr;

   for (i = param[0] + 1; i < x; ++i) {
      writeList(i, aigOrdered);
      for (j = 0, y = aigOrdered.size(); j < y; ++j) {
         gg = gArr(aigOrdered[j]);
         if (gg->fanin[1] * gg->fanin[2] == 0) aigRemove(aigOrdered[j], 0);
         else if (gg->fanin[1] == 1) aigRemove(aigOrdered[j], gg->fanin[2]);
         else if (gg->fanin[2] == 1) aigRemove(aigOrdered[j], gg->fanin[1]);
         else if (gg->fanin[1] == gg->fanin[2]) aigRemove(aigOrdered[j], gg->fanin[1]);
         else if (gg->fanin[1] / 2 == gg->fanin[2] / 2) aigRemove(aigOrdered[j], 0);
      }
      aigOrdered.clear();
   }
   while (CirGate::flipped.size()) {
      i = CirGate::flipped.back();
      CirGate::flipped.pop_back();
      if (g[i]) g[i] = (CirGate*)(((size_t)g[i]) - 1);
   }

   floatList.clear(); unusedList.clear();
   y = param[0] + 1;
   for (i = 1; i < y; ++i) if (g[i]) {
      tmpType = g[i]->getTypeStr();
      if (tmpType == "UNDEF") continue;
      if ((tmpType == "AIG") && ((g[g[i]->fanin[1] / 2]->getTypeStr() == "UNDEF") ||
         (g[g[i]->fanin[2] / 2]->getTypeStr() == "UNDEF"))) floatList.push_back(i);
      if (!g[i]->fanout.size()) unusedList.push_back(i);
   }
   for (; y < x; ++y) if (g[g[i]->fanin[1] / 2]->getTypeStr() == "UNDEF") floatList.push_back(i);
   for (i = 0, x = unusedList.size() / 2, y = unusedList.size() - 1; i < x; ++i) {
      unusedList[i] = unusedList[i] + unusedList[y - i];
      unusedList[y - i] = unusedList[i] - unusedList[y - i];
      unusedList[i] = unusedList[i] - unusedList[y - i];
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
