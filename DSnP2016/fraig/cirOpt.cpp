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
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
