// bsls_atomicoperations_arm_all_gcc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_ARM_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_ARM_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for ARM/GCC.
//
//@CLASSES:
//  bsls::AtomicOperations_ARM_ALL_GCC: implementation of atomics for ARM/GCC.
//
//@AUTHOR: Alexei Zakharov (azakhar1), Andrei Basov (abasov)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Linux ARM platform with GCC.  The classes are for private use only.
// See 'bsls_atomicoperations' and 'bsls_atomic' for the public interface to
// atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_ARM) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_ARM_ALL_GCC;
typedef AtomicOperations_ARM_ALL_GCC  AtomicOperations_Imp;

           // ======================================================
           // struct Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC>
           // ======================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(Types::Int64))));
    };

    struct Pointer
    {
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
    };
};

                     // ===================================
                     // struct AtomicOperations_ARM_ALL_GCC
                     // ===================================

struct AtomicOperations_ARM_ALL_GCC
    : AtomicOperations_Default32<AtomicOperations_ARM_ALL_GCC>
{
    typedef Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC> AtomicTypes;

        // *** atomic functions for int ***

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------------------
                     // struct AtomicOperations_ARM_ALL_GCC
                     // -----------------------------------

#if BSLS_PLATFORM_CMP_VERSION < 40700
#   if defined(BSLS_PLATFORM_CPU_ARM_V7)

#       define BSLS_ATOMICOPERATIONS_BARRIER()          \
            asm volatile ("dmb" ::: "memory")           \

#       define BSLS_ATOMICOPERATIONS_INSTR_BARRIER()    \
            asm volatile ("isb" ::: "memory")           \

#   else

#       define BSLS_ATOMICOPERATIONS_BARRIER()                  \
            int temp_reg;                                       \
            asm volatile (                                      \
                    "mcr p15, 0, %[temp_reg], c7, c10, 5 \n\t"  \
                    :                                           \
                    : [temp_reg] "r" (temp_reg)                 \
                    : )                                         \

#       define BSLS_ATOMICOPERATIONS_INSTR_BARRIER()            \
            int temp_reg;                                       \
            asm volatile (                                      \
                    "mcr p15, 0, %[temp_reg], c7, c5, 4 \n\t"   \
                    :                                           \
                    : [temp_reg] "r" (temp_reg)                 \
                    : )                                         \

#   endif
#else
#   define BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
#endif

inline
int AtomicOperations_ARM_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
#else
    int result = atomicInt->d_value;
    BSLS_ATOMICOPERATIONS_BARRIER();

    return result;

#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
#else
    return getInt(atomicInt);
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    BSLS_ATOMICOPERATIONS_BARRIER();
    atomicInt->d_value = value;
    BSLS_ATOMICOPERATIONS_BARRIER();
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
#else
    BSLS_ATOMICOPERATIONS_BARRIER();
    setInt(atomicInt, value);
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_exchange_n(&atomicInt->d_value,
                               swapValue,
                               __ATOMIC_SEQ_CST);
#else
    int oldValue;
    int tmp;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrex   %[old], [%3]            \n\t"
        "       strex   %[tmp], %[val], [%3]    \n\t"
        "       teq     %[tmp], #0              \n\t"
        "       bne     1b                      \n\t"

                : [old] "=&r" (oldValue),
                  [tmp] "=&r" (tmp),
                        "+Qo" (*atomicInt)
                :       "r"   (atomicInt),
                  [val] "Ir"  (swapValue)
                : "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compareValue;
#else
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
#else
    Types::Int64 result;

    asm volatile (
        "       ldrexd  %[res], %H[res], [%1]       \n\t"

                : [res] "=&r" (result)
                :       "r"   (atomicInt),
                        "Qo"  (*atomicInt)
                : "memory");

    BSLS_ATOMICOPERATIONS_BARRIER();

    return result;
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    swapInt64(atomicInt, value);
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_exchange_n(&atomicInt->d_value,
                               swapValue,
                               __ATOMIC_SEQ_CST);
#else
    Types::Int64 oldValue;
    int rc;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrexd  %[old], %H[old], [%3]           \n\t"
        "       strexd  %[rc], %[val], %H[val], [%3]    \n\t"
        "       teq     %[rc], #0                       \n\t"
        "       bne     1b                              \n\t"

                : [rc]  "=&r" (rc),
                  [old] "=&r" (oldValue),
                        "+Qo" (*atomicInt)
                :       "r"   (atomicInt),
                  [val] "Ir"  (swapValue)
                : "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_compare_exchange_n(&atomicInt->d_value,
                                &compareValue,
                                swapValue,
                                false,
                                __ATOMIC_SEQ_CST,
                                __ATOMIC_SEQ_CST);
    return compareValue;
#else
    Types::Int64 oldValue;
    int rc;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrexd  %[old], %H[old], [%3]           \n\t"
        "       mov     %[rc], #0                       \n\t"
        "       teq     %[old], %[cmp]                  \n\t"
        "       teqeq   %H[old], %H[cmp]                \n\t"
        "       strexdeq %[rc], %[val], %H[val], [%3]   \n\t"
        "       teq     %[rc], #0                       \n\t"
        "       bne     1b                              \n\t"

                : [rc]  "=&r" (rc),
                  [old] "=&r" (oldValue),
                        "+Qo" (*atomicInt)
                :       "r"   (atomicInt),
                  [cmp] "r"   (compareValue),
                  [val] "r"   (swapValue)
                : "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    Types::Int64 old;
    Types::Int64 newVal;
    Types::Int64 prev = atomicInt->d_value;

    do {
        old = prev;
        newVal = old + value;
    } while (old != (prev = testAndSwapInt64(atomicInt, old, newVal)));

    return newVal;
#endif
}

}  // close package namespace

}  // close enterprise namespace

#endif  // defined(BSLS_PLATFORM_CPU_ARM) && (CMP_GNU || CMP_CLANG)

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
