// -*- C++ -*- 

#ifndef _MWC_H_
#define _MWC_H_

/**
 * @class MWC
 * @brief A super-fast multiply-with-carry pseudo-random number generator.
 *
 */

class MWC {
public:

  MWC (unsigned long seed1, unsigned long seed2)
    : z (seed1), w (seed2)
  {}

  inline unsigned long next (void) {
    // These magic numbers are derived from a note by George Marsaglia.
    unsigned long znew = (z=36969*(z&65535)+(z>>16));
    unsigned long wnew = (w=18000*(w&65535)+(w>>16));
    unsigned long x = (znew << 16) + wnew;
    return x;
  }
  
private:

  unsigned long z;
  unsigned long w;
  
};

#endif
