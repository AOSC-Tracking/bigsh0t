#ifdef USE_SSE
    #if defined(__GNUC__) || defined (__clang__)
        #include <x86intrin.h>
    #elif defined(_MSC_VER)
        #include <emmintrin.h>
        #include <mmintrin.h>
        #include <intrin.h>
    #else
        #error Unable to determine compiler and SSE headers to include.
    #endif
#endif
