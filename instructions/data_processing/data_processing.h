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

    // Ensure registers are valid
    if (inst->rd >= REGISTER_COUNT || inst->rd >= REGISTER_COUNT ) {
        *fault_out = ERROR_INVALID_REGISTER_INDEX;
        return ERROR;
    }
    if (inst->immediate_mode) {
        if (inst->reg_operand.rm >= REGISTER_COUNT) {
            *fault_out = ERROR_INVALID_REGISTER_INDEX;
        }
        return ERROR;
    }

    switch (inst->op) {
    case OP_ADD:
        add_op(cpu, inst);
        break;

    case OP_SUB:
        sub_op(cpu, inst);
        break;

    case OP_AND:
        and_op(cpu, inst);
        break;

    case OP_ORR:
        or_op(cpu, inst);
        break;

    case OP_CMP:
        cmp_op(cpu, inst);
        break;

    case OP_MOV:
        mov_op(cpu, inst);
        break;

    default:
        assert(false && "Unknown data processing opcode");
        break;
    }
}