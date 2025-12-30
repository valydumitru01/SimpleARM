//
// Created by valentin on 12/24/25.
//
#pragma once
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "instructions.h"
#include "../cpu.h"



static inline void halt_op(ExecResult *result) {
    assert(result != NULL);
    result->should_halt = 1;
}

static inline void mul_op(CpuState *cpu, const DecodedInst *inst) {
    assert(cpu != NULL);
    assert(inst != NULL);


    const uint32_t a = cpu->regs[inst->ra];
    const uint32_t b = cpu->regs[inst->rb];
    const uint32_t r = a * b;
    cpu->regs[inst->rd] = r;
    cpu_extract_nz(cpu, r);
}

static inline void cmp_op(CpuState *cpu, const DecodedInst *inst) {
    assert(cpu != NULL);
    assert(inst != NULL);


    const uint32_t a = cpu->regs[inst->ra];
    const uint32_t b = cpu->regs[inst->rb];
    // We don't write on the out register
    // many CPU don't modify operands on CMP
    // we just set the flags based on the subtraction
    const uint32_t r = a - b;
    cpu_extract_nz(cpu, r);
    cpu_set_sub_flags(cpu, a, b, r);
}

static inline void nop_op(CpuState *cpu) {
    // Nothing happens
}


static inline void mov_op(CpuState *cpu, const DecodedInst *inst) {
    assert(cpu != NULL);
    assert(inst != NULL);

    uint32_t *d = &cpu->regs[inst->rd];

    const bool is_imm = (inst->raw >> MOV_IS_IMM_SHIFT) & MOV_IS_IMM_MASK;
    if (is_imm) {
        *d = inst->imm;
        cpu_extract_nz(cpu, *d);
        return;
    }
    const uint32_t *b = &cpu->regs[inst->rb];

    *d = *b;
    cpu_extract_nz(cpu, *d);
}
