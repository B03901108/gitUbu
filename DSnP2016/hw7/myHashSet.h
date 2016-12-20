/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // _TD_: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
   public:
      iterator(vector<Data>* V, size_t c, vector<Data>* U, vector<Data>* L) {
         nowVec = V;
         upb = U;
         lwb = L;
         cursor = c;
      }
      iterator(const iterator& i) :nowVec(i.nowVec), cursor(i.cursor), upb(i.upb), lwb(i.lwb) {}
      ~iterator() {}

      const Data& operator * () const { return (*nowVec)[cursor]; }
      Data& operator * () { return (*nowVec)[cursor]; }
      iterator& operator ++ () {
         if ((++cursor) < nowVec->size()) return *this;
         cursor = 0;
         do { ++nowVec; } while ((nowVec != upb) && (nowVec->size() == 0));
         return *this;
      }
      iterator operator ++ (int) {
         iterator tmpIt(nowVec, cursor, upb, lwb);
         if ((++cursor) >= nowVec->size()) {
            cursor = 0;
            do { ++nowVec; } while ((nowVec != upb) && (nowVec->size() == 0));
         }
         return tmpIt;
      }
      iterator& operator -- () {
         if (cursor != 0) { --cursor; return *this; }
         while (nowVec != lwb) {
            cursor = (--nowVec)->size();
            if (cursor != 0) { --cursor; break; }
         }
         return *this;
      }
      iterator operator -- (int) {
         iterator tmpIt(nowVec, cursor, upb, lwb);
         if (cursor != 0) --cursor;
         else while (nowVec != lwb) {
            cursor = (--nowVec)->size();
            if (cursor != 0) { --cursor; break; }
         }
         return tmpIt;
      }

      iterator& operator = (const iterator& i) {
         nowVec = i.nowVec;
         cursor = i.cursor;
         upb = i.upb;
         lwb = i.lwb;
         return *this;
      }

      bool operator != (const iterator& i) const {
         if (cursor != i.cursor) return true;
         if (nowVec != i.nowVec) return true;
         return false;
      }
      bool operator == (const iterator& i) const {
         if (cursor != i.cursor) return false;
         if (nowVec != i.nowVec) return false;
         return true;
      }

   private:
      vector<Data>* nowVec;
      vector<Data>* upb;
      vector<Data>* lwb;
      size_t cursor;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // _TD_: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      vector<Data>* V = _buckets;
      vector<Data>* W = _buckets + (_numBuckets - 1);
      for (size_t i = _numBuckets; i > 0; --i, --W) if (W->size() != 0) break;
      if ((++W) != V) for (size_t i = _numBuckets; ((i > 0) && (V->size() == 0)); --i, ++V) {}
      return iterator(V, 0, W, V);
   }
   // Pass the end
   iterator end() const {
      vector<Data>* V = _buckets;
      vector<Data>* W = _buckets + (_numBuckets - 1);
      for (size_t i = _numBuckets; i > 0; --i, --W) if (W->size() != 0) break;
      if ((++W) != V) for (size_t i = _numBuckets; ((i > 0) && (V->size() == 0)); --i, ++V) {}
      return iterator(W, 0, W, V);
   }
   // return true if no valid data
   bool empty() const {
      for (size_t i = 0 ; i < _numBuckets; ++i) if (_buckets[i]->size()) return false;
      return true;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for (size_t i = 0 ; i < _numBuckets; ++i) s += _buckets[i]->size();
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      vector<Data>* nowVec = _buckets + bucketNum(d);
      for (size_t i = 0, x = nowVec->size(); i < x; ++i) if ((*nowVec)[i] == d) return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      vector<Data>* nowVec = _buckets + bucketNum(d);
      for (size_t i = 0, x = nowVec->size(); i < x; ++i)
         if ((*nowVec)[i] == d) { d = (*nowVec)[i]; return true; }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      vector<Data>* nowVec = _buckets + bucketNum(d);
      for (size_t i = 0, x = nowVec->size(); i < x; ++i)
         if ((*nowVec)[i] == d) { (*nowVec)[i] = d; return true; }
      nowVec->push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      vector<Data>* nowVec = _buckets + bucketNum(d);
      for (size_t i = 0, x = nowVec->size(); i < x; ++i) if ((*nowVec)[i] == d) return false;
      nowVec->push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      vector<Data>* nowVec = _buckets + bucketNum(d);
      for (size_t i = 0, x = nowVec->size(); i < x; ++i) if ((*nowVec)[i] == d) {
         if (i != x - 1) (*nowVec)[i] = (*nowVec)[x - 1];
         nowVec->pop_back();
         return true;
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
