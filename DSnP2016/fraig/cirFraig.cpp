/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions
unsigned getHashSize(unsigned s) {
   if (s < 8) return 7;
   if (s < 16) return 13;
   if (s < 32) return 31;
   if (s < 64) return 61;
   if (s < 128) return 127;
   if (s < 512) return 509;
   if (s < 2048) return 1499;
   if (s < 8192) return 4999;
   if (s < 32768) return 13999;
   if (s < 131072) return 59999;
   if (s < 524288) return 100019;
   if (s < 2097152) return 300007;
   if (s < 8388608) return 900001;
   if (s < 33554432) return 1000003;
   if (s < 134217728) return 3000017;
   if (s < 536870912) return 5000011;
   return 7000003;
}
/*******************************/
/*   Global variable and enum  */
/*******************************/
unsigned numHole;
vector<unsigned>* _holes;

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::aigMerge(unsigned domin, unsigned repla) {
   unsigned i, x, j;
   CirGate* hh;
   CirGate** g = CirGate::gateArr;
   CirGate* gg = g[repla];
   CirGate* ff = g[domin];

   for (i = 0, x = gg->fanout.size(); i < x; ++i) {
      j = gg->fanout[i];
      hh = g[j / 2];
      if ((hh->fanin[1] / 2 == repla) && (j % 2 == hh->fanin[1] % 2)) {
         hh->fanin[1] = domin * 2 + (j % 2);
         ff->fanout.push_back(j);
      } else {
         hh->fanin[2] = domin * 2 + (j % 2);
         ff->fanout.push_back(j);
      }
   }
   
   x = ((gg->fanin[1] / 2 == gg->fanin[2] / 2) ?(2) :(3));
   for (i = 1; i < x; ++i) {
      hh = g[gg->fanin[i] / 2];
      for (j = 0; j < hh->fanout.size(); ) {
         if (hh->fanout[j] / 2 != repla) { ++j; continue; }
         if (j != hh->fanout.size() - 1) hh->fanout[j] = hh->fanout.back();
         hh->fanout.pop_back();
      }
   }

   delete g[repla];
   g[repla] = NULL;
}

bool
CirMgr::fallInHole(unsigned aigVar) {
   unsigned i, j, x;
   CirGate** g = CirGate::gateArr;
   unsigned firIn = g[aigVar]->fanin[1] % 1000;
   unsigned secIn = g[aigVar]->fanin[2] % 1000;
   unsigned holeNo = ((firIn * firIn * firIn) + (secIn * secIn * secIn)) % numHole;
   
   firIn = g[aigVar]->fanin[1]; secIn = g[aigVar]->fanin[2];
   for (i = 0, x = _holes[holeNo].size(); i < x; ++i) {
      j = _holes[holeNo][i];
      if (((g[j]->fanin[1] == firIn) && (g[j]->fanin[2] == secIn)) ||
         ((g[j]->fanin[2] == firIn) && (g[j]->fanin[1] == secIn))) {
         aigMerge(j, aigVar);
         return false;
      }
   }
   _holes[holeNo].push_back(aigVar);
   return true;
}

void
CirMgr::strash() {
   unsigned i, j, y;
   CirGate* gg;
   vector<unsigned> estimate;
   unsigned x =  param[0] + param[2] + 1;
   CirGate** g = CirGate::gateArr;
   for (i = param[0] + 1; i < x; ++i) writeList(i, estimate);
   numHole = getHashSize(estimate.size());
   _holes = new vector<unsigned> [numHole];
   estimate.clear();

   for (i = 0, x = param[1]; i < x; ++i) {
      gg = gArr(PIList[i]);
      if (gg == g[PIList[i]]) continue;
      g[PIList[i]] = gg;
      for (j = 0, y = gg->fanout.size(); j < y; ++j) if (((size_t)(g[gg->fanout[j] / 2])) % 2) {
         g[gg->fanout[j] / 2] = (CirGate*)(((size_t)g[gg->fanout[j] / 2]) - 1);
         if (g[gg->fanout[j] / 2]->getTypeStr() != "PO") estimate.push_back(gg->fanout[j] / 2);
      }
   }
   gg = gArr(0);
   if (gg != g[0]) {
      g[0] = gg;
      for (j = 0, y = gg->fanout.size(); j < y; ++j) if (((size_t)(g[gg->fanout[j] / 2])) % 2) {
         g[gg->fanout[j] / 2] = (CirGate*)(((size_t)g[gg->fanout[j] / 2]) - 1);
         if (g[gg->fanout[j] / 2]->getTypeStr() != "PO") estimate.push_back(gg->fanout[j] / 2);
      }
   }
   for (i = 0, x = floatList.size(); i < x; ++i) {
      gg = gArr(floatList[i]);
      if (gg->getTypeStr() == "PO") {
         g[floatList[i]] = gg;
         j = gg->fanin[1] / 2;
         g[j] = gArr(j);
      } else {
         for (size_t k = 1; k < 3; ++k) { j = gg->fanin[k] / 2; g[j] = gArr(j); }
         if (((size_t)g[floatList[i]]) % 2) { estimate.push_back(floatList[i]); g[floatList[i]] = gg; }
      }
   }

   for (i = 0; i < estimate.size(); ++i) {
      gg = g[estimate[i]];
      for (j = 0, y = gg->fanout.size(); j < y; ++j) if (((size_t)(g[gg->fanout[j] / 2])) % 2) {
         g[gg->fanout[j] / 2] = (CirGate*)(((size_t)g[gg->fanout[j] / 2]) - 1);
         if (g[gg->fanout[j] / 2]->getTypeStr() != "PO") estimate.push_back(gg->fanout[j] / 2);
      }
      if (!fallInHole(estimate[i])) --param[3];
   }

   estimate.clear();
   for (i = 0, x = floatList.size(); i < x; ++i)
      if (g[floatList[i]]) estimate.push_back(floatList[i]);
   floatList.swap(estimate);
   CirGate::flipped.clear();
   delete [] _holes;
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
