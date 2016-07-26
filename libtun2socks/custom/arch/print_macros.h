//
//  print_macros.h
//  tun2socks
//
//  Created by LEI on 7/26/16.
//  Copyright © 2016 TouchingApp. All rights reserved.
//

#ifndef PRINT_MACROS
#define PRINT_MACROS

#ifdef _MSC_VER

// size_t
#define PRIsz "Iu"

// signed exact width (intN_t)
#define PRId8 "d"
#define PRIi8 "i"
#define PRId16 "d"
#define PRIi16 "i"
#define PRId32 "I32d"
#define PRIi32 "I32i"
#define PRId64 "I64d"
#define PRIi64 "I64i"

// unsigned exact width (uintN_t)
#define PRIo8 "o"
#define PRIu8 "u"
#define PRIx8 "x"
#define PRIX8 "X"
#define PRIo16 "o"
#define PRIu16 "u"
#define PRIx16 "x"
#define PRIX16 "X"
#define PRIo32 "I32o"
#define PRIu32 "I32u"
#define PRIx32 "I32x"
#define PRIX32 "I32X"
#define PRIo64 "I64o"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define PRIX64 "I64X"

// signed maximum width (intmax_t)
#define PRIdMAX "I64d"
#define PRIiMAX "I64i"

// unsigned maximum width (uintmax_t)
#define PRIoMAX "I64o"
#define PRIuMAX "I64u"
#define PRIxMAX "I64x"
#define PRIXMAX "I64X"

// signed pointer (intptr_t)
#define PRIdPTR "Id"
#define PRIiPTR "Ii"

// unsigned pointer (uintptr_t)
#define PRIoPTR "Io"
#define PRIuPTR "Iu"
#define PRIxPTR "Ix"
#define PRIXPTR "IX"

#else

#include <inttypes.h>

#define PRIsz "zu"

#endif

#endif