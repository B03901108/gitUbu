/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
#include <algorithm>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = NULL, DListNode<T>* n = NULL):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TD: decide the initial value for _isSorted
   DList() {
      _isSorted = true;
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n = NULL): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TD: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         if (_node != NULL) _node = _node->_next;
         return *(this);
      }
      iterator operator ++ (int) {
         if (_node == NULL) return *(this);
         iterator tmpIt(_node);
         _node = _node->_next;
         return tmpIt;
      }
      iterator& operator -- () {
         if (_node != NULL) _node = _node->_prev;
         return *(this);
      }
      iterator operator -- (int) {
         if (_node == NULL) return *(this);
         iterator tmpIt(_node);
         _node = _node->_prev;
         return tmpIt;
      }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TD: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head->_next == _head); }
   size_t size() const {
      size_t _size = 0;
      for (DListNode<T>* i = _head->_next; i != _head; i = i->_next, ++_size) {}
      return _size;
   }

   void push_back(const T& x) {
      DListNode<T>* newNode = new DListNode<T>(T(x));
      newNode->_prev = _head->_prev;
      newNode->_next = _head;
      _head->_prev->_next = newNode;
      _head->_prev = newNode;

      _isSorted = false;
   }
   void pop_front() {
      DListNode<T>* cpHead = _head->_next;
      if (cpHead == _head) return;
      _head->_next = cpHead->_next;
      cpHead->_next->_prev = _head;
      delete cpHead;
   }
   void pop_back() {
      DListNode<T>* cpTail = _head->_prev;
      if (cpTail == _head) return;
      _head->_prev = cpTail->_prev;
      cpTail->_prev->_next = _head;
      delete cpTail;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if (_head->_next == _head)  return false;
      DListNode<T>* delNode = pos._node;
      delNode->_next->_prev = delNode->_prev;
      delNode->_prev->_next = delNode->_next;
      delete delNode;
      return true;
   }
   bool erase(const T& x) {
      for (DListNode<T>* i = _head->_next; i != _head; i = i->_next) {
         if (i->_data == x) {
            i->_next->_prev = i->_prev;
            i->_prev->_next = i->_next;
            delete i;
            return true;
         }
      }
      return false;
   }

   void clear() {
      for (DListNode<T>* i = _head->_next; i != _head;) { i = i->_next; delete i->_prev; }
      _head->_prev = _head->_next = _head;
      _isSorted = true;
   }  // delete all nodes except for the dummy node

   void sort() const {
      if (_isSorted) return;
      size_t arrSize = size(), j = 0;
      if (arrSize < 2) return;
      DListNode<T>** tmpArr = new DListNode<T>* [arrSize];
      for (DListNode<T>* i = _head->_next; i != _head; i = i->_next, ++j) tmpArr[j] = i;

      ::sort(tmpArr, tmpArr + arrSize, cmpr);
      _head->_next = tmpArr[0]; _head->_prev = tmpArr[(--j)];
      tmpArr[0]->_prev = _head; tmpArr[j]->_next = _head;
      for (; j > 0; --j) {
         tmpArr[j]->_prev = tmpArr[j - 1];
         tmpArr[j - 1]->_next = tmpArr[j];
      }

      delete [] tmpArr;
      _isSorted = true;
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TD] helper functions; called by public member functions
   static bool cmpr(DListNode<T>* x, DListNode<T>* y) { return (x->_data < y->_data); }
};

#endif // DLIST_H
