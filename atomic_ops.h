#ifndef __ATOMIC_OPS_H
#define __ATOMIC_OPS_H
#pragma once
#ifdef __GNUC__
// Atomic functions in GCC are present from version 4.1.0 on
// http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html

// Test for GCC >= 4.1.0
#if (__GNUC__ < 4) || \
    ((__GNUC__ == 4) && ((__GNUC_MINOR__ < 1) || \
                        ((__GNUC_MINOR__     == 1) && \
                         (__GNUC_PATCHLEVEL__ < 0))) )
                        
#error Atomic built-in functions are only available in GCC in versions >= 4.1.0
#endif // end of check for GCC 4.1.0

/// @brief atomically adds a_count to the variable pointed by a_ptr
/// @return the value that had previously been in memory
#define AtomicAdd(a_ptr,a_count) __sync_fetch_and_add (a_ptr, a_count)

/// @brief atomically substracts a_count from the variable pointed by a_ptr
/// @return the value that had previously been in memory
#define AtomicSub(a_ptr,a_count) __sync_fetch_and_sub (a_ptr, a_count)

/// @brief Compare And Swap
///        If the current value of *a_ptr is a_oldVal, then write a_newVal into *a_ptr
/// @return true if the comparison is successful and a_newVal was written
#define CAS(a_ptr, a_oldVal, a_newVal) __sync_bool_compare_and_swap(a_ptr, a_oldVal, a_newVal)

/// @brief Compare And Swap
///        If the current value of *a_ptr is a_oldVal, then write a_newVal into *a_ptr
/// @return the contents of *a_ptr before the operation
#define CASVal(a_ptr, a_oldVal, a_newVal) __sync_val_compare_and_swap(a_ptr, a_oldVal, a_newVal)

#else
#include <Windows.h>
#define AtomicAdd(a_ptr, a_count) InterlockedExchangeAdd((LONG*)a_ptr, a_count)

#define AtomicAdd64(a_ptr, a_count) InterlockedExchangeAdd64((LONGLONG*)a_ptr, a_count)

//#define CAS(a_ptr, a_oldVal, a_newVal) InterlockedCompareExchange((LONG*)a_ptr, a_newVal, a_oldVal)

inline bool CAS(volatile unsigned int *a_ptr, unsigned int a_oldVal, unsigned int a_newVal){
	LONG l = InterlockedCompareExchange((LONG*)a_ptr, a_newVal, a_oldVal);
	return *a_ptr == a_newVal;
}

#endif // __GNUC__


#endif // __ATOMIC_OPS_H
