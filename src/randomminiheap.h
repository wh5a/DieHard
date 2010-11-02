// -*- C++ -*-

/**
 * @file   randomminiheap.h
 * @brief  Randomly allocates a particular object size in a range of memory.
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 *
 * Copyright (C) 2006 Emery Berger, University of Massachusetts Amherst
 */

#ifndef _RANDOMMINIHEAP_H_
#define _RANDOMMINIHEAP_H_

#include <assert.h>
#include <stdio.h>

extern "C" void reportDoubleFreeError (void);
extern "C" void reportInvalidFreeError (void);
extern "C" void reportOverflowError (void);


#include "bitmap.h"
#include "check.h"
#include "checkpoweroftwo.h"
#include "diefast.h"
#include "modulo.h"
#include "randomnumbergenerator.h"
#include "sassert.h"

class RandomMiniHeapBase {
public:

  inline virtual void * malloc (size_t) = 0;
  inline virtual bool free (void *) = 0;
  inline virtual size_t getSize (void *) = 0;
  virtual void activate (void) = 0;
  virtual ~RandomMiniHeapBase () {}
};


/**
 * @class RandomMiniHeap
 * @brief Randomly allocates objects of a given size.
 * @param Numerator the heap multiplier numerator.
 * @param Denominator the heap multiplier denominator.
 * @param ObjectSize the object size managed by this heap.
 * @sa    RandomHeap
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 **/
template <int Numerator,
	  int Denominator,
	  size_t ObjectSize,
	  int NObjects,
	  class Allocator,
	  bool DieFastOn>
class RandomMiniHeap : public RandomMiniHeapBase, private Allocator {

  /// Check values for sanity checking.
  enum { CHECK1 = 0xEEDDCCBB, CHECK2 = 0xBADA0101 };

  /// A convenience struct.
  typedef struct {
    char obj[ObjectSize];
  } ObjectStruct;

  friend class Check<RandomMiniHeap *>;


public:

  typedef RandomMiniHeapBase SuperHeap;

  RandomMiniHeap (void)
    : _check1 ((size_t) CHECK1),
      _random (RealRandomValue::value(), RealRandomValue::value()),
      _miniHeap (NULL),
      _inUse (0),
      _freedValue (_random.next() | 1), // Enforce invalid pointer value.
      _check2 ((size_t) CHECK2)
  {
    Check<RandomMiniHeap *> sanity (this);

    /// Some sanity checking.
    CheckPowerOfTwo<ObjectSize>	_SizeIsPowerOfTwo;
    CheckPowerOfTwo<NObjects>	_NObjectsIsPowerOfTwo;
  }

  /// @return an allocated object of size ObjectSize
  /// @param sz   requested object size
  /// @note May return NULL even though there is free space.
  inline void * malloc (size_t sz)
  {
    Check<RandomMiniHeap *> sanity (this);

    // Ensure size is reasonable.
    assert (sz <= ObjectSize);
    assert (isActivated());

    void * ptr = NULL;

    // Try to allocate an object from the bitmap.
    int index = (int) (_random.next() & (NObjects - 1));
    if (!_miniHeapBitmap.tryToSet (index)) {
      return NULL;
    }

    _inUse++;
    
    // Get the address of the indexed object.
    assert (index < NObjects);
    ptr = getObject (index);
    
    if (DieFastOn) {
      // Check to see if this object was overflowed.
      if (DieFast::checkNot (ptr, ObjectSize, _freedValue)) {
	reportOverflowError();
      }
    }

    return ptr;
  }


  /// @return the space remaining from this point in this object
  /// @nb Returns zero if this object is not managed by this heap.
  inline size_t getSize (void * ptr) {
    Check<RandomMiniHeap *> sanity (this);

    if (!inBounds(ptr)) {
      return 0;
    }

    // Compute offset corresponding to the pointer.
    size_t offset = computeOffset (ptr);

    // Return the space remaining in the object from this point.
    return ObjectSize - modulo<ObjectSize>(offset);
  }


