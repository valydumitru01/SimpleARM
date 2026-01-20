//
// Created by valentin on 12/24/25.
//

#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "memory.h"
#include "cpu/cpu.h"
#include "instructions/cond.h"
#include "faults/codes.h"

typedef struct DecodedB {
    word_t raw;
    /// The comparison operator (EQ, LT, etc)
    CondCode cond;
    /// Boolean flag to indicate if we store the pc to link register
    bool link;
    /// 32-bit signed offset to branch to
    /// This value must be added to the PC to get the target address
    int32_t offset;
} DecodedB;

static inline DecodedB decode_b(const word_t raw_inst, FaultCodeDecode* fault_out) {
    static const word_t OFFSET_MASK = 0x00FFFFFF;
    static const word_t LINK_MASK = 0x1000000;
    static const word_t COND_MASK = 0xF0000000;
    static const uint8_t COND_RIGHT_SHIT = 28;
    static const uint8_t OFFSET_SIGN_EXTEND_SHIFT = 8;
    static const uint8_t OFFSET_BYTE_SCALE_SHIFT = 2;

    // The cond must be masked to extract
    // and then must be shifted right (27) to get rid of the padding
    const CondCode cond = (raw_inst & COND_MASK) >> COND_RIGHT_SHIT;

    // We extract the 24 bits immediate offset with the mask,
    // and we convert to int32, to interpret as 2s complement
    int32_t imm24 = (int32_t)(raw_inst & OFFSET_MASK);
    // We sign-extend 24 -> 32
    // (fills left side with 1s)
    imm24 = (imm24 << OFFSET_SIGN_EXTEND_SHIFT) >> OFFSET_SIGN_EXTEND_SHIFT;
    // Then we need to scale it to bytes, as
    // required by the ARM specification
    const int32_t offset = imm24 << OFFSET_BYTE_SCALE_SHIFT;

    const bool link = (raw_inst & LINK_MASK) != 0;

    const DecodedB inst = {.raw = raw_inst, .cond = cond, .link = link, .offset = offset};
    return inst;
}

static inline void b_op(CpuState* cpu, const DecodedB* inst, FaultCodeExecute* fault_out) {
    static const word_t LINK_ADDRESS_CLEARED_BITS_MASK = ~3u;
    assert(cpu != NULL);
    assert(inst != NULL);


    if (!cond_passed(inst->cond, &cpu->cpsr)) {
        return;
    }

    const word_t pc = cpu_get_reg(cpu, PC_REGISTER_INDEX);
    if (inst->link) {
        // Subtract one word to account for instruction prefetch
        // PC points two instructions ahead, and we need to store the next instruction
        // after this branch in case we want to continue where we left off
        word_t stored_address = pc - WORD_SIZE_BYTES;
        // Clear the bits [1:0] for alignment
        stored_address &= LINK_ADDRESS_CLEARED_BITS_MASK;
        // Store the "left off" address to the link register
        cpu_set_reg(cpu, LINK_REGISTER_INDEX, stored_address);
    }
    const word_t next_address = pc + inst->offset;
    cpu_set_reg(cpu, PC_REGISTER_INDEX, next_address);
}
