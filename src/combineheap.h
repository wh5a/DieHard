// -*- C++ -*-

#ifndef _COMBINEHEAP_H_
#define _COMBINEHEAP_H_

template <class SmallHeap,
	  class BigHeap>
class CombineHeap {
public:

  inline void * malloc (size_t sz) {
    void * ptr;
    if (sz > SmallHeap::MAX_SIZE) {
      ptr = _big.malloc (sz);
    } else {
      ptr = _small.malloc (sz);
    }
    return ptr;
  }

  inline bool free (void * ptr) {
    if (_small.free (ptr)) {
      return true;
    } else {
      return _big.free (ptr);
    }
  }

  inline size_t getSize (void * ptr) {
    size_t sz = _small.getSize (ptr);
    if (sz == 0) {
      sz = _big.getSize (ptr);
    }
    return sz;
  }

private:
  SmallHeap _small;
  BigHeap _big;
};

#endif
