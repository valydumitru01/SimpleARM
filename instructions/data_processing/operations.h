/**************************************************************************************************
 * @file   operations.h
 * @author Valentin Dumitru
 * @date   19/01/2026
 * @brief  Add description of this file if needed @TODO
 *
 * Copyright (c) 2023 Valentin Dumitru. Licensed under the MIT License.
 * See LICENSE.txt in the project root for license information.
 **************************************************************************************************/
#pragma once


#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "cpu/cpu.h"
#include "instructions/data_processing/data_processing_decoder.h"


static inline void mov_op(CpuState *cpu, const DecodedDataProcessing *inst) {
    assert(cpu != NULL);
    assert(inst != NULL);


    const register_index_t destination_reg_idx = inst->rd;

    if (inst->immediate_mode) {
        cpu_set_reg(cpu, destination_reg_idx, inst->imm_operand.imm8);
    } else {
        const word_t operator1_reg = cpu_get_reg(cpu, inst->rn);
        cpu_set_reg(cpu, destination_reg_idx, operator1_reg);
    }
    const word_t destination_reg = cpu_get_reg(cpu, destination_reg_idx);
    cpu_extract_nz(&cpu->cpsr, destination_reg);
}

static inline void add_op(CpuState* cpu, const DecodedDataProcessing* inst) {
    assert(cpu != NULL);
    assert(inst != NULL);

    const register_index_t destination_reg_idx = inst->rd;
    const word_t operator1_reg = cpu_get_reg(cpu, inst->rn);

    if (inst->immediate_mode) {
        const word_t add_result = cpu_get_reg(cpu, operator1_reg) + inst->imm_operand.imm8;
        cpu_set_reg(cpu, destination_reg_idx, add_result);
    } else {
        const word_t operator2_reg = cpu_get_reg(cpu, inst->rn);
        const word_t add_result = cpu_get_reg(cpu, operator1_reg) + cpu_get_reg(cpu, operator2_reg);
        cpu_set_reg(cpu, destination_reg_idx, add_result);
    }

}

static inline void and_op(CpuState* cpu, const DecodedDataProcessing* inst) {

}