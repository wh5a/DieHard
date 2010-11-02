// -*- C++ -*-

/**
 * @file   mmapwrapper.h
 * @brief  Provides platform-independent interface to memory map functions.
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 * @note   Copyright (C) 2005 by Emery Berger, University of Massachusetts Amherst.
 */


#ifndef _MMAPWRAPPER_H_
#define _MMAPWRAPPER_H_

#if defined(_WIN32)
#include <windows.h>
#else
// UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#endif


#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

#if 0 // executable heap
#define MMAP_PROTECTION_MASK (PROT_READ | PROT_WRITE | PROT_EXEC)
#else
#define MMAP_PROTECTION_MASK (PROT_READ | PROT_WRITE)
#endif

#if !defined(_WIN32)
#if defined(_SVR4)
extern "C" int madvise (caddr_t, size_t, int);
#endif
#endif


class MmapWrapper {
public:

#if defined(_WIN32) 

#define MMAP_PROTECTION PAGE_READWRITE

  // Microsoft Windows has 4K pages aligned to a 64K boundary.
  enum { Size = 4 * 1024 };
  enum { Alignment = 64 * 1024 };

  static void * map (size_t sz) {
    void * ptr;
    ptr = VirtualAlloc (NULL, sz, MEM_RESERVE | MEM_COMMIT, MMAP_PROTECTION);
    return  ptr;
  }
  
  static bool unmap (void * ptr, size_t) {
    size_t sz = getSize (ptr);
    if (sz) {
      return (VirtualFree (ptr, 0, MEM_RELEASE) != 0);
    } else {
      return false;
    }
  }

  static void dontneed (void * ptr, size_t sz) {
    VirtualFree (ptr, sz, MEM_DECOMMIT);
    VirtualAlloc (ptr, sz, MEM_COMMIT, MMAP_PROTECTION);
  }

  static void protect (void * ptr, size_t sz) {
    DWORD oldProtection;
    VirtualProtect (ptr, sz, PAGE_NOACCESS, &oldProtection);
  }

  static void unprotect (void * ptr, size_t sz) {
    DWORD oldProtection;
    VirtualProtect (ptr, sz, MMAP_PROTECTION, &oldProtection);
  }

  static size_t getSize (void * ptr) {
    MEMORY_BASIC_INFORMATION buff;
    if (VirtualQuery (ptr, &buff, sizeof(MEMORY_BASIC_INFORMATION)) != 0) {
      return buff.RegionSize;
    } else {
      return 0;
    }
  }

#else

#if defined(__SVR4)
  // Solaris aligns 8K pages to a 64K boundary.
  enum { Size = 8 * 1024 };
  enum { Alignment = 64 * 1024 };
#else
  // Linux and most other operating systems align memory to a 4K boundary.
  enum { Size = 4 * 1024 };
  enum { Alignment = 4 * 1024 };
#endif

  static void * map (size_t sz) {

    if (sz == 0) {
      return NULL;
    }

    void * ptr;

#if defined(MAP_ALIGN) && defined(MAP_ANON)
    // Request memory aligned to the Alignment value above.
    ptr = mmap ((char *) Alignment, sz, MMAP_PROTECTION_MASK, MAP_PRIVATE | MAP_ALIGN | MAP_ANON, -1, 0);
#elif !defined(MAP_ANONYMOUS)
    static int fd = ::open ("/dev/zero", O_RDWR);
    ptr = mmap (NULL, sz, MMAP_PROTECTION_MASK, MAP_PRIVATE, fd, 0);
#else
    ptr = mmap (NULL, sz, MMAP_PROTECTION_MASK, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

    if (ptr == MAP_FAILED) {
      return NULL;
    } else {
      return ptr;
    }
  }

  static void dontneed (void * ptr, size_t sz) {
    madvise ((caddr_t) ptr, sz, MADV_DONTNEED);
  }

  static bool unmap (void * ptr, size_t sz) {
    if (munmap (reinterpret_cast<char *>(ptr), sz) == 0) {
      return true;
    } else {
      return false;
    }
  }

  static void protect (void * ptr, size_t sz) {
    mprotect ((char *) ptr, sz, PROT_NONE);
  }

  static void unprotect (void * ptr, size_t sz) {
    mprotect ((char *) ptr, sz, MMAP_PROTECTION_MASK);
  }

   
#endif

};

#endif
