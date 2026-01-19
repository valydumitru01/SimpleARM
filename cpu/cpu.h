//
// Created by valentin on 12/20/25.
//
#pragma once

#include <stdint.h>
#include "cpsr.h"
#include "registers.h"


#define INITIAL_PC 0





typedef struct CpuState {
    /// General purpose registers R0-R15
    /// R15 is the program counter (PC)
    Registers regs;
    /// Current Program Status Register (CPSR)
    /// Bits for flags: N (Negative), Z (Zero), C (Carry), V (Overflow) \n
    ///
    /// bit 31\n
    /// | N | Z | C | V |...| bit 0
    Cpsr cpsr;
} CpuState;

static inline CpuState construct_cpu_state() {
    const CpuState cpu = {construct_registers(), construct_cpsr()};
    return cpu;
}

static inline void cpu_set_reg(CpuState *cpu, const RegisterIndex index, const uint32_t value) {
    // Defensive: cpu must be valid
    assert(cpu != NULL);
    assert(index < REGISTER_COUNT);

    regs_set(&cpu->regs, index, value);
}

static inline word_t cpu_get_reg(const CpuState *cpu, const RegisterIndex index) {
    // Defensive: cpu must be valid
    assert(cpu != NULL);
    assert(index < REGISTER_COUNT);

    return regs_get(&cpu->regs, index);
}

