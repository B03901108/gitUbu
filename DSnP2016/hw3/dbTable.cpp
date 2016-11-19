/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   size_t x = r._data.size();
   if (x > 0) {
      if (r._data[0] != INT_MAX) os << r._data[0];
      else os << '.';
      for (size_t i = 1; i < x; ++i){
         os << ' ';
         if (r._data[i] != INT_MAX) os << r._data[i];
         else os << '.';
      }
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   size_t x = t._table.size();
   size_t y = ((x > 0) ?(t._table[0].size()) :(0));
   for (size_t i = 0; i < x; ++i) {
      for (size_t j = 0; j < y; ++j) {
         if (t._table[i][j] != INT_MAX) os << right << setw(6) <<  t._table[i][j];
         else os << right << setw(6) <<  '.';
      }
      os << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   bool noFtn = true;
   string fline;
   getline(ifs, fline);

   if (t._table.size() != 0) t._table.clear();
   bool neg = false;//record if the element < 0
   int tmpEle = INT_MAX;//INT_MAX->empty
   DBRow tmpRow;
   size_t x = fline.length();

   for (size_t i = 0; i < x; ++i) {
      if (fline[i] == '-') neg = true;
      else if (fline[i] == ',') {
         tmpRow.addData((neg) ?(-tmpEle) :(tmpEle));
         neg = false;
         tmpEle = INT_MAX;
      }
      else if (fline[i] == 13) {
         tmpRow.addData((neg) ?(-tmpEle) :(tmpEle));
         neg = false;
         tmpEle = INT_MAX;
         t._table.push_back(tmpRow);

         size_t y = tmpRow.size();
         if (noFtn) {
            t.colMax.reserve(y);
            t.colMin.reserve(y);
            t.colSum.reserve(y);
            t.colSize.reserve(y);
            t.colCount.reserve(y);

            int k;
            for (size_t j = 0; j < y; ++j) {
               k = tmpRow[j];
               t.colMax.push_back(k);
               t.colMin.push_back(k);
               if (k == INT_MAX) {
                  t.colSize.push_back(0);
                  t.colSum.push_back(0);
               }
               else {
                  t.colSize.push_back(1);
                  t.colSum.push_back(k);
               }
            }
            noFtn = false;
         }
         else {
            int k;
            for (size_t j = 0; j < y; ++j) if (tmpRow[j] != INT_MAX) {
               k = tmpRow[j];
               ++t.colSize[j];
               t.colSum[j] += k;
               if ((t.colMax[j] == INT_MAX) || (k > t.colMax[j])) t.colMax[j] = k;
               if (k < t.colMin[j]) t.colMin[j] = k;
            }
         }

         if ((i == x - 1) || (fline[i+1] == 13)) {
            size_t z = t._table.size();
            vector<int> tmpCol;
            for (size_t j = 0; j < y; ++j) {
               for (size_t k = 0; k < z; ++k) tmpCol.push_back(t._table[k][j]);
               std::sort(tmpCol.begin(), tmpCol.end());

               if ((z == 0) || (tmpCol[0] == INT_MAX)) t.colCount.push_back(0);
               else {
                  t.colCount.push_back(1);
                  for (size_t k = 1; ((k < z) && (tmpCol[k] != INT_MAX)); ++k) {
                     if (tmpCol[k] != tmpCol[k - 1]) ++t.colCount[j];
                  }
               }
               tmpCol.clear();
            }
            break;
         }
            //avoid redundent carriage return
         tmpRow.reset();//new row
      }
      else {
         if ((tmpEle == INT_MAX) && (!neg)) tmpEle = fline[i] - '0';
         else tmpEle = tmpEle * 10 + fline[i] - '0';
      }
   }

   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TD
   assert((c < _data.size()) && (c >= 0));
   _data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   size_t x = _sortOrder.size();
   assert(x > 0);
   if (r1[_sortOrder[0]] < r2[_sortOrder[0]]) return true;
   for (size_t i = 1; i < x; i++)
      if ((r1[_sortOrder[i - 1]] == r2[_sortOrder[i - 1]]) && (r1[_sortOrder[i]] < r2[_sortOrder[i]]))
         return true;
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TD
   size_t x = _table.size();
   colMax.clear();
   colMin.clear();
   colSum.clear();
   colSize.clear();
   colCount.clear();
   for (size_t i = 0; i < x; ++i) _table[i].reset();
   _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   assert(d.size() > 0);
   int ma = INT_MAX, mi = INT_MAX, su = 0, si = 0, c;
   vector<int> tmpD = d;
   size_t i = 0, x = tmpD.size();

   for (; i < x; ++i) {
      _table[i].addData(tmpD[i]);
      if (tmpD[i] != INT_MAX) {
         if (tmpD[i] < mi) mi = tmpD[i];
         if ((tmpD[i] > ma) || (ma == INT_MAX)) ma = tmpD[i];
         su += tmpD[i];
         ++si;
      }
   }

   std::sort(tmpD.begin(), tmpD.end());
   if (tmpD[0] == INT_MAX) c = 0;
   else {
      c = 1;
      for (size_t j = 1; ((j < x) && (tmpD[j] != INT_MAX)); ++j) {
         if (tmpD[j] != tmpD[j - 1]) ++c;
      }
   }

   colMax.push_back(ma);
   colMin.push_back(mi);
   colSum.push_back(su);
   colSize.push_back(si);
   colCount.push_back(c);
}

void
DBTable::addRow(const DBRow& r)
{
  size_t x = r.size(), y = _table.size();
  assert((y > 0) && (x == _table[0].size()));
  
  for (size_t i = 0; i < x; ++i) if (r[i] != INT_MAX) {
     if (r[i] < colMin[i]) colMin[i] = r[i];
     if ((r[i] > colMax[i]) || (colMax[i] == INT_MAX)) colMax[i] = r[i];
     colSum[i] += r[i];
     ++colSize[i];
     for (size_t j = 0; j < y; ++j) {
        if (r[i] == _table[j][i]) break;
        if (j == y - 1) ++colCount[i];
     }
  }
  _table.push_back(r);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   assert(c >= 0);
   bool uniq;
   int ma, mi;
   size_t x = _table[c].size(), y = _table.size();
   
   for (size_t i = 0; i < x; ++i) if (_table[c][i] != INT_MAX) {
      --colSize[i];
      colSum[i] -= _table[c][i];
      
      uniq = true;
      mi = ma = INT_MAX;
      for (size_t j = 0; j < y; ++j) {
         if ((_table[j][i] == INT_MAX) || (j == c)) continue;
         if (_table[c][i] == _table[j][i]) { uniq = false; break;}
         
         if (_table[j][i] < mi) mi = _table[j][i];
         if ((_table[j][i] > ma) || (ma == INT_MAX)) ma = _table[j][i];
      }
      if (uniq) {
         --colCount[i];
         if (_table[c][i] == colMax[i]) colMax[i] = ma;
         if (_table[c][i] == colMin[i]) colMin[i] = mi;
      }
   }

   _table[c].reset();
   _table.erase(_table.begin() + c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   assert(c >= 0);
   size_t x = _table.size();
   for (size_t i = 0; i < x; ++i) _table[i].removeCell(c);

   colMax.erase(colMax.begin() + c);
   colMin.erase(colMin.begin() + c);
   colSum.erase(colSum.begin() + c);
   colSize.erase(colSize.begin() + c);
   colCount.erase(colCount.begin() + c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   if (colMax[c] == INT_MAX) return NAN;
   return (float)colMax[c];
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   if (colMin[c] == INT_MAX) return NAN;
   return (float)colMin[c];
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   if (colSize[c] == 0) return NAN;
   return (float)colSum[c];
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   return colCount[c];
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   if (colSize[c] == 0) return NAN;
   return ((float)colSum[c]) / ((float)colSize[c]);
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);
}
void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   size_t x = _table.size();
   if( x > 0) {
      if (_table[0][c] != INT_MAX) cout << _table[0][c];
      else cout << '.';
      for (size_t i = 1; i < x; ++i) {
         cout << ' ';
         if (_table[i][c] != INT_MAX) cout << _table[i][c];
         else cout << '.';
      }
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t j = 0; j < nc; ++j) nv += colSize[j];
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

