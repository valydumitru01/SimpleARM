
#pragma once
#include "data_processing.h"
#include "memory.h"

typedef enum ShiftType : byte_t {
    SHIFT_LSL = 0,
    SHIFT_LSR = 1,
    SHIFT_ASR = 2,
    SHIFT_ROR = 3,
} ShiftType;

typedef struct Operand2RegShiftImm {
    /// Type of the shift
    ShiftType type;
    /// Immediate value specifying the shift amount
    uint8_t imm5;
} Operand2RegShiftImm;

typedef struct Operand2RegShiftReg {
    /// Type of the shift
    ShiftType type;
    /// Register that specifies the shift amount
    register_index_t rs;
} Operand2RegShiftReg;

typedef struct RegShift {
    /// If true, shift is specified by register
    /// If false, shift is specified by immediate
    bool shift_by_reg;

    union {
        /// Shift specification when shift is by immediate
        Operand2RegShiftImm by_imm;
        /// Shift specification when shift is by register
        Operand2RegShiftReg by_reg;
    };
} RegShift;

typedef struct Operand2Reg {
    /// Second operand register index
    register_index_t rm;
    /// Shift specification
    RegShift shift;
} Operand2Reg;

typedef struct Operand2Imm {
    /// 8-bit immediate value
    /// [0:7]
    byte_t imm8;
    /// 4-bit rotate value
    /// [8:11]
    RegShift rotate;
} Operand2Imm;

;

typedef struct DecodedDataProcessing {
    word_t raw;
    CondCode cond;
    bool immediate_mode;
    OpCode op;
    bool set_condition_codes;
    /// Index of the first register (first operator)
    register_index_t rn;
    /// Index of the destination register
    register_index_t rd;

    /// Second operand, either register or immediate
    /// It is a union because it occupies the same space in the instruction
    union {
        Operand2Reg reg_operand;
        Operand2Imm imm_operand;
    };
} DecodedDataProcessing;


static inline DecodedDataProcessing decode(const word_t raw_inst, FaultCodeExecute* fault_out) {
    static const uint8_t COND_SHIFT = 28u;
    static const uint8_t I_SHIFT = 24u;
    static const uint8_t OP_SHIFT = 21u;
    static const uint8_t S_SHIFT = 20u;
    static const uint8_t RN_SHIFT = 16u;
    static const uint8_t RD_SHIFT = 12u;

    static const uint8_t SHIFT_SHIFT = 4u;
    static const uint8_t SHIFT_BY_REG_TYPE_SHIFT = 5u;
    static const uint8_t SHIFT_BY_REG_RS_SHIFT = 8u;
    static const uint8_t SHIFT_BY_IMM_TYPE_SHIFT = 5u;
    static const uint8_t SHIFT_BY_IMM_IMM5_SHIFT = 7u;
    static const uint8_t RM_SHIFT = 0u;

    static const uint8_t ROT_SHIFT = 8u;
    static const uint8_t IMM_SHIFT = 0u;

    static const word_t COND_MASK = 0xF0000000;
    static const word_t I_MASK = 0x02000000;
    static const word_t OP_MASK = 0x01E00000;
    static const word_t S_MASK = 0x00100000;
    static const word_t RN_MASK = 0x000F0000;
    static const word_t RD_MASK = 0x0000F000;

    static const word_t SHIFT_MASK = 0x00000FF0;
    static const word_t SHIFT_BY_REG_MASK = 0x00000010;
    static const word_t SHIFT_BY_REG_TYPE_MASK = 0x00000060;
    static const word_t SHIFT_BY_REG_RS_MASK = 0x00000F00;
    static const word_t SHIFT_BY_IMM_TYPE_MASK = 0x00000060;
    static const word_t SHIFT_BY_IMM_IMM5_MASK = 0x000000F80;
    static const word_t RM_MASK = 0x0000000F;

    static const word_t ROT_MASK = 0x00000F00;
    static const word_t IMM_MASK = 0x000000FF;
    DecodedDataProcessing d;
    d.raw = raw_inst;

    d.cond = (raw_inst & COND_MASK) >> COND_SHIFT;
    d.immediate_mode = (raw_inst & I_MASK) != 0;
    d.op = (raw_inst & OP_MASK) >> OP_SHIFT;
    d.set_condition_codes = (raw_inst & S_MASK) != 0;
    d.rn = (raw_inst & RN_MASK) >> RN_SHIFT;
    d.rd = (raw_inst & RD_MASK) >> RD_SHIFT;

    if (d.immediate_mode) {
        const byte_t rotate = (raw_inst & ROT_MASK) >> ROT_SHIFT;
        const byte_t imm8 = (raw_inst & IMM_MASK) >> IMM_SHIFT;

        d.imm_operand = (Operand2Imm){
            .rotate = rotate,
            .imm8 = imm8
        };
    }
    else {
        const byte_t shift = (raw_inst & SHIFT_MASK) >> SHIFT_SHIFT;
        const register_index_t rm = (raw_inst & RM_MASK) >> RM_SHIFT;
        RegShift shift_reg;

        shift_reg.shift_by_reg = (shift >> SHIFT_BY_IMM_IMM5_SHIFT) & SHIFT_BY_REG_MASK;
        if (shift_reg.shift_by_reg) {
            shift_reg.by_reg.type = (shift >> SHIFT_BY_REG_TYPE_SHIFT) & SHIFT_BY_REG_TYPE_MASK;
            shift_reg.by_reg.rs = (shift >> SHIFT_BY_REG_RS_SHIFT) & SHIFT_BY_REG_RS_MASK;
        } else {
            shift_reg.by_imm.type = (shift >> SHIFT_BY_IMM_TYPE_SHIFT) & SHIFT_BY_IMM_TYPE_MASK;
            shift_reg.by_imm.imm5 = (shift >> SHIFT_BY_IMM_IMM5_SHIFT) & SHIFT_BY_IMM_IMM5_MASK;
        }


        d.reg_operand = (Operand2Reg){
            .shift = shift,
            .rm = rm
        };
    }



    return d;
}

