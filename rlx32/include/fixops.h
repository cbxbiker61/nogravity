#pragma once
//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2005 - realtech VR

This file is part of No Gravity 1.9

No Gravity is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Stephane Denis
Prepared for public release: 02/24/2004 - Stephane Denis, realtech VR
*/
//-------------------------------------------------------------------------

#ifndef __FIXOPCODE
#define __FIXOPCODE

#define xMUL8(a, b) ((uint32_t)(((uint32_t)(a)*(uint32_t)(b))>>8))
#define SHL16(x)         ((int32_t)(x)<<16)
#define SHR16(x)         ((int32_t)(x)>>16)
#define SHLD(x)          ((int32_t)(x)<<16)
#define SHRD(x)          ((int32_t)(x)>>16)

#if defined (_HAS_LONGLONG)
     #define VMUL(a, b) ((int32_t)(((__int64)(a) * (__int64)(b)) >> 16))
     #define VDIV(a, b) ((int32_t)(((__int64)(a) << 16) / (b)))

#elif defined (_MSC_VER)


#pragma warning(disable:4035)

#if defined (_M_IX86)
inline int32_t VMUL ( int32_t a, int32_t b )
{
    __asm    {
       mov    eax, a
       mov    ebx, b
       imul   ebx
       shrd   eax, edx, 16
    }
}
inline int32_t VDIV ( int32_t a, int32_t b )
{
    __asm    {
       mov    edx, a
       mov    ebx, b
       mov    eax, 0
       shrd   eax, edx, 16
       sar    edx, 16
       idiv   ebx
    }
}
#pragma warning(default:4035)
#define VMUL2(a)           VMUL(a, b)
#define VMUL_DIV(a, b, c)    VDIV(VMUL(a, b), c)
#define fMUL               VMUL
#define fDIV               VDIV
#endif

#elif defined ( __WATCOMC__)

int32_t VMUL(int32_t a, int32_t b);
#pragma aux VMUL = \
   "imul   ebx" \
   "shrd   eax, edx, 16d" \
   parm   [EAX] [EBX] \
   value  [EAX] \
   modify [EDX];

int32_t VMUL2(int32_t a);
#pragma aux VMUL2 = \
  "imul   eax" \
  "shrd   eax, edx, 16d" \
   parm   [EAX] \
   value  [EAX] \
   modify [EDX];

int32_t VDIV (int32_t A, int32_t B);
#pragma aux VDIV = \
  "mov  eax, 0" \
  "shrd eax, edx, 16" \
  "sar  edx, 16" \
  "idiv ebx" \
   parm [edx] [ebx] \
   value [eax];

int32_t VMUL_DIV (int32_t A, int32_t B, int32_t C);
#pragma aux VMUL_DIV = \
  "imul ebx" \
  "idiv ecx" \
   parm [eax] [ebx] [ecx] \
   value [eax] \
   modify [edx];

#define fMUL             VMUL
#define fDIV             VDIV

#elif defined ( __DJGPP__ )
extern inline int32_t VMUL(int32_t x, int32_t y)
{
    int32_t rv;
    __asm__ __volatile__ (
    "pushl %%edx          \n"
    "imul  %%ecx          \n"
    "shrdl $16, %%edx, %%eax\n"
    "popl  %%edx"
    :"=a" (rv)
    :"a"  (x),
    "c"  (y)
    );
    return rv;
}
extern inline int32_t VMUL2(int32_t x)
{
    int32_t rv;
    __asm__ __volatile__ (
    "pushl %%edx           \n"
    "imul  %%eax           \n"
    "shrdl $16, %%edx, %%eax \n"
    "popl  %%edx"
    :"=a" (rv)
    : "a" (x)
    );
    return rv;
}
extern inline int32_t VDIV(int32_t x, int32_t y)
{
    int32_t rv;
    __asm__ __volatile__ (
    "xorl  %%eax, %%eax    \n"
    "shrdl $16, %%edx, %%eax\n"
    "sarl  $16, %%edx      \n"
    "idivl %%ecx          \n"
    :"=a" (rv)
    :"d"  (x),
    "c"  (y)
    );
    return rv;
}
extern inline int32_t VMUL_DIV (int32_t x, int32_t y, int32_t z)
{
    int32_t rv;
    __asm__ __volatile__ (
    "pushl  %%edx\n"
    "imul   %%ebx\n"
    "idivl  %%ecx\n"
    "popl   %%edx"
    :"=a" (rv)
    :"a"  (x),
    "b"  (y),
    "c"  (z)
    );
    return rv;
}
#else

  #define fMUL(x, y)        (((float)(x)*(float)(y))*(1.f/65536.f))
  #define fDIV(x, y)        (((float)(x)*65536.f)/(float)(y))
  #define VMUL(x, y)        (int32_t)fMUL(x, y)
  #define VDIV(x, y)        (int32_t)fDIV(x, y)
  #define VMUL2(x)         VMUL(x, x)
  #define VMUL_DIV(x, y, z)  ((((float)(x)*(float)(y))/(float)(z)))

#endif

#endif

