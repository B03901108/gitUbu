/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      size_t index= _data.size(), tmpId;
      _data.push_back(d);
      while (index > 0) {
      	 tmpId = (index - 1) / 2;
         if (d < _data[tmpId]) {
            _data[index] = _data[tmpId];
            _data[tmpId] = d;
            index = tmpId;
         }
         else break;
      }
   }
   void delMin() { delData(0); }
   void delData(size_t i) {
      size_t newSize = (_data.size() - 1), tmpId;
      swap(i, newSize);
      _data.pop_back();
      while (i > 0) {
      	 tmpId = (i - 1) / 2;
         if (_data[i] < _data[tmpId]) { swap(i, tmpId); i = tmpId; }
         else break;
      }
      while (2 * i + 1 < newSize) {
      	 tmpId = (i + 1) * 2;
      	 if ((tmpId == newSize) || (_data[tmpId - 1] < _data[tmpId])) --tmpId;
         if (_data[tmpId] < _data[i]) { swap(i, tmpId); i = tmpId; }
         else break;
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
   bool swap(size_t x, size_t y) {
   	  if (x == y) return false;
      Data tmpData = _data[x];
      _data[x] = _data[y];
      _data[y] = tmpData;
      return true;
   }
};

#endif // MY_MIN_HEAP_H
