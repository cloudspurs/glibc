/* Miscellaneous macros.
   Copyright (C) 2022-2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _SYS_ASM_H
#define _SYS_ASM_H

#include <sys/regdef.h>
#include <sysdeps/generic/sysdep.h>

#define STACK_ALIGN 16

/* Macros to handle different pointer/register sizes for 32/64-bit code.  */
#if __loongarch_grlen == 64
#define PTRLOG 3
#define SZREG 8
#define REG_L ld.d
#define REG_S st.d
#define SRLI srli.d
#define SRAI srai.d
#define SLLI slli.d
#define ADDI addi.d
#define ADD  add.d
#define SUB  sub.d
#define LI  li.d
#define BSTRINS  bstrins.d

/* Align reg to 2^n.  Used in assembly.  */
#define REG_ALIGN_ASM(reg, n) bstrins.d reg, zero, (n-1), 0

/* Align reg to 2^n.  Used in C.  */
#define REG_ALIGN_C(reg, n) \
  "bstrins.d\t" __STRING(reg) ", $zero, (" __STRING(n) "-1), 0"

#define LOAD_ADDR(reg, sym) la.pcrel reg, t0, sym

#define LOAD_LOCAL(reg, sym) \
  pcalau12i reg, %pc_hi20(sym); \
  ld.d	    reg, reg, %pc_lo12(sym);

#define LOAD_GLOBAL(reg, sym) \
  la.got    reg, sym; \
  ld.d	    reg, reg, 0;

#define LA_GOT(reg, sym) la.got reg, t0, sym

#define CALL(sym) call36 sym
#define TAIL(sym) tail36 t0, sym

#elif __loongarch_grlen == 32

#define PTRLOG 2
#define SZREG 4
#define REG_L ld.w
#define REG_S st.w
#define SRLI srli.w
#define SRAI srai.w
#define SLLI slli.w
#define ADDI addi.w
#define ADD  add.w
#define SUB  sub.w
#define LI  li.w
#define BSTRINS  bstrins.w

/* LA32R not have bstrins.w, use srli.w and slli.w on both LA32S and LA32R.  */
#define REG_ALIGN_ASM(reg, n) \
  srli.w reg, reg, n; \
  slli.w reg, reg, n;

#define REG_ALIGN_C(reg, n) \
  "srli.w\t" __STRING(reg)", " __STRING(reg)", " __STRING(n) "\n\t" \
  "slli.w\t" __STRING(reg)", " __STRING(reg)", " __STRING(n)

#define LOAD_ADDR(reg, sym) \
  1: pcaddu12i	reg, %pcadd_hi20(sym); \
     addi.w	reg, reg, %pcadd_lo12(1b);

#define LOAD_LOCAL(reg, sym) \
  1: pcaddu12i	reg, %pcadd_hi20(sym); \
     ld.w	reg, reg, %pcadd_lo12(1b);

#define LOAD_GLOBAL(reg, sym) \
  1: pcaddu12i	reg, %got_pcadd_hi20(sym); \
     ld.w	reg, reg, %pcadd_lo12(1b); \
     ld.w	reg, reg, 0;

#define LA_GOT(reg, sym) la.got reg, sym

#define CALL(sym) call30 sym
#define TAIL(sym) tail30 t0, sym

#else
#error __loongarch_grlen must equal 32 or 64
#endif

#if __loongarch_frlen == 64
  #define SZFREG 8
  #define FREG_L fld.d
  #define FREG_S fst.d
#elif __loongarch_frlen == 32
  #define SZFREG 4
  #define FREG_L fld.s
  #define FREG_S fst.s
#endif

#define SZVREG 16
#define SZXREG 32

/*  Declare leaf routine.
    The usage of macro LEAF/ENTRY is as follows:
    1. LEAF(fcn) -- the align value of fcn is .align 3 (default value)
    2. LEAF(fcn, 6) -- the align value of fcn is .align 6
*/
#define LEAF_IMPL(symbol, aln, ...)	\
	.text;				\
	.globl symbol;			\
	.align aln;			\
	.type symbol, @function;	\
symbol: \
	cfi_startproc;


#define LEAF(...) LEAF_IMPL(__VA_ARGS__, 3)
#define ENTRY(...) LEAF(__VA_ARGS__)

#define	LEAF_NO_ALIGN(symbol)		\
	.text;				\
	.globl	symbol;			\
	.type	symbol, @function;	\
symbol: \
	cfi_startproc;

#define ENTRY_NO_ALIGN(symbol) LEAF_NO_ALIGN(symbol)


/* Mark end of function.  */
#undef END
#define END(function) \
  cfi_endproc; \
  .size function, .- function;

/* Stack alignment.  */
#define ALMASK ~15

#endif /* sys/asm.h */
