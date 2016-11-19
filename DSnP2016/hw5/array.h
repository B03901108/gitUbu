/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // _TD_: decide the initial value for _isSorted
   Array() : _data(NULL), _size(0), _capacity(0) {}
   ~Array() { if (_data != NULL) delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n = NULL): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // _TD_: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) {
         iterator tmpIt(_node);
         ++_node;
         return (tmpIt);
      }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) {
         iterator tmpIt(_node);
         --_node;
         return (tmpIt);
      }

      iterator operator + (int i) const {
         iterator tmpIt(_node);
         tmpIt._node += i;
         return (tmpIt);
      }
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      T*    _node;
   };

   // _TD_: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + _size); }
   bool empty() const { return (_size == 0); }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if (_data == NULL) {
         _data = new T [1];
         _capacity = 1;
      }
      else if (_size == _capacity) {
         _capacity *= 2;
         T* tmpData = new T [_capacity];
         for (size_t i = 0; i < _size; ++i) tmpData[i] = _data[i];
         delete [] _data;
         _data = tmpData;
      }
      _data[_size] = x;
      ++_size;
   }
   void pop_front() { *_data = _data[(--_size)]; }
   void pop_back() { --_size; }

   bool erase(iterator pos) {
      if (_size == 0) return false;
      if (*pos != _data[(--_size)]) *pos = _data[_size];
      return true;
   }
   bool erase(const T& x) {
      if (_size == 0) return false;
      T* y = _data + (_size - 1);
      for (T* i = _data; i != y; ++i) {
         if (*i == x) {
            *i = *y;
            --_size;
            return true;
         }
      }
      if (*y == x) { --_size; return true; }
      return false;
   }

   void clear() { _size = 0; }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data + _size); }

   // Nice to have, but not required in this homework...
   void reserve(size_t n) {
      if (n > _capacity) {
         _capacity = n;
         T* tmpData = new T [_capacity];
         for (size_t i = 0; i < _size; ++i) tmpData[i] = _data[i];
         delete [] _data;
         _data = tmpData;
      }
   }
   void resize(size_t n) {
      while (n > _size) push_back(T());
      _size = n;
   }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL _TD_] Helper functions; called by public member functions
};

#endif // ARRAY_H
