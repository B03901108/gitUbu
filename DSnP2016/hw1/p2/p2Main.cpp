#include <iostream>
#include <stdlib.h>
#include "p2Table.h"
using namespace std;
int parsCol(const string&);

int main()
{
   Table table;

   // TODO: read in the csv file
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.

   // TODO read and execute commands
   while (true)
   {
      getline(cin, csvFile);
      //if(csvFile=="RUN") for(int i=0; i<table.getNumCol(); i++) table.sum(i);
      switch(csvFile[1])
      {
         case 'X'://cmd->EXIT
            table.clear();
            exit(0);
         case 'R'://cmd->PRINT
            table.print();
            break;
         case 'U'://cmd->SUM
            table.sum(parsCol(csvFile));
            break;
         case 'A'://cmd->MAX
            table.max(parsCol(csvFile));
            break;
         case 'I'://cmd->MIN
            table.min(parsCol(csvFile));
            break;
         case 'O'://cmd->COUNT
            table.count(parsCol(csvFile));
            break;
         case 'V'://cmd->AVE
            table.ave(parsCol(csvFile));
            break;
         case 'D'://cmd->ADD
            table.add(csvFile);
            break;
         /*default:
            cout << "cmd error." << endl;
            cout << csvFile << endl;*/
      }
   }
}
int parsCol(const string& S)
{
   int argCol=0;
   for(unsigned int i=0; i<S.length(); i++)
   {
      if((S[i]>='0')&&(S[i]<='9'))
      {
         do
         {
            argCol=argCol*10+S[i]-'0';
            i++;
         }
         while((S[i]>='0')&&(S[i]<='9'));
         return argCol;
      }
   }
   return 0;
}
