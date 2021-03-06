/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr() {
   if (PIList) delete [] PIList;
   unsigned x = param[2] + param[0] + 1;
   CirGate** g = CirGate::gateArr;
   if (g) {
      for (unsigned i = 0; i < x; ++i) if (g[i]) delete g[i];
      delete [] g;
      CirGate::gateArr = NULL;
   }
}

CirGate*
CirMgr::getGate(unsigned id) const {
   return CirGate::gateArr[id];
}

bool
CirMgr::readCircuit(const string& fileName)
{
   lineNo = 0;
   int tmpNum;
   unsigned upbLine;
   CirGate** g;
   ifstream FILE(fileName.c_str());
   if (!FILE.is_open()) {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

   getline(FILE, errMsg, ' ');
   if (errMsg != "aag") return parseError(ILLEGAL_IDENTIFIER);
   for (size_t i = 0, j = 0; i < 5; ++i) {
      if (i == 4) getline(FILE, errMsg);
      else getline(FILE, errMsg, ' ');
      myStr2Int(errMsg, tmpNum);
      if (i != 2) { param[(j++)] = (unsigned)tmpNum; }
   }

   ++lineNo;
   upbLine = param[0] + param[2] + 1;
   PIList = new unsigned[param[1]];
   g = CirGate::gateArr = new CirGate* [upbLine];
   while (upbLine > 0) g[(--upbLine)] = NULL;

   upbLine = param[1];
   for (; lineNo <= upbLine; ++lineNo) {
      getline(FILE, errMsg);
      myStr2Int(errMsg, tmpNum);
      new CirPiGate(lineNo + 1, (unsigned)tmpNum);
      PIList[lineNo - 1] = (unsigned)(tmpNum / 2);
   }

   upbLine += param[2];
   for (unsigned tmpid = param[0] + 1; lineNo <= upbLine; ++lineNo, ++tmpid) {
      getline(FILE, errMsg);
      myStr2Int(errMsg, tmpNum);
      new CirPoGate(lineNo + 1, tmpid * 2, (unsigned)tmpNum);
   }

   upbLine += param[3];
   for (int i = 0, j = 0; lineNo <= upbLine; ++lineNo) {
      getline(FILE, errMsg, ' '); myStr2Int(errMsg, tmpNum);
      getline(FILE, errMsg, ' '); myStr2Int(errMsg, i);
      getline(FILE, errMsg); myStr2Int(errMsg, j);
      new CirAigGate(lineNo + 1, (unsigned)tmpNum, (unsigned)i, (unsigned)j);
   }

   while ((getline(FILE, errMsg)) && (errMsg.length() > 1)) {
      upbLine = errMsg.find(' ');
      myStr2Int(errMsg.substr(1, upbLine - 1), tmpNum);
      if (errMsg[0] == 'o') g[param[0] + ((unsigned)tmpNum) + 1]->symbol = errMsg.substr(upbLine + 1);
      else if (errMsg[0] == 'i') g[PIList[tmpNum]]->symbol = errMsg.substr(upbLine + 1);
   }

   new CirConstGate();
   upbLine = param[0] + param[2] + 1;
   for (unsigned i = 1, j; i < upbLine; ++i) if (g[i]) {
      errMsg = g[i]->getTypeStr();
      if (errMsg == "AIG") lineNo = 3;
      else if (errMsg == "PO") lineNo = 2;
      else continue;
      for (colNo = 1; colNo < lineNo; ++colNo) {
         j = g[i]->fanin[colNo];
         if (!g[(j / 2)]) { g[(j / 2)] = new CirUndefGate((j / 2) * 2); floatList.push_back(i); }
         else if (g[(j / 2)]->getTypeStr() == "UNDEF") floatList.push_back(i);
         g[(j / 2)]->fanout.push_back(2 * i + (j % 2));
      }
   }
   for (upbLine = param[0]; upbLine > 0; --upbLine)
      if ((g[upbLine]) && (g[upbLine]->fanout.size() == 0)) unusedList.push_back(upbLine);
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const {
   cout << "\nCircuit Statistics\n";
   for (char i = 0; i < 18; ++i) cout << '=';
   cout << "\n  ";
   cout << left << setw(7) << "PI";
   cout << right << setw(7) << param[1];
   cout << "\n  ";
   cout << left << setw(7) << "PO";
   cout << right << setw(7) << param[2];
   cout << "\n  ";
   cout << left << setw(7) << "AIG";
   cout << right << setw(7) << param[3];
   cout << endl;
   for (char i = 0; i < 18; ++i) cout << '-';
   cout << "\n  ";
   cout << left << setw(7) << "Total";
   cout << right << setw(7) << (param[1] + param[2] + param[3]);
   cout << endl;
}

bool
CirMgr::netList(unsigned index, unsigned& lineNum) {
   char tmpChar = 0;
   CirGate** g = CirGate::gateArr;
   CirGate* gg = g[index];
   if (((size_t)gg) % 2) return true;

   string s = gg->getTypeStr();
   if (s == "UNDEF") return false;
   if (index == 0) cout << '[' << lineNum << "] CONST0" << endl;
   else if (s == "PI") {
      cout << '[' << lineNum << "] PI\t" << index;
      if (gg->symbol.length() != 0) cout << "\t(" << gg->symbol << ')';
      cout << endl;
   } else if (index > param[0]) {
      if (!netList((gg->fanin[1]) / 2, lineNum)) ++tmpChar;
      cout <<  '[' << lineNum << "] PO\t" << index << '\t';
      if (tmpChar > 0) cout << '*';
      if (gg->fanin[1] % 2) cout << '!';
      cout << (gg->fanin[1] / 2);
      if (gg->symbol.length() != 0) cout << "\t(" << gg->symbol << ')';
      cout << endl;
   } else {
      for (char i = 1; i < 3; ++i) if (!netList((gg->fanin[i]) / 2, lineNum)) tmpChar += i;
      cout <<  '[' << lineNum << "] AIG\t" << index << '\t';
      if (tmpChar % 2) cout << '*';
      if ((gg->fanin[1]) % 2) cout << '!';
      cout << ((gg->fanin[1]) / 2) << '\t';
      if (tmpChar > 1) cout << '*';
      if ((gg->fanin[2]) % 2) cout << '!';
      cout << ((gg->fanin[2]) / 2) << endl;
   }

   ++lineNum;
   g[index] = (CirGate*)(((size_t)gg) + 1);
   CirGate::flipped.push_back(index);
   return true;
}

void
CirMgr::printNetlist() {
   unsigned j =  param[0] + param[2] + 1;
   CirGate** g = CirGate::gateArr;
   for (unsigned i = param[0] + 1, k = 0; i < j; ++i) netList(i, k);
   for (size_t i = CirGate::flipped.size(); i > 0; --i) {
      j = CirGate::flipped[i - 1];
      g[j] = (CirGate*)(((size_t)g[j]) - 1);
   }
   CirGate::flipped.clear();
}

void
CirMgr::printPIs() const {
   cout << "PIs of the circuit:";
   for (unsigned i = 0, j = param[1]; i < j; ++i) cout << ' ' << PIList[i];
   cout << endl;
}

void
CirMgr::printPOs() const {
   cout << "POs of the circuit:";
   for (unsigned i = param[0] + 1, j = param[2]; j > 0; ++i, --j) cout << ' ' << i;
   cout << endl;
}

void
CirMgr::printFloatGates() const {
   size_t x = floatList.size();
   if (x > 0) {
      cout << "Gates with floating fanin(s):";
      cout << ' ' << floatList[0];
      for (size_t i = 1; i < x; ++i)
         if (floatList[i] != floatList[i - 1]) cout << ' ' << floatList[i];
      cout << endl;
   }
   x = unusedList.size();
   if (x > 0) {
      cout << "Gates defined but not used  :";
      for (; x > 0; --x) cout << ' ' << unusedList[x - 1];
      cout << endl;
   }
}

void
CirMgr::writeList(unsigned index, vector<unsigned>& V) {
   CirGate** g = CirGate::gateArr;
   CirGate* gg = g[index];
   if (((size_t)gg) % 2) return;

   if (index > param[0]) writeList((gg->fanin[1]) / 2, V);
   else if (gg->getTypeStr() == "AIG") {
      for (char i = 1; i < 3; ++i) writeList((gg->fanin[i]) / 2, V);
      V.push_back(index);
   }

   g[index] = (CirGate*)(((size_t)gg) + 1);
   CirGate::flipped.push_back(index);
}

void
CirMgr::writeAag(ostream& outfile) {
   unsigned i;
   unsigned* tmpPtr;
   string tmpStr;
   vector<unsigned> aigOrdered;
   unsigned j =  param[0] + param[2] + 1;
   CirGate** g = CirGate::gateArr;

   for (i = param[0] + 1; i < j; ++i) writeList(i, aigOrdered);
   for (i = CirGate::flipped.size(); i > 0; --i) {
      j = CirGate::flipped[i - 1];
      g[j] = (CirGate*)(((size_t)g[j]) - 1);
   }
   CirGate::flipped.clear();

   outfile << "aag " << param[0] << ' ' << param[1] << ' ' << 0;
   outfile << ' ' << param[2] << ' ' << aigOrdered.size() << endl;

   j =  param[0] + param[2] + 1;
   for (i = 0; i < param[1]; ++i) outfile << (PIList[i] * 2) << endl;
   for (i = param[0] + 1; i < j; ++i) outfile << g[i]->fanin[1] << endl;
   for (i = 0, j = aigOrdered.size(); i < j; ++i) {
      tmpPtr = g[aigOrdered[i]]->fanin;
      outfile << tmpPtr[0] << ' ' << tmpPtr[1] << ' ' << tmpPtr[2] << endl;
   }

   for (i = 0; i < param[1]; ++i) {
      tmpStr = g[PIList[i]]->symbol;
      if (tmpStr.size() != 0) outfile << 'i' << i << ' ' << tmpStr << endl;
   }
   for (i = param[0] + 1, j = 0; j < param[2]; ++i, ++j) {
      tmpStr = g[i]->symbol;
      if (tmpStr.size() != 0) outfile << 'o' << j << ' ' << tmpStr << endl;
   }
   outfile << 'c' << endl;
   outfile << "generated by Wei-Lun Huang" << endl;
}
