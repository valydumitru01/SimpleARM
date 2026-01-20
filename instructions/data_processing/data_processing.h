//
// Created by valentin on 12/24/25.
//
#pragma once
#include <stdint.h>
#include "instructions/opcodes.h"
#include "faults/codes.h"
#include "instructions/cond.h"
#include "instructions/data_processing/data_processing_decoder.h"
#include "instructions/data_processing/operations.h"




static inline ReturnStatus data_proc_op(CpuState* cpu, const DecodedDataProcessing* inst, FaultCodeExecute* fault_out) {
    assert(cpu != NULL);
    assert(inst != NULL);

    // Ensure registers are valid
    if (inst->rd >= REGISTER_COUNT || inst->rd >= REGISTER_COUNT ) {
        *fault_out = ERROR_INVALID_REGISTER_INDEX;
        return ERROR;
    }
    // If PC is used as operand, warn
    if (inst->rd == PC_REGISTER_INDEX || inst->rn == PC_REGISTER_INDEX) {
        *fault_out = WARNING_PC_USED_AS_OPERAND;
    }

    // The same thing as before, but for the second operand
    // if not in immediate mode
    if (!inst->immediate_mode) {
        if (inst->reg_operand.rm >= REGISTER_COUNT) {
            *fault_out = ERROR_INVALID_REGISTER_INDEX;
            return ERROR;
        }
        if (inst->reg_operand.rm == PC_REGISTER_INDEX) {
            *fault_out = WARNING_PC_USED_AS_OPERAND;
        }
    }

    const word_t operand1 = cpu_get_reg(cpu, inst->rn);
    const word_t operand2 = inst->immediate_mode ? inst->imm_operand.imm8 : cpu_get_reg(cpu, inst->reg_operand.rm);
    const register_index_t result_reg = inst->reg_operand.rm;

    switch (inst->op) {
    case OP_ADD:
        add_op(cpu, result_reg, operand1, operand2);
        break;

    case OP_SUB:
        sub_op(cpu, result_reg, operand1, operand2);
        break;

    case OP_AND:
        and_op(cpu, result_reg, operand1, operand2);
        break;

    case OP_ORR:
        or_op(cpu, result_reg, operand1, operand2);
        break;

    case OP_CMP:
        cmp_op(cpu, result_reg, operand1, operand2);
        break;

    case OP_MOV:
        mov_op(cpu, result_reg, operand1, operand2);
        break;

    default:
        assert(false && "Unknown data processing opcode");
        break;
    }
}