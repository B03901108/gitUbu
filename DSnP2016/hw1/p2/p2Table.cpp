#include <iostream>
#include <iomanip>
#include <fstream>
#include "p2Table.h"
using namespace std;

// Implement member functions of class Row and Table here
Row::Row(int x)
{
  if(x>0) _data=new int[x];
  else _data=NULL;
}
void Row::reset(int x)//no memory free! Just alloc new memory.
{
  if(x>0) _data=new int[x];
  else _data=NULL;
}

const Row& Table::operator[](size_t i) const
{
  return _rows[i];
}
Row& Table::operator[](size_t i)
{
  return _rows[i];
}
bool Table::read(const string& csvFile)
{
    numCol=1;//initialize numCol
    string fline;
    char* csvTitle=new char[csvFile.length()+1];//substitute for the str.
    for(unsigned int i=0; i<=csvFile.length(); i++) csvTitle[i]=csvFile[i];

    ifstream FILE(csvTitle);
    getline(FILE, fline);
    /*for(int i=0; i<fline.length(); i++) cout << (int)fline[i] << ' ';
    cout << endl;*/
    if(_rows.size()!=0) _rows.clear();
    for(int i=0; fline[i]!=13; i++) if(fline[i]==',') numCol++;
    //char->space-saving
    sumCol=new int[numCol];
    maxCol=new char[numCol];
    minCol=new char[numCol];
    sizeCol=new int[numCol];
    cntCol=new int[numCol];
    for(int i=0; i<numCol; i++)
    {//initialize parameters of the columns
        sumCol[i]=0;
        maxCol[i]=-100;//range:-99~100
        minCol[i]=101;//range:-99~100
        sizeCol[i]=0;
        cntCol[i]=0;
    }
    //time-saving
    if(numCol<200)
    {
        collision=new bool* [numCol];
        for(int i=0; i<numCol; i++)
        {
            collision[i]=new bool[200];
            for(int j=0; j<200; j++) collision[i][j]=true;//no collision
        }
    }
    else
    {
        collision=new bool* [200];
        for(int i=0; i<200; i++)
        {
            collision[i]=new bool[numCol];
            for(int j=0; j<numCol; j++) collision[i][j]=true;//no collision
        }
    }

    bool neg=false;//record if the element < 0
    char tmpEle=101;//101->empty
    Row tmpRow(numCol);
    for(unsigned int i=0, j=0; i<fline.length(); i++)
    {//j is the cursor on tmpRow.
        if(fline[i]=='-') neg=true;
        else if(fline[i]==',')
        {
            tmpRow[j]=((neg)?(-tmpEle):(tmpEle));
            if(tmpRow[j]<101)
            {
                sumCol[j]+=tmpRow[j];
                sizeCol[j]++;
                if(tmpRow[j]<minCol[j]) minCol[j]=tmpRow[j];
                if(tmpRow[j]>maxCol[j]) maxCol[j]=tmpRow[j];
                if(numCol<200)
                {
                    if(collision[j][99+(int)tmpRow[j]])
                    {
                        cntCol[j]++;
                        collision[j][99+(int)tmpRow[j]]=false;
                    }
                }
                else if(collision[99+(int)tmpRow[j]][j])
                {
                    cntCol[j]++;
                    collision[99+(int)tmpRow[j]][j]=false;
                }                    
            }
            neg=false;
            tmpEle=101;
            j++;
        }
        else if(fline[i]==13)
        {
            tmpRow[j]=((neg)?(-tmpEle):(tmpEle));
            if(tmpRow[j]<101)
            {
                sumCol[j]+=tmpRow[j];
                sizeCol[j]++;
                if(tmpRow[j]<minCol[j]) minCol[j]=tmpRow[j];
                if(tmpRow[j]>maxCol[j]) maxCol[j]=tmpRow[j];
                if(numCol<200)
                {
                    if(collision[j][99+(int)tmpRow[j]])
                    {
                        cntCol[j]++;
                        collision[j][99+(int)tmpRow[j]]=false;
                    }
                }
                else if(collision[99+(int)tmpRow[j]][j])
                {
                    cntCol[j]++;
                    collision[99+(int)tmpRow[j]][j]=false;
                }
            }
            neg=false;
            tmpEle=101;
            j=0;
            _rows.push_back(tmpRow);
            if((i==fline.length()-1)||(fline[i+1]==13)) break;
            //avoid redundent carriage return
            tmpRow.reset(numCol);//new row
        }
        else
        {
            if(tmpEle>100) tmpEle=fline[i]-'0';
            else tmpEle=tmpEle*10+fline[i]-'0';
        }
    }

    delete [] csvTitle;
    return true;
}    
void Table::add(const string& S)
{
    bool neg=false;//whether the element < 0 or not
    char tmpEle=101;//101->null cell
    Row tmpRow(numCol);

    for(unsigned int i=4, j=0; i<=S.length(); i++)
    {//j is the cursor on tmpRow.
        if(S[i]=='-') neg=true;
        else if((S[i]>='0')&&(S[i]<='9'))
        {
            if(tmpEle>100) tmpEle=S[i]-'0';
            else tmpEle=tmpEle*10+S[i]-'0';
        }
        else
        {
            if(tmpEle>100) tmpRow[j]=101;
            else
            {
                tmpRow[j]=((neg)?(-tmpEle):(tmpEle));
                sumCol[j]+=tmpRow[j];
                sizeCol[j]++;
                if(tmpRow[j]<minCol[j]) minCol[j]=tmpRow[j];
                if(tmpRow[j]>maxCol[j]) maxCol[j]=tmpRow[j];
                if(numCol<200)
                {
                    if(collision[j][99+(int)tmpRow[j]])
                    {
                        cntCol[j]++;
                        collision[j][99+(int)tmpRow[j]]=false;
                    }
                }
                else if(collision[99+(int)tmpRow[j]][j])
                {
                    cntCol[j]++;
                    collision[99+(int)tmpRow[j]][j]=false;
                }
            }
            neg=false;
            tmpEle=101;
            j++;
            if((int)j==numCol)
            {
               _rows.push_back(tmpRow);
               break;
            }
        }
    }
}
void Table::print()
{
    for(unsigned int i=0; i<_rows.size(); i++)
    {
        for(int j=0; j<numCol; j++)
	{
            if(_rows[i][j]>100) cout << right << setw(4) << ' ';//101->empty
            else cout << right << setw(4)  << _rows[i][j];
        }
        cout << endl;
    }
    //for(int i=0; i<numCol; i++) {count(i); ave(i);}
}
void Table::sum(int x)
{
   cout << "The summation of data in column #" << x
        << " is " << sumCol[x] << '.' << endl;
}
void Table::max(int x)
{
   cout << "The maximum of data in column #" << x
        << " is " << (int)maxCol[x] << '.' << endl;
}
void Table::min(int x)
{
   cout << "The minimum of data in column #" << x
        << " is " << (int)minCol[x] << '.' << endl;
}
void Table::count(int x)
{
   cout << "The distinct count of data in column #" << x
        << " is " << cntCol[x] << '.' << endl;
}
void Table::ave(int x)
{
   cout << "The average of data in column #" << x << " is ";
   ios::fmtflags F(cout.flags());
   cout << fixed;
   cout << setprecision(1) << ((double)sumCol[x])/((double)sizeCol[x]) << '.' << endl;
   cout.flags(F);
}
void Table::clear()
{
   for(unsigned int i=0; i<_rows.size(); i++) delete [] &(_rows[i][0]);

   if(numCol<200) for(int i=0; i<numCol; i++) delete [] collision[i];
   else for(int i=0; i<200; i++) delete [] collision[i];
   delete [] collision;

   delete [] maxCol;
   delete [] minCol;
   delete [] sumCol;
   delete [] sizeCol;
   delete [] cntCol;
   _rows.clear();
}

