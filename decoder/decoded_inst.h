//
// Created by valentin on 12/24/25.
//
#pragma once
#include <stdint.h>
#include "../instructions/opcodes.h"

/// Decoded instruction, from the uint32 that is fetched.
/// The format of the encoded (now decoded) is the following:
///
typedef struct DecodedInst {
    uint32_t raw;
    Opcode op;
    /// Index of the destination register
    /// Is of type uint8_t because we have only 16 registers (0-15)
    uint8_t rd;
    /// Index of the register A (first operator)
    /// Is of type uint8_t because we have only 16 registers (0-15)
    uint8_t ra;
    /// Index of the register B (second operator)
    /// Is of type uint8_t because we have only 16 registers (0-15)
    uint8_t rb;
    /// Immediate, for storing constants
    uint16_t imm;
} DecodedInst;

