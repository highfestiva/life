#define _STLP_PLATFORM "Mac OS X"

#ifndef _STLP_USE_STDIO_IO
#  define _STLP_USE_UNIX_IO
#endif

/* #define _STLP_USE_STDIO_IO */

/* If not explicitly specified otherwise, work with threads
 */
#if !defined(_STLP_NO_THREADS) && !defined(_REENTRANT)
#  define _REENTRANT
#endif

#if defined(_REENTRANT) && !defined(_PTHREADS)
# define _PTHREADS
#endif

#ifdef __UCLIBC__ /* uClibc 0.9.27 */
#  define _STLP_USE_UCLIBC 1
#  if !defined(__UCLIBC_HAS_WCHAR__)
#    ifndef _STLP_NO_WCHAR_T
#      define _STLP_NO_WCHAR_T
#    endif
#    ifndef _STLP_NO_NATIVE_MBSTATE_T
#      define _STLP_NO_NATIVE_MBSTATE_T
#    endif
#    ifndef _STLP_NO_NATIVE_WIDE_STREAMS
#      define _STLP_NO_NATIVE_WIDE_STREAMS
#    endif
#  endif /* __UCLIBC_HAS_WCHAR__ */
   /* Hmm, bogus _GLIBCPP_USE_NAMESPACES seems undefined... */
#  define _STLP_VENDOR_GLOBAL_CSTD 1
#endif


#if defined(_PTHREADS)
#  if !defined(_STLP_THREADS)
#    define _STLP_THREADS
#  endif
#  define _STLP_PTHREADS
/*
#  ifndef __USE_UNIX98
#    define __USE_UNIX98
#  endif
*/
/* This feature exist at least since glibc 2.2.4 */
/* #  define __FIT_XSI_THR */ /* Unix 98 or X/Open System Interfaces Extention */
#  ifdef __USE_XOPEN2K
/* The IEEE Std. 1003.1j-2000 introduces functions to implement spinlocks. */
#   ifndef __UCLIBC__ /* There are no spinlocks in uClibc 0.9.27 */
#     define _STLP_USE_PTHREAD_SPINLOCK
#   else
#     ifndef _STLP_DONT_USE_PTHREAD_SPINLOCK
        /* in uClibc (0.9.26) pthread_spinlock* declared in headers
         * but absent in library */
#       define _STLP_DONT_USE_PTHREAD_SPINLOCK
#     endif
#   endif
#   ifndef _STLP_DONT_USE_PTHREAD_SPINLOCK
#     define _STLP_USE_PTHREAD_SPINLOCK
#     define _STLP_STATIC_MUTEX _STLP_mutex
#   endif
/* #   define __FIT_PSHARED_MUTEX */
#  endif
#endif

/* Endiannes */
#if defined (__BIG_ENDIAN__)
#  define _STLP_BIG_ENDIAN 1
#elif defined (__LITTLE_ENDIAN__)
#  define _STLP_LITTLE_ENDIAN 1
#endif

#if defined(__GNUC__) && (__GNUC__ < 3)
#  define _STLP_NO_NATIVE_WIDE_FUNCTIONS 1
#endif

#ifdef __GLIBC__
#  if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 3) || (__GLIBC__ > 2)
/* From glibc 2.3.x default allocator is malloc_alloc, if was not defined other */
#    if !defined(_STLP_USE_MALLOC) && !defined(_STLP_USE_NEWALLOC) && !defined(_STLP_USE_PERTHREAD_ALLOC) && !defined(_STLP_USE_NODE_ALLOC)
#      define _STLP_USE_MALLOC 1
#    endif
#  endif
/* Following platforms has no long double:
 *   - Alpha
 *   - PowerPC
 *   - SPARC, 32-bits (64-bits platform has long double)
 *   - MIPS, 32-bits
 *   - ARM
 *   - SH4
 */
#  if defined(__alpha__) || \
      defined(__ppc__) || defined(PPC) || defined(__powerpc__) || \
      ((defined(__sparc) || defined(__sparcv9) || defined(__sparcv8plus)) && !defined ( __WORD64 ) && !defined(__arch64__)) /* ? */ || \
      (defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32)) || \
      defined(__arm__) || \
      defined(__sh__)
 /* #  if defined(__NO_LONG_DOUBLE_MATH) */
#    define _STLP_NO_LONG_DOUBLE
#  endif
#endif
