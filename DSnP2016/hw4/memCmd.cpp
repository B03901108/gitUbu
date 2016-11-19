/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // _TD_
   vector<string> options;
   CmdExec::lexOptions(option, options);

   int numItem;
   int numSize = 0;
   size_t x = options.size();
   string focusOption;
   if (x < 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   focusOption = options[0];
   if (myStrNCmp("-Array", focusOption, 2) == 0) {
      if (x < 2) return CmdExec::errorOption(CMD_OPT_MISSING, focusOption);
      if ((!myStr2Int(options[1], numSize)) || (numSize < 1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      if (x < 3) return CmdExec::errorOption(CMD_OPT_MISSING, "");
      if ((!myStr2Int(options[2], numItem)) || (numItem < 1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
   }
   else if ((myStr2Int(focusOption, numItem)) && (numItem > 0)) {
      if (x != 1) {
         if (myStrNCmp("-Array", options[1], 2) != 0)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
         if (x < 3) return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
         if ((!myStr2Int(options[2], numSize)) || (numSize < 1))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      }
   }
   else return CmdExec::errorOption(CMD_OPT_ILLEGAL, focusOption);

   if (x > 3) return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
   if (x == 3) mtest.newArrs((size_t)numItem, (size_t)numSize);
   else mtest.newObjs((size_t)numItem);

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // _TD_
   vector<string> options;
   CmdExec::lexOptions(option, options);

   bool rnReq = false;
   int optOnly;
   size_t targetSize;
   size_t delArr = 1;
   size_t x = options.size();
   string focusOption, sizeOption;
   if (x < 1) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   focusOption = options[0];
   if (myStrNCmp("-Array", focusOption, 2) == 0) {
      if (x < 2) return CmdExec::errorOption(CMD_OPT_MISSING, "");
      ++delArr;
      focusOption = options[1];
   }
   if (myStrNCmp("-Random", focusOption, 2) == 0) rnReq = true;
   else if (myStrNCmp("-Index", focusOption, 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, focusOption);
   
   if (x < delArr + 1) return CmdExec::errorOption(CMD_OPT_MISSING, focusOption);
   sizeOption = options[delArr];
   if ((!myStr2Int(sizeOption, optOnly)) || (optOnly < ((rnReq) ?(1) :(0)))) 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, sizeOption);
   if ((delArr < 2) && (x != 2)) {
      if (myStrNCmp("-Array", options[2], 2) != 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      ++delArr;
   }


   if (x > 3) {
      focusOption = options[3];
      if ((myStrNCmp("-Array", focusOption, 2) == 0) || 
         (myStrNCmp("-Random", focusOption, 2) == 0) || 
         (myStrNCmp("-Index", focusOption, 2) == 0))
         return CmdExec::errorOption(CMD_OPT_EXTRA, focusOption);
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, focusOption);
   }
   if (rnReq) {
      if (delArr < 2) {
         targetSize = mtest.getObjListSize();
         if (!targetSize) {
            cerr << "Size of object list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, focusOption);
         }
         for (; optOnly > 0; --optOnly) mtest.deleteObj((size_t)rnGen((int)targetSize));
      }
      else {
         targetSize = mtest.getArrListSize();
         if (!targetSize) {
            cerr << "Size of array list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, focusOption);
         }
         for (; optOnly > 0; --optOnly) mtest.deleteArr((size_t)rnGen((int)targetSize));
      }
   }
   else {
      if (delArr < 2) {
         targetSize = mtest.getObjListSize();
         if ((size_t)optOnly >= targetSize) {
            cerr << "Size of object list (" << targetSize << ") is <= " << optOnly << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, sizeOption);
         }
         mtest.deleteObj((size_t)optOnly);
      }
      else {
         targetSize = mtest.getArrListSize();
         if ((size_t)optOnly >= targetSize) {
            cerr << "Size of array list (" << targetSize << ") is <= " << optOnly << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, sizeOption);
         }
         mtest.deleteArr((size_t)optOnly);
      }
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


