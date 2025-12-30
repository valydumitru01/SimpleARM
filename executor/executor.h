//
// Created by valentin on 12/24/25.
//
#pragma once
#include <assert.h>
#include <stdbool.h>

#include "../cpu.h"
#include "../instructions/opcodes.h"
#include "../decoder/decoded_inst.h"
#include "../instructions/instruction_impls.h"

typedef enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_UNKNOWN_OPCODE = -11,
    ERROR_INVALID_REGISTER_INDEX = -12,
} ErrorCode;

typedef struct ExecResult {
    uint32_t next_pc;
    int should_halt;
    ErrorCode error_code;
} ExecResult;

static inline ExecResult execute_instruction(
    CpuState *cpu,
    const DecodedInst *inst
) {
    ExecResult result = {0};
    
    // Ensure registers are valid
    if (inst->rd >= REGISTER_COUNT || inst->ra >= REGISTER_COUNT || inst->rb >= REGISTER_COUNT) {
        result.error_code = ERROR_INVALID_REGISTER_INDEX;
        return result;
    }

    result.next_pc = cpu_get_pc(cpu) + WORD_SIZE;

    switch (inst->op) {
        case NOP:
            nop(cpu);
            break;

        case HALT:
            halt_op(&result);
            break;

        case ADD:
            add_op(cpu, inst);
            break;

        case SUB:
            sub_op(cpu, inst);
            break;

        case AND:
            and_op(cpu, inst);
            break;

        case ORR:
            or_op(cpu, inst);
            break;

        case MUL:
            mul_op(cpu, inst);
            break;

        case CMP:
            cmp_op(cpu, inst);
            break;

        case MOV:
            mov_op(cpu, inst);
            break;

        default:
            result.error_code = ERROR_UNKNOWN_OPCODE;
            break;
    }
    return result;
}