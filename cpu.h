//
// Created by valentin on 12/20/25.
//
#pragma once

#include <stdint.h>
#define REGISTER_COUNT 16
#define PC_REGISTER_INDEX 15
#define INITIAL_PC 0
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

typedef uint8_t register_t;

typedef struct CpuState {
    /// General purpose registers R0-R15
    /// R15 is the program counter (PC)
    uint32_t regs[REGISTER_COUNT];
    /// Current Program Status Register (CPSR)
    /// Bits for flags: N (Negative), Z (Zero), C (Carry), V (Overflow) \n
    ///
    /// bit 31\n
    /// | N | Z | C | V |...| bit 0
    uint32_t cpsr;
} CpuState;

static inline CpuState construct_cpu_state() {
    const CpuState cpu = {.regs = {0}, .cpsr = 0};
    return cpu;
}

static inline uint32_t cpu_get_pc(const CpuState *cpu) {
    return cpu->regs[PC_REGISTER_INDEX];
}

static inline void cpu_set_pc(CpuState *cpu, const uint32_t pc) {
    cpu->regs[PC_REGISTER_INDEX] = pc;
}


static inline void cpu_extract_nz(CpuState *cpu, const uint32_t value) {
    // Defensive: cpu must be valid
    assert(cpu != NULL);

    // Clear N and Z; preserve everything else (including C and V)
    cpu->cpsr &= ~(CPSR_FLAG_N | CPSR_FLAG_Z);

    // Z flag: set if result is exactly zero
    if (value == 0u) {
        cpu->cpsr |= CPSR_FLAG_Z;
    }

    // N flag: set if MSB (bit 31) is 1
    if ((value & 0x80000000u) != 0u) {
        cpu->cpsr |= CPSR_FLAG_N;
    }
}


static inline void cpu_set_add_flags(CpuState *cpu, const uint32_t a, uint32_t b, const uint32_t result) {
    assert(cpu != NULL);

    // C Flag: Set if the result carries (goes over the limit in add)
    // If the result is less than one of the operands, a carry occurred
    if (result < a) {
        // Set the C flag
        cpu->cpsr |= CPSR_FLAG_C;
    } else {
        // Remove the C flag
        cpu->cpsr &= ~CPSR_FLAG_C;
    }
}

static inline void cpu_set_sub_flags(CpuState *cpu, const uint32_t a, const uint32_t b, const uint32_t result) {
    assert(cpu != NULL);

    // C Flag: Set if the operation does NOT borrow (a >= b)
    // If a is greater than or equal to b, no borrow occurred
    if (a >= b) {
        // Set the C flag
        cpu->cpsr |= CPSR_FLAG_C;
    } else {
        // Remove the C flag
        cpu->cpsr &= ~CPSR_FLAG_C;
    }
}