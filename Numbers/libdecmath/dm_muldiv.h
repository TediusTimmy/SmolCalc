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
#ifndef DM_MULDIV_H
#define DM_MULDIV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SIZEOF_INT128__
   #define DM_NO_128_BIT_TYPE
#endif

#ifndef DM_NO_128_BIT_TYPE
typedef __uint128_t dm_muldiv_t[1];
#else
typedef uint32_t dm_muldiv_t[4];
#endif

void dm_muldiv_loadFrom (dm_muldiv_t, uint64_t);
void dm_muldiv_mulBy    (dm_muldiv_t, uint64_t);
void dm_muldiv_divBy    (dm_muldiv_t, uint64_t, uint64_t*, uint64_t*);

void dm_muldiv_sub      (dm_muldiv_t, dm_muldiv_t, dm_muldiv_t);

int  dm_muldiv_zero     (dm_muldiv_t);
int  dm_muldiv_less     (dm_muldiv_t, dm_muldiv_t);

#ifdef __cplusplus
}
#endif

#endif /* DM_MULDIV_H */
