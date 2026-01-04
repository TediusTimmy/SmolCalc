/*
BSD 3-Clause License

Copyright (c) 2022, Thomas DiModica
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef DM_DOUBLE_H
#define DM_DOUBLE_H

/*
   dm_double is a decimal floating point class with sixteen digits of
   significand and an exponent range of -511 to 511, inclusive.

   It is eight bytes (64 bits) per number. The exponent range is intended to
   cover up the lack of subnormals.

   The significand is a sixteen-digit, normalized number:
      1000000000000000 - 9999999999999999.
   It is biased by 1000000000000000 in order to fit into 53 bits.

   The sign is stored as the most significant bit.

   The exponent field is a ten bit number, biased by 512. The value -512
   is reserved for special cases, indicated in the significand:
      All zeros : zero
      All ones : infinity
      Anything else : NaN

   The IEEE-754 exceptions perform their default behavior, as most users of
      the C++ types float or double would expect.
*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t dm_double;


int dm_double_fpclassify  (dm_double); // Returns FP_NORMAL for non-canonical numbers.
int dm_double_iscanonical (dm_double); // Returns 0 for "trap representations"
int dm_double_isfinite    (dm_double);
int dm_double_isinf       (dm_double);
int dm_double_isnan       (dm_double);
int dm_double_isnormal    (dm_double);
int dm_double_signbit     (dm_double);
int dm_double_issubnormal (dm_double); // Always 0
int dm_double_issignaling (dm_double); // Always 0
int dm_double_iszero      (dm_double);

int dm_double_isgreater      (dm_double, dm_double);
int dm_double_isgreaterequal (dm_double, dm_double);
int dm_double_isless         (dm_double, dm_double);
int dm_double_islessequal    (dm_double, dm_double); // All animals are equal but some are more equal than others
int dm_double_islessgreater  (dm_double, dm_double);
int dm_double_isunordered    (dm_double, dm_double);
//int dm_double_iseqsig        (dm_double, dm_double); // No exceptions

int dm_double_isequal        (dm_double, dm_double); // Because no exceptions
int dm_double_isunequal      (dm_double, dm_double);


dm_double dm_double_fabs     (dm_double);
dm_double dm_double_fmin     (dm_double, dm_double);
dm_double dm_double_fmax     (dm_double, dm_double);
dm_double dm_double_copysign (dm_double, dm_double);

#define DM_FE_TONEAREST           0
#define DM_FE_TONEARESTFROMZERO   1
#define DM_FE_UPWARD              2
#define DM_FE_DOWNWARD            3
#define DM_FE_TOWARDZERO          4
#define DM_FE_TONEARESTODD        5
#define DM_FE_TONEARESTTOWARDZERO 6
#define DM_FE_FROMZERO            7
int dm_fesetround  (int);
int dm_fegetround  ();
uint64_t dm_decideRound (int, int, int64_t, int, int);

dm_double dm_double_ceil      (dm_double); // DM_FE_UPWARD
dm_double dm_double_floor     (dm_double); // DM_FE_DOWNWARD
dm_double dm_double_trunc     (dm_double); // DM_FE_TOWARDZERO
dm_double dm_double_nearbyint (dm_double); // dm_fegetround()
dm_double dm_double_round     (dm_double); // DM_FE_TONEARESTFROMZERO
dm_double dm_double_roundeven (dm_double); // DM_FE_TONEAREST

dm_double dm_double_modf      (dm_double, dm_double*);
dm_double dm_double_frexp     (dm_double, int*);       // This is BASE 10!
dm_double dm_double_ldexp     (dm_double, int);        // This is BASE 10!

dm_double dm_double_fmod (dm_double, dm_double);
dm_double dm_double_fma  (dm_double, dm_double, dm_double);

dm_double dm_double_neg   (dm_double);            // -x
dm_double dm_double_add   (dm_double, dm_double); // x + y
dm_double dm_double_sub   (dm_double, dm_double); // x - y
dm_double dm_double_mul   (dm_double, dm_double); // x * y
dm_double dm_double_div   (dm_double, dm_double); // x / y
dm_double dm_double_add_r (dm_double, dm_double, int);
dm_double dm_double_sub_r (dm_double, dm_double, int);
dm_double dm_double_mul_r (dm_double, dm_double, int);
dm_double dm_double_div_r (dm_double, dm_double, int);


void        dm_double_tostring       (dm_double, char [24]); // 24? -9.999999999999999e-511\0
dm_double   dm_double_fromstring     (const char *);
#ifndef DM_NO_DOUBLE_MATH
double      dm_double_todouble       (dm_double);
dm_double   dm_double_fromdouble     (double);
   #ifndef DM_NO_LONG_DOUBLE_MATH
long double dm_double_tolongdouble   (dm_double);
dm_double   dm_double_fromlongdouble (long double);
   #endif /* ! DM_NO_LONG_DOUBLE_MATH */
#endif /* ! DM_NO_DOUBLE_MATH */

#ifdef __cplusplus
}
#endif

#endif /* DM_DOUBLE_H */
