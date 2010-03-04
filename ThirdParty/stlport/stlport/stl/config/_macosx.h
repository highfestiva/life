#define _STLP_PLATFORM "Mac OS X"

#if defined (__BIG_ENDIAN__)
#  define _STLP_BIG_ENDIAN 1
#elif defined (__LITTLE_ENDIAN__)
#  define _STLP_LITTLE_ENDIAN 1
#endif

#define _STLP_USE_UNIX_IO

#if !defined(_STLP_NO_THREADS) && !defined(_REENTRANT)
#  define _REENTRANT
#endif
#if defined(_REENTRANT) && !defined(_STLP_THREADS)
#  define _STLP_THREADS
#  define _STLP_PTHREADS
#endif
