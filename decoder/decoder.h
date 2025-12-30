//
// Created by valentin on 12/24/25.
//
#pragma once
#include <stdint.h>
#include "../instructions/opcodes.h"
#include "../faults/codes.h"
#include "decoded_inst.h"

#define INST_OPCODE_SHIFT   28u
#define INST_RD_SHIFT       24u
#define INST_RA_SHIFT       20u
#define INST_RB_SHIFT       16u

#define MOV_IS_IMM_SHIFT    23u

/// Mask for a 4-bit field (e.g., opcode, rd, ra, rb)
/// Used to extract the relevant bits from the instruction (the last 4 bits of the field)
///
/// Underlying binary: \n
/// 0000 0000 0000 0000 0000 0000 0000 1111
#define INST_FIELD_MASK  0xFu


/// Mask for extracting the immediate (16 bits) from the instruction
#define INST_IMM_MASK     0xFFFFu

/// Mask a 1-bit flag to extract whether it is an immediate mov or not
#define MOV_IS_IMM_MASK   0x1u



static DecodedInst decode(const uint32_t raw_inst, FaultCode *fault_out)
{
    DecodedInst d = {0};

    d.raw = raw_inst;

    // No need to shift for the immediate as it occupies
    // the rightmost bits.
    d.imm =(uint16_t)(raw_inst & INST_IMM_MASK);
    d.op = (Opcode)((raw_inst >> INST_OPCODE_SHIFT) & INST_FIELD_MASK);
    d.rd = (raw_inst >> INST_RD_SHIFT) & INST_FIELD_MASK;
    d.ra = (raw_inst >> INST_RA_SHIFT) & INST_FIELD_MASK;
    d.rb = (raw_inst >> INST_RB_SHIFT) & INST_FIELD_MASK;

    return d;
}