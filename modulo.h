#ifndef _MODULO_H_
#define _MODULO_H_

#include <stdlib.h>
#include "checkpoweroftwo.h"

// Use a fast modulus function if possible.

template <int B>
inline size_t modulo (size_t v) {
  if (IsPowerOfTwo<B>::VALUE) {
    return (v & (B-1));
  } else {
    return (v % B);
  }
}

#endif
