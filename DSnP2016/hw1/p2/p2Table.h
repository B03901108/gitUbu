#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:
   Row(int =0);
   void reset(int =0);
   const int operator[] (size_t i) const {return _data[i];}
   int& operator[] (size_t i) {return _data[i];}

private:
   int* _data;
};

class Table
{
public:
   const Row& operator[] (size_t) const;
   Row& operator[] (size_t);
   //int getNumCol() {return numCol;}

   bool read(const string&);
   void print();
   
   void sum(int);
   void max(int);
   void min(int);
   void count(int);
   void ave(int);

   void add(const string&);
   void clear();
private:
   vector<Row>  _rows;
   int numCol;//the number of columns
   
   char* maxCol;//maxCol[i]->max. of data in Col i
   char* minCol;//minCol[i]->min. of data in Col i
   int* sumCol;//sumCol[i]->summation of data in Col i
   int* sizeCol;//sizeCol[i]->size of Col i
   int* cntCol;//cntCol[i]-># of distinct elements in Col i

   bool** collision;
};

#endif // P2_TABLE_H
