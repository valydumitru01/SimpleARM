//
// Created by valentin on 12/24/25.
//

#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "cpu/cpu.h"
#include "executor/executor.h"
#include "../data_processing/decoder.h"

static inline void mul_op(CpuState *cpu, const DecodedInst *inst) {
    assert(cpu != NULL);
    assert(inst != NULL);


    const uint32_t a = cpu->regs[inst->ra];
    const uint32_t b = cpu->regs[inst->rb];
    const uint32_t r = a * b;
    cpu->regs[inst->rd] = r;
    cpu_extract_nz(cpu, r);
}