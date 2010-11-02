// -*- C++ -*-

/**
 * @file   bumpalloc.h
 * @brief  Obtains memory in chunks, allocates by pointer bumping.
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 * @note   Copyright (C) 2005 by Emery Berger, University of Massachusetts Amherst.
 *
 **/


#ifndef _BUMPALLOC_H_
#define _BUMPALLOC_H_

/**
 * @class BumpAlloc
 * @brief Obtains memory in chunks and bumps a pointer through the chunks.
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 */

template <class Super,
          int ChunkSize = 65536>
class BumpAlloc : public Super {
public:

  BumpAlloc (void)
    : _bump (0),
      _remaining (0)
  {}

  inline void * malloc (size_t sz) {
    // Enforce double-word alignment.
    sz = (sz + sizeof(double) - 1) & ~(sizeof(double) - 1);
    // If there's not enough space left to fulfill this request, get
    // enough chunks to do.
    if (_remaining < sz) {
      refill(sz);
    }
    char * old = _bump;
    _bump += sz;
    _remaining -= sz;
    return old;
  }

  /// Free is disabled (we only bump, never reclaim).
  inline bool free (void *) { return false; }

private:

  /// The bump pointer.
  char * _bump;

  /// How much space remains in the current chunk.
  size_t _remaining;

  // Get another chunk.
  void refill (size_t sz) {
    if (sz < ChunkSize) {
      sz = ChunkSize;
    }
    // Round up the size request to a multiple of the ChunkSize.
    sz = ChunkSize * ((sz + ChunkSize - 1) / ChunkSize);
    _bump = (char *) Super::malloc (sz);
    _remaining = sz;
  }

};

#endif