  /// @brief Relinquishes ownership of this pointer.
  /// @return true iff the object was on this heap and was freed by this call.
  inline bool free (void * ptr) {
    Check<RandomMiniHeap *> sanity (this);

    // Return false if the pointer is out of range.
    if (!inBounds(ptr)) {
      return false;
    }

    int index = computeIndex (ptr);
    assert ((index >= 0) && (index < NObjects));

    // Reset the appropriate bit in the bitmap.
    if (_miniHeapBitmap.reset (index)) {
      // We actually reset the bit, so this was not a double free.
      _inUse--;
      if (DieFastOn) {
	checkOverflowError (ptr, index);
	// Trash the object.
	DieFast::fill (ptr, ObjectSize, _freedValue);
      }
      return true;
    } else {
      reportDoubleFreeError();
      return false;
    }
  }


  /// @brief Activates the heap, making it ready for allocations.
  NO_INLINE void activate (void) {
    if (_miniHeap == NULL) {
      // Go get memory for the heap and the bitmap, making it ready
      // for allocations.
      _miniHeap = (char *)
	Allocator::malloc (NObjects * ObjectSize);
      if (_miniHeap) {
	_miniHeapBitmap.reserve (NObjects);
	if (DieFastOn) {
	  DieFast::fill (_miniHeap, NObjects * ObjectSize, _freedValue);
	}
      } else {
	assert (0);
      }
    }
  }


private:

  // Disable copying and assignment.
  RandomMiniHeap (const RandomMiniHeap&);
  RandomMiniHeap& operator= (const RandomMiniHeap&);

  /// Sanity check.
  void check (void) const {
    assert ((_check1 == CHECK1) &&
	    (_check2 == CHECK2));
  }

  /// @return the object at the given index.
  inline void * getObject (int index) const {
    assert (index >= 0);
    assert (index < NObjects);
    assert (_miniHeap != NULL);
    return (void *) &((ObjectStruct *) _miniHeap)[index];
  }

  /// @return the index corresponding to the given object.
  inline int computeIndex (void * ptr) const {
    assert (inBounds(ptr));
    size_t offset = computeOffset (ptr);
    return (int) (offset / ObjectSize);
  }

  /// @return the distance of the object from the start of the heap.
  inline size_t computeOffset (void * ptr) const {
    assert (inBounds(ptr));
    size_t offset = ((size_t) ptr - (size_t) _miniHeap);
    return offset;
  }


  /// @brief Checks to see if the predecessor and successor have been overflowed.
  void checkOverflowError (void * ptr, int index) const
  {
    if ((index > 0) && (!_miniHeapBitmap.isSet (index - 1))) {
      void * p = (void *) (((ObjectStruct *) ptr) - 1);
      if (DieFast::checkNot (p, ObjectSize, _freedValue)) {
	reportOverflowError();
      }
    }
    if ((index < (NObjects - 1))  && (!_miniHeapBitmap.isSet (index + 1))) {
      void * p = (void *) (((ObjectStruct *) ptr) + 1);
      if (DieFast::checkNot (p, ObjectSize, _freedValue)) {
	reportOverflowError();
      }
    }
  }

  /// @return true iff the index is invalid for this heap.
  inline bool inBounds (void * ptr) const {
    if ((ptr < _miniHeap) || (ptr >= _miniHeap + NObjects * ObjectSize)
	|| (_miniHeap == NULL)) {
      return false;
    }
    return true;
  }

  /// @return true iff heap is currently active.
  inline bool isActivated (void) const {
    return (_miniHeap != NULL);
  }

  /// Sanity check value.
  const size_t _check1;

  /// A random value used to overwrite freed space for debugging (with DieFast).
  const size_t _freedValue;

  /// The bitmap for this heap.
  BitMap<Allocator> _miniHeapBitmap;

  /// A local random number generator.
  RandomNumberGenerator _random;

  /// The heap pointer.
  char * _miniHeap;

  /// How many objects are in use.
  size_t _inUse;

  /// Sanity check value.
  const size_t _check2;

};


#endif

