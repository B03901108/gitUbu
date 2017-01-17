/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions
void caseGen(unsigned* c, unsigned x) {
   int seedIn = 1 << 16;
   for (unsigned i = 0; i < x; ++i) {
      c[i] = rnGen(seedIn);
      c[i] = c[i] << 16;
      c[i] += rnGen(seedIn);
   }
}

bool compHead(vector<unsigned>& fir, vector<unsigned>& sec) { return (fir[0] < sec[0]); }

/*******************************/
/*   Global variable and enum  */
/*******************************/
unsigned numSlot;
vector<unsigned>* _slots;

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::bitSim(unsigned index) {
   unsigned i, j, k;
   unsigned* bl = CirGate::bitList;
   CirGate** g = CirGate::gateArr;
   CirGate* gg = g[index];
   if (((size_t)gg) % 2) return;

   if (index > param[0]) {
      i = gg->fanin[1]; bitSim(i / 2);
      bl[index] = ((i % 2) ?(~bl[i / 2]) :(bl[i / 2]));
   } else if (gg->getTypeStr() == "AIG") {
      i = gg->fanin[1]; bitSim(i / 2); j = ((i % 2) ?(~bl[i / 2]) :(bl[i / 2]));
      i = gg->fanin[2]; bitSim(i / 2); k = ((i % 2) ?(~bl[i / 2]) :(bl[i / 2]));
      bl[index] = j & k;
      g[index] = (CirGate*)(((size_t)gg) + 1);
      CirGate::flipped.push_back(index);
   }
}

bool
CirMgr::FECGHash(unsigned index, vector< vector<unsigned> >& newFECG) {
   unsigned i, j, k, l, noSlot;
   unsigned x = CirGate::FECGSet[index].size();
   unsigned* bl = CirGate::bitList;
   numSlot = getHashSize(x);
   _slots = new vector<unsigned> [numSlot];
   for (i = 0; i < x; ++i) {
      j = CirGate::FECGSet[index][i];
      k = bl[j]; l = ((~k) % 1021); k = (k % 1021);
      noSlot = ((k * k * k) + (l * l * l)) % numSlot;
      _slots[noSlot].push_back(j);
   }
   for (i = 0; i < numSlot; ++i) if (_slots[i].size() > 1) {
      newFECG.push_back(vector<unsigned>());
      newFECG.back().swap(_slots[i]);
      if (newFECG.back().size() == x) { delete [] _slots; return false; }
   }
   delete [] _slots;
   return true;
}

