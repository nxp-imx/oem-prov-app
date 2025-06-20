/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2025 NXP
 */

 #ifndef __OEM_PROV_ARITHMETIC_OPS_H__
 #define __OEM_PROV_ARITHMETIC_OPS_H__

 #include <stdint.h>

 #define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

 #define ADD_OVERFLOW(a, b, res) __builtin_add_overflow(a, b, res)
 #define SUB_OVERFLOW(a, b, res) __builtin_sub_overflow(a, b, res)
 #define MUL_OVERFLOW(a, b, res) __builtin_mul_overflow(a, b, res)
 #define INC_OVERFLOW(a, b)	__builtin_add_overflow(a, b, &(a))
 #define DEC_OVERFLOW(a, b)	__builtin_sub_overflow(a, b, &(a))

 #ifndef BIT
 #define BIT(n) (1UL << (n))
 #endif
 #define BIT_MASK(length)        ((1UL << (length)) - 1)
 #define SET_BITS(val, mask)     ((val) |= (mask))
 #define CLEAR_BITS(val, mask)   ((val) &= ~(mask))
 #define IS_BIT_SET(value, n)    (((value) & BIT(n)) != 0)
 #define SET_BIT(value, n)       ((value) |= BIT(n))

 #endif /* __OEM_PROV_ARITHMETIC_OPS_H__ */
