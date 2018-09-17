/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef OMPENMP_COMPAT
#define OMPENMP_COMPAT

#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_max_threads() 1
#endif

#endif