void
CirMgr::randomSim() {
   unsigned i, x, j, y;
   vector<unsigned> dummyArr;
   CirGate** g = CirGate::gateArr;
   if (!hasBeenSim) {
      CirGate::FECGSet.push_back(vector<unsigned>());
      CirGate::FECGSet[0].push_back(0);
      for (i = param[0] + 1, x = i + param[2]; i < x; ++i) writeList(i, dummyArr);
      for (i = 1, x = param[0] + 1; i < x; ++i)
         if ((((size_t)g[i]) % 2) && (gArr(i)->getTypeStr() == "AIG")) CirGate::FECGSet[0].push_back(i);
      while (CirGate::flipped.size()) {
         i = CirGate::flipped.back();
         g[i] = (CirGate*)(((size_t)g[i]) - 1);
         CirGate::flipped.pop_back();
      }
      if (CirGate::FECGSet[0].size() < 2) { CirGate::FECGSet.clear(); return; }
   } else for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i)
      for (j = 0, y = CirGate::FECGSet[i].size(); j < y; ++j) {
         CirGate::FECGSet[i][j] /= 2;
         g[CirGate::FECGSet[i][j]]->FECNo = 0;
      }

   vector< vector<unsigned> > newFECG;
   unsigned remainTime = 0;
   size_t caseCount = 0;
   unsigned* bl = CirGate::bitList;
   unsigned* caseIn = new unsigned[param[1]];
   unsigned* caseOut = new unsigned[param[2]];
   while ((CirGate::FECGSet.size()) && (remainTime < 5)) {
      caseGen(caseIn, param[1]);
      caseCount += 32;
      for (i = 0, x = param[1]; i < x; ++i) bl[PIList[i]] = caseIn[i];
      for (i = param[0] + 1, x = i + param[2], j = 0; i < x; ++i, ++j) {
         bitSim(i);
         caseOut[j] = bl[i];
      }
      while (CirGate::flipped.size()) {
         i = CirGate::flipped.back();
         g[i] = (CirGate*)(((size_t)g[i]) - 1);
         CirGate::flipped.pop_back();
      }
      if (_simLog) for (i = 0; i < 32; ++i) {
         for (j = 0, y = param[1]; j < y; ++j) { (*_simLog) << (caseIn[j] % 2); caseIn[j] /= 2; }
         (*_simLog) << ' ';
         for (j = 0, y = param[2]; j < y; ++j) { (*_simLog) << (caseOut[j] % 2); caseOut[j] /= 2; }
         (*_simLog) << endl;
      }

      y = 0; //changeExist
      for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i) if (FECGHash(i, newFECG)) ++y;
      if (y) { remainTime = 0; CirGate::FECGSet.swap(newFECG); }
      else ++remainTime;
      newFECG.clear();
   }

   std::sort(CirGate::FECGSet.begin(), CirGate::FECGSet.end(), compHead);
   for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i) {
      remainTime = CirGate::FECGSet[i][0];
      CirGate::FECGSet[i][0] = remainTime * 2;
      g[remainTime]->FECNo = 1 + 2 * i;
      for (j = 1, y = CirGate::FECGSet[i].size(); j < y; ++j) {
         if (~bl[CirGate::FECGSet[i][j]] != bl[remainTime]) {
            g[CirGate::FECGSet[i][j]]->FECNo = 1 + 2 * i;
            CirGate::FECGSet[i][j] *= 2;
         } else {
         	g[CirGate::FECGSet[i][j]]->FECNo = 2 + 2 * i;
            CirGate::FECGSet[i][j] = 2 * CirGate::FECGSet[i][j] + 1;
         }
      }
   }

   hasBeenSim = true;
   delete [] caseOut;
   delete [] caseIn;
   cout << caseCount << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile) {
   unsigned i, x, j, y;
   vector<unsigned> dummyArr;
   CirGate** g = CirGate::gateArr;
   if (!hasBeenSim) {
      CirGate::FECGSet.push_back(vector<unsigned>());
      CirGate::FECGSet[0].push_back(0);
      for (i = param[0] + 1, x = i + param[2]; i < x; ++i) writeList(i, dummyArr);
      for (i = 1, x = param[0] + 1; i < x; ++i)
         if ((((size_t)g[i]) % 2) && (gArr(i)->getTypeStr() == "AIG")) CirGate::FECGSet[0].push_back(i);
      while (CirGate::flipped.size()) {
         i = CirGate::flipped.back();
         g[i] = (CirGate*)(((size_t)g[i]) - 1);
         CirGate::flipped.pop_back();
      }
      if (CirGate::FECGSet[0].size() < 2) { CirGate::FECGSet.clear(); return; }
   } else for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i)
      for (j = 0, y = CirGate::FECGSet[i].size(); j < y; ++j) {
         CirGate::FECGSet[i][j] /= 2;
         g[CirGate::FECGSet[i][j]]->FECNo = 0;
      }

   vector< vector<unsigned> > newFECG;
   unsigned remainTime = 0;
   size_t caseCount = 0;
   unsigned* bl = CirGate::bitList;
   unsigned* caseIn = new unsigned[param[1]];
   unsigned* caseOut = new unsigned[param[2]];
   while ((CirGate::FECGSet.size()) && (remainTime < 5)) {
      caseGen(caseIn, param[1]);
      caseCount += 32;
      for (i = 0, x = param[1]; i < x; ++i) bl[PIList[i]] = caseIn[i];
      for (i = param[0] + 1, x = i + param[2], j = 0; i < x; ++i, ++j) {
         bitSim(i);
         caseOut[j] = bl[i];
      }
      while (CirGate::flipped.size()) {
         i = CirGate::flipped.back();
         g[i] = (CirGate*)(((size_t)g[i]) - 1);
         CirGate::flipped.pop_back();
      }
      if (_simLog) for (i = 0; i < 32; ++i) {
         for (j = 0, y = param[1]; j < y; ++j) { (*_simLog) << (caseIn[j] % 2); caseIn[j] /= 2; }
         (*_simLog) << ' ';
         for (j = 0, y = param[2]; j < y; ++j) { (*_simLog) << (caseOut[j] % 2); caseOut[j] /= 2; }
         (*_simLog) << endl;
      }

      y = 0; //changeExist
      for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i) if (FECGHash(i, newFECG)) ++y;
      if (y) { remainTime = 0; CirGate::FECGSet.swap(newFECG); }
      else ++remainTime;
      newFECG.clear();
   }

   std::sort(CirGate::FECGSet.begin(), CirGate::FECGSet.end(), compHead);
   for (i = 0, x = CirGate::FECGSet.size(); i < x; ++i) {
      remainTime = CirGate::FECGSet[i][0];
      CirGate::FECGSet[i][0] = remainTime * 2;
      g[remainTime]->FECNo = 1 + 2 * i;
      for (j = 1, y = CirGate::FECGSet[i].size(); j < y; ++j) {
         if (~bl[CirGate::FECGSet[i][j]] != bl[remainTime]) {
            g[CirGate::FECGSet[i][j]]->FECNo = 1 + 2 * i;
            CirGate::FECGSet[i][j] *= 2;
         } else {
         	g[CirGate::FECGSet[i][j]]->FECNo = 2 + 2 * i;
            CirGate::FECGSet[i][j] = 2 * CirGate::FECGSet[i][j] + 1;
         }
      }
   }

   hasBeenSim = true;
   delete [] caseOut;
   delete [] caseIn;
   cout << caseCount << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
