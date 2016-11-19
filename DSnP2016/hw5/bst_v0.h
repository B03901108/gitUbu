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
#include <stack>
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

   BSTreeNode(const T& d, bool isRed, BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, BSTreeNode<T>* r = NULL):
      _data(d), _parent(p), _left(l), _right(r) { if (isRed) _parent = (BSTreeNode<T>*)(((size_t)_parent) + 1); }
   BSTreeNode<T>* parent() { return (BSTreeNode<T>*)((((size_t)_parent) >> 1) << 1); }
   void encrypt() { _parent = (BSTreeNode<T>*)(((size_t)_parent) + 1); }
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
   BSTree() {
      // _nil is a dummy node
      _size = 0;
      _nil = new BSTreeNode<T>(T(), false);
      _nil->_left = _nil->_right = _nil;
      // _nil's parent = NULL, while both of its children are the root (or tiself if empty())
   }
   ~BSTree() { clear(); delete _nil; }

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
         if (!_node->_parent) return *(this);
         if (_node->_right->_parent) { _node = _node->_right; return (*this); }
         
         while (_node == _node->parent()->_right) {
            _node = _node->parent();
            if (!_node->_parent) return *(this);
         }
         _node = _node->parent();
         return *(this);
      }
      iterator operator ++ (int) {
         iterator tmpIt(_node);
         ++(*this);
         return tmpIt;
      }
      iterator& operator -- () {
         if (!_node->_parent) {
            do { _node = _node->_right; } while (_node->_right->_parent);
            return *(this);
         }
         if (_node->_left->_parent) { _node = _node->_left; return (*this); }
         
         BSTreeNode<T>* cpNode = _node;
         while (_node == _node->parent()->_left) {
            _node = _node->parent();
            if (!_node->_parent) { _node = cpNode; return *(this); }
         }
         _node = _node->parent();
         return *(this);
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
   };

   // TODO: implement these functions
   void print() { }
   iterator begin() const { return iterator(NULL); }
   iterator end() const { return iterator(_nil); }
   bool empty() const { return (_size == 0); }
   size_t size() const { return _size; }

   void insert(const T& x) { }
   void pop_front() { }
   void pop_back() { }

   // return false if nothing to erase
   bool erase(iterator pos) { return false; }
   bool erase(const T& x) { return false; }

   void clear() {
      if (_size == 0) return;
      reset(_nil->_right);
      _nil->_right = _nil->_left = _nil;
   }  // delete all nodes except for the dummy node

   void sort() const { }

private:
   BSTreeNode<T>*  _nil;     // = dummy node if list is empty
   size_t _size;
   // [OPTIONAL TODO] helper functions; called by public member functions
   void reset(BSTreeNode<T>* B) {
      if (B->_left != _nil) reset(B->_left);
      if (B->_right != _nil) reset(B->_right);
      delete B;
      --_size;
   }
   void rotate(BSTreeNode<T>* B, bool isLeft) {
   	  bool marked = false;
      BSTreeNode<T>* tmpNode = ((isLeft) ? (B->_right) :(B->_left));
      if (isLeft) {
         B->_right = tmpNode->_left;
         if (tmpNode->_left != _nil) {
            if (tmpNode->_left->_parent != tmpNode) marked = true;
            tmpNode->_left->_parent = B;
            if (marked) { tmpNode->_left->encrypt(); marked = false; }
         }
      }
      else {
         B->_left = tmpNode->_right;
         if (tmpNode->_right != _nil) {
            if (tmpNode->_right->_parent != tmpNode) marked = true;
            tmpNode->_right->_parent = B;
            if (marked) { tmpNode->_right->encrypt(); marked = false; }
         }
      }

      if (tmpNode->_parent != tmpNode->parent()) marked = true;
      tmpNode->_parent = B->parent();
      if (marked) { tmpNode->encrypt(); marked = false; }
      if (B == B->parent()->_left) B->parent()->_left = tmpNode;
      if (B == B->parent()->_right) B->parent()->_right = tmpNode;

      if (isLeft) tmpNode->_left = B;
      else tmpNode->_right = B;
      if (B->_parent != B->parent()) marked = true;
      B->_parent = tmpNode;
      if (marked) B->encrypt();
   }
};

#endif // BST_H
