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

static inline void mov_op(CpuState* cpu, const register_index_t reg, const word_t value, bool rises_cpsr) {
    cpu_set_reg(cpu, reg, value);

    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, value);
    cpsr_extract_negative(&cpu->cpsr, value);
}

static inline void and_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t result = operand1 & operand2;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
}

static inline void eor_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t result = operand1 ^ operand2;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
}

static inline void sub_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t result = operand1 - operand2;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_sub(&cpu->cpsr, operand1, operand2);
    cpsr_extract_overflow_sub(&cpu->cpsr, operand1, operand2, result);
}

static inline void rsb_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t result = operand2 - operand1;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_sub(&cpu->cpsr, operand2, operand1); // Reversed operands
    cpsr_extract_overflow_sub(&cpu->cpsr, operand2, operand1, result); // Reversed operands
}

static inline void add_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t result = operand1 + operand2;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_add(&cpu->cpsr, operand2, operand1);
    cpsr_extract_overflow_add(&cpu->cpsr, operand1, operand2, result);

}

static inline void adc_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t carry = cpsr_get_carry(&cpu->cpsr) ? 1 : 0;
    const word_t operand2_with_carry = operand2 + carry;
    const word_t result = operand1 + operand2_with_carry;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_add(&cpu->cpsr, operand1, operand2_with_carry);
    cpsr_extract_overflow_add(&cpu->cpsr, operand1, operand2_with_carry, result);
}

static inline void sbc_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t carry = cpsr_get_carry(&cpu->cpsr) ? 1 : 0;
    const word_t operand2_with_carry = operand2 - carry;
    const word_t result = operand1 - operand2_with_carry;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_sub(&cpu->cpsr, operand1, operand2_with_carry);
    cpsr_extract_overflow_sub(&cpu->cpsr, operand1, operand2_with_carry, result);

}

static inline void rsc_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2, bool rises_cpsr) {
    // Operation
    const word_t carry = cpsr_get_carry(&cpu->cpsr) ? 1 : 0;
    const word_t operand1_with_carry = operand1 + carry;
    const word_t result = operand2 - operand1_with_carry;
    cpu_set_reg(cpu, reg, result);

    // CPSR updates
    if (!rises_cpsr) return;
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    cpsr_extract_carry_sub(&cpu->cpsr, operand2, operand1_with_carry); // Reversed operands
    cpsr_extract_overflow_sub(&cpu->cpsr, operand2, operand1_with_carry, result); // Reversed operands

}

static inline void tst_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    // Operation
    // No storing of the result
    const word_t result = operand1 & operand2;

    // CPSR updates
    // We update the flags no matter the S flag
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
}
static inline void teq_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    // Operation
    // No storing of the result
    const word_t result = operand1 ^ operand2;

    // CPSR updates
    // We update the flags no matter the S flag
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
}
static inline void cmp_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    // Operation
    // No storing of the result
    const word_t result = operand1 - operand2;

    // CPSR updates
    // We update the flags no matter the S flag
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
    //

}
static inline void cmn_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    // Operation
    // No storing of the result
    const word_t result = operand1 + operand2;

    // CPSR updates
    // We update the flags no matter the S flag
    cpsr_extract_zero(&cpu->cpsr, result);
    cpsr_extract_negative(&cpu->cpsr, result);
}
static inline void orr_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    const word_t result = operand1 | operand2;
    cpu_set_reg(cpu, reg, result);
}
static inline void bic_op(CpuState* cpu, const register_index_t reg, const word_t operand1, const word_t operand2) {
    const word_t result = operand1 & ~operand2;
    cpu_set_reg(cpu, reg, result);
}
static inline void mvn_op(CpuState* cpu, const register_index_t reg, const word_t operand2, bool rises_cpsr) {
    const word_t result = ~operand2;
    cpu_set_reg(cpu, reg, result);
}