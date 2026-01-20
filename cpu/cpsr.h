//
// Created by valentin on 12/20/25.
//
#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
/// CPSR Flag for Negative condition
/// Bit 31
/// Set when the result of an operation is negative
/// (i.e., the most significant bit of the result is 1)
#define CPSR_FLAG_N (1u << 31)
/// CPSR Flag for Zero condition
/// Bit 30
/// Set when the result of an operation is zero
/// (i.e., all bits of the result are 0)
#define CPSR_FLAG_Z (1u << 30)
/// CPSR Flag for Carry condition
/// Bit 29
/// Set when an operation results in a carry out of the most significant bit
/// (e.g., in addition) or a borrow into the most significant bit (e.g., in subtraction)
/// Used for *unsigned* arithmetic operations
///
/// Example: \n
/// 0xFFFFFFFF + 0x1 = 0x00000000 with carry \n
/// or 0x00000000 - 0x1 = 0xFFFFFFFF with borrow
#define CPSR_FLAG_C (1u << 29)
/// CPSR Flag for Overflow condition
/// Bit 28
/// Set when an operation results in a signed overflow
/// (i.e., when the result of a signed operation is too large or too small to be represented
/// in the available number of bits)
/// Used for *signed* arithmetic operations
#define CPSR_FLAG_V (1u << 28)
/// CPSR Flag for Thumb state
/// Bit 5
/// Set when the CPU is in Thumb mode (16-bit instruction set)
/// Cleared when in ARM mode (32-bit instruction set)
#define CPSR_FLAG_T (1u << 5)


typedef struct {
    uint32_t value;
} Cpsr;

static inline Cpsr construct_cpsr() {
    const Cpsr cpsr = {0};
    return cpsr;
}

static inline bool cpsr_get_negative(const Cpsr* c) { return c->value & CPSR_FLAG_N; }
static inline bool cpsr_get_zero(const Cpsr* c) { return c->value & CPSR_FLAG_Z; }
static inline bool cpsr_get_carry(const Cpsr* c) { return c->value & CPSR_FLAG_C; }
static inline bool cpsr_get_overflow(const Cpsr* c) { return c->value & CPSR_FLAG_V; }
static inline bool cpsr_get_thumb(const Cpsr* c) { return c->value & CPSR_FLAG_T; }

static inline void cpsr_clear_negative(Cpsr* cpsr) { cpsr->value &= ~CPSR_FLAG_N; }
static inline void cpsr_clear_zero(Cpsr* cpsr) { cpsr->value &= ~CPSR_FLAG_Z; }
static inline void cpsr_clear_carry(Cpsr* cpsr) { cpsr->value &= ~CPSR_FLAG_C; }
static inline void cpsr_clear_overflow(Cpsr* cpsr) { cpsr->value &= ~CPSR_FLAG_V; }
static inline void cpsr_clear_thumb(Cpsr* cpsr) { cpsr->value &= ~CPSR_FLAG_T; }

static inline void cpsr_set_negative(Cpsr* cpsr) { cpsr->value |= CPSR_FLAG_N; }
static inline void cpsr_set_zero(Cpsr* cpsr) { cpsr->value |= CPSR_FLAG_Z; }
static inline void cpsr_set_carry(Cpsr* cpsr) { cpsr->value |= CPSR_FLAG_C; }
static inline void cpsr_set_overflow(Cpsr* cpsr) { cpsr->value |= CPSR_FLAG_V; }
static inline void cpsr_set_thumb(Cpsr* cpsr) { cpsr->value |= CPSR_FLAG_T; }

static inline void cpsr_extract_negative(Cpsr* cpsr, const uint32_t value) {
    // Defensive: cpu must be valid
    assert(cpsr != NULL);

    // N flag: set if most significative bit (bit 31) is 1
    if ((value & 0x80000000u) != 0u)
        cpsr_set_negative(cpsr);
    else
        cpsr_clear_negative(cpsr);
}

static inline void cpsr_extract_zero(Cpsr* cpsr, const uint32_t value) {
    // Defensive: cpu must be valid
    assert(cpsr != NULL);

    // Z flag: set if result is exactly zero
    if (value == 0u)
        cpsr_set_zero(cpsr);
    else
        cpsr_clear_zero(cpsr);
}

static inline void cpsr_extract_overflow_add(Cpsr* cpsr, const word_t operand1, const word_t operand2, const word_t result) {
    assert(cpsr != NULL);
    static const word_t SIGN_EXTRACT_MASK = 0x80000000u;

    const word_t a_b_difference = operand1 ^ operand2;
    const word_t a_r_difference = operand1 ^ result;

    // ADD overflow:
    // - operands have the same sign
    const bool same_sign_operands = (a_b_difference & SIGN_EXTRACT_MASK) == 0u;
    // - result sign differs from operand a
    const bool result_differs = (a_r_difference & SIGN_EXTRACT_MASK) != 0u;

    const bool overflow = same_sign_operands && result_differs;

    if (overflow) cpsr_set_overflow(cpsr);
    else cpsr_clear_overflow(cpsr);
}

static inline void cpsr_extract_overflow_sub(Cpsr* cpsr, const word_t operand1, const word_t operand2, const word_t result) {
    assert(cpsr != NULL);
    static const word_t SIGN_EXTRACT_MASK = 0x80000000u;

    const word_t a_b_difference = operand1 ^ operand2;
    const word_t a_r_difference = operand1 ^ result;

    // SUB overflow (r = a - b):
    // - operands have different signs
    const bool different_sign_operands = (a_b_difference & SIGN_EXTRACT_MASK) != 0u;
    // - result sign differs from operand a
    const bool result_differs = (a_r_difference & SIGN_EXTRACT_MASK) != 0u;

    const bool overflow = different_sign_operands && result_differs;

    if (overflow) cpsr_set_overflow(cpsr);
    else cpsr_clear_overflow(cpsr);
}


static inline void cpsr_extract_carry_add(Cpsr* cpsr, const word_t operand1, const word_t operand2) {
    uint64_t wide_result = (uint64_t) operand1 + (uint64_t) operand2;
    // If the next bit of the msb of 32 bits is set, a carry occurred
    // (meaning the result is more than what 32 bits can represent)
    if ((wide_result >> 32) != 0) {
        // Set the C flag
        cpsr_set_carry(cpsr);
    }
    else {
        // No carry occurred
        cpsr_clear_carry(cpsr);
    }
}

static inline void cpsr_extract_carry_sub(Cpsr* cpsr, const word_t operand1, const word_t operand2) {
    // C Flag: Set if the operation does NOT borrow (a >= b)
    // If it is greater than or equal to b, no borrow occurred
    if (operand1 >= operand2) {
        // Set the C flag
        cpsr_set_carry(cpsr);
    }
    else {
        // No carry occurred
        cpsr_clear_carry(cpsr);
    }
}
