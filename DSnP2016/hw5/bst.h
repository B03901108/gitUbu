/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TD: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, BSTreeNode<T>* r = NULL):
      _data(d), _parent(p), _left(l), _right(r) { }
   
   T _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
public:
   // TD: decide the initial value for _isSorted
   BSTree(): _entry(NULL), _size(0), _balance(true) { }
   ~BSTree() { clear(); }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n = NULL): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TD: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         if (isEnd()) return (*this);
         if (_node->_right) {
            _node = _node->_right;
            while (_node->_left) _node = _node->_left;
            return (*this);
         }
         
         BSTreeNode<T>* cpNode = _node;
         while (_node->_parent) {
            if (_node == _node->_parent->_right) _node = _node->_parent;
            else { _node = _node->_parent; return (*this); }
         }
         _node = cpNode;
         return toBeEnd();
      }
      iterator operator ++ (int) {
         iterator tmpIt(_node);
         ++(*this);
         return tmpIt;
      }
      iterator& operator -- () {
         if (isEnd(true)) return (*this);
         if (_node->_left) {
            _node = _node->_left;
            while (_node->_right) _node = _node->_right;
            return (*this);
         }
         
         BSTreeNode<T>* cpNode = _node;
         while (_node->_parent) {
            if (_node == _node->_parent->_left) _node = _node->_parent;
            else { _node = _node->_parent; return (*this); }
         }
         _node = cpNode;
         return (*this);
      }
      iterator operator -- (int) {
         iterator tmpIt(_node);
         --(*this);
         return tmpIt;
      }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      BSTreeNode<T>* _node;
      iterator& toBeEnd() { _node = (BSTreeNode<T>*)(((size_t)_node) + 1); return (*this); }
      bool isEnd(bool update = false) {
      	 size_t N = (size_t)_node;
         size_t legalN = (((size_t)_node) >> 1) << 1;
         if (update) _node = (BSTreeNode<T>*)legalN;
         return (N != legalN);
      }
   };

   // TD: implement these functions
   void print() { if (_entry) show(_entry, 0); }
   iterator begin() const {
   	  if (_size == 0) return iterator(NULL).toBeEnd();
      BSTreeNode<T>* least = _entry;
      while (least->_left) least = least->_left;
      return iterator(least);
   }
   iterator end() const {
      if (_size == 0) return iterator(NULL).toBeEnd();
      BSTreeNode<T>* most = _entry;
      while (most->_right) most = most->_right;
      return iterator(most).toBeEnd();
   }
   bool empty() const { return (_size == 0); }
   size_t size() const { return _size; }

   void insert(const T& x) {
      ++_size;
      BSTreeNode<T>* newNode = new BSTreeNode<T>(x);
      if (!_entry) { _entry = newNode; return; }
      newNode->_parent = _entry;
      while (true) {
         if (newNode->_data < newNode->_parent->_data) {
            if (newNode->_parent->_left) newNode->_parent = newNode->_parent->_left;
            else { newNode->_parent->_left = newNode; return; }
         }
         else if (newNode->_parent->_right) newNode->_parent = newNode->_parent->_right;
         else { newNode->_parent->_right = newNode; return; }
      }
   }
   void pop_front() {
      if (_size == 0) return;
      BSTreeNode<T>* least = _entry;
      BSTreeNode<T>* p;
      BSTreeNode<T>* r;
      while (least->_left) least = least->_left;
      p = least->_parent;
      r = least->_right;

      if (r) r->_parent = p;
      if (p) p->_left = r;
      else _entry = r;
      
      delete least;
      --_size;
   }
   void pop_back() {
      if (_size == 0) return;
      BSTreeNode<T>* most = _entry;
      BSTreeNode<T>* p;
      BSTreeNode<T>* l;
      while (most->_right) most = most->_right;
      p = most->_parent;
      l = most->_left;

      if (l) l->_parent = p;
      if (p) p->_right = l;
      else _entry = l;
      
      delete most;
      --_size;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if (_size == 0) return false;

      BSTreeNode<T>* delNode = pos._node;
      BSTreeNode<T>* p = delNode->_parent;
      BSTreeNode<T>* r = delNode->_right;
      bool isLeft = ((p) && (delNode == p->_left));

      if ((r) && (delNode->_left)) {
         _balance = (!_balance);
         if (_balance) {
            delNode = delNode->_left;
            isLeft = true;
            while (delNode->_right) { isLeft = false; delNode = delNode->_right; }
         }
         else {
            delNode = r;
            isLeft = false;
            while (delNode->_left)  { isLeft = true; delNode = delNode->_left; }
         }
         pos._node->_data = delNode->_data;
         p = delNode->_parent;
         r = delNode->_right;
      }

      if (delNode->_left) r = delNode->_left;
      if (r) { r->_parent = p; }
      if (!p) _entry = r;
      else if (isLeft) p->_left = r;
      else p->_right = r;
      
      delete delNode;
      --_size;
      return true;
   }
   bool erase(const T& x) {
      BSTreeNode<T>* nowNode = _entry;
      while (nowNode) {
         if (x == nowNode->_data) { erase(iterator(nowNode)); return true; }
         if (x < nowNode->_data) nowNode = nowNode->_left;
         else nowNode = nowNode->_right;
      }
      return false;
   }

   void clear() {
      if (_size == 0) return;
      reset(_entry);
      _entry = NULL;
      _balance = true;
   }  // delete all nodes except for the dummy node

   void sort() const { }

private:
   BSTreeNode<T>*  _entry;     // = dummy node if list is empty
   size_t _size;
   bool _balance;
   // [OPTIONAL TD] helper functions; called by public member functions
   void reset(BSTreeNode<T>* B) {
      if (B->_left) reset(B->_left);
      if (B->_right) reset(B->_right);
      delete B;
      --_size;
   }
   void show(BSTreeNode<T>* B, size_t space) {
      cout << B->_data << endl;
      if (B->_left) {
         for (size_t i = 0; i < space; ++i) cout << ' ';
         cout << "LEFT:";
         show(B->_left, space + 1);
      }
      if (B->_right) {
         for (size_t i = 0; i < space; ++i) cout << ' ';
         cout << "RIGHT:";
         show(B->_right, space + 1);
      }
   }
};


#endif // BST_H
