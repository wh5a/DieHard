#ifndef _LARGEHEAP_H_
#define _LARGEHEAP_H_

#include <assert.h>

#include "checkpoweroftwo.h"
#include "staticlog.h"
#include "mmapwrapper.h"


class LargeHeap {
public:

  void * malloc (size_t sz) {
    void * ptr = MmapWrapper::map (sz);
    set (ptr, sz);
    return ptr;
  }

  bool free (void * ptr) {
    // If we allocated this object, free it.
    size_t sz = get(ptr);
    if (sz > 0) {
      MmapWrapper::unmap (ptr, sz);
      clear (ptr);
      return true;
    } else {
      return false;
    }
  }

  size_t getSize (void * ptr) {
    size_t s = get(ptr);
    if (!s) {
      return 0;
    } else {
      size_t offset = (size_t) ptr - (getIndex(ptr) << POINTER_SHIFT);
      return s - offset;
    }
  }

private:

  inline int getIndex (void * ptr) const {
    return (size_t) ptr >> POINTER_SHIFT;
  }
  
  inline size_t get (void * ptr) const {
    int index = getIndex(ptr);
    assert (index >= 0);
    assert (index < SIZE_ENTRIES);
    return _size[index];
  }
  
  inline void set (void * ptr, size_t sz) {
    // Initialize a range with the actual size.
    int index = getIndex(ptr);
    size_t currSize = sz;
    int iterations = (sz + PAGE_SIZE - 1) / PAGE_SIZE;
    for (int i = 0; i < iterations; i++) {
      assert (index + i < SIZE_ENTRIES);
      _size[index + i] = currSize;
      currSize -= PAGE_SIZE;
    }
  }
  
  inline void clear (void * ptr) {
    int index = getIndex(ptr);
    size_t sz = _size[index];
    int iterations = (sz + PAGE_SIZE - 1) / PAGE_SIZE;
    for (int i = 0; i < iterations; i++) {
      assert (index + i < SIZE_ENTRIES);
      _size[index + i] = 0;
    }
  }

  enum { PAGE_SIZE = 4096 };
  enum { POINTER_SHIFT = StaticLog<PAGE_SIZE>::VALUE };
  enum { SIZE_ENTRIES = 1 << (32 - POINTER_SHIFT) };

  size_t _size[SIZE_ENTRIES];

};


#endif
