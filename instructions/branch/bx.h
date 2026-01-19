//
// Created by valentin on 12/24/25.
//

#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "cpu/cpu.h"
#include "executor/executor.h"
#include "../data_processing/data_proc_op.h"
#include "../cond.h"


typedef struct DecodedBx {
    word_t raw;
    CondCode cond;
    /// Index of the register B (target address)
    /// Is of type uint8_t because we have only 16 registers (0-15)
    RegisterIndex rn;
} DecodedBx;

/// Decodes BX instruction
/// B/BL encoding:
/// [31:28]: Condition field
/// [27: 4]: '0001 0010 1111 1111 1111 0001' fill bits
/// [ 4: 0]: Register index
static inline DecodedBx decode_bx(const word_t raw_inst, FaultCode *fault_out) {

    static const word_t RN_MASK = 0x0000000F;
    static const uint8_t COND_RIGHT_SHIFT = 28;

    // No need to mask, we assume word_t is unsigned
    const CondCode cond = raw_inst >> COND_RIGHT_SHIFT;
    const RegisterIndex rn = raw_inst & RN_MASK;

    const DecodedBx decoded = {.raw = raw_inst, .cond = cond, .rn = rn};
    return decoded;
}


static inline void bx_op(CpuState* cpu, const DecodedBx* inst) {
    static const word_t ARM_PC_ALIGN_MASK      = ~3u; // word align (clear bits0..1)
    static const word_t THUMB_STATE_BIT = 1u; // target bit0 selects Thumb when using BX

    assert(cpu != NULL);
    assert(inst != NULL);

    assert(inst->rn != PC_REGISTER_INDEX && "Using R15 (PC) as operand is undefined");

    const CondCode condition = inst->cond;
    if (!cond_passed(condition, &cpu->cpsr))
        return;

    const word_t target_address = cpu_get_reg(cpu, inst->rn);

    const bool is_thumb = (target_address & THUMB_STATE_BIT) != 0;
    assert(is_thumb == false && "Thumb mode is not supported");

    // Align PC based on next state
    word_t aligned_target_address = target_address;
    aligned_target_address &= ARM_PC_ALIGN_MASK;

    cpu_set_reg(cpu, PC_REGISTER_INDEX, aligned_target_address);
}
