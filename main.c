/// A simple ARM-like CPU simulator in C
/// Instructions supported:
/// - NOP (No Operation)
/// - HALT (Stop Execution)
/// - ADD (Add two registers and store result in a register)
/// - SUB (Subtract two registers and store result in a register)
/// - MOV (Move immediate value to a register)
/// - B (Branch to a specified address)
/// - BEQ (Branch if Equal)
/// - BNE (Branch if Not Equal)
/// - AND (Bitwise AND two registers and store result in a register)
/// - ORR (Bitwise OR two registers and store result in a register)
/// - MUL (Multiply two registers and store result in a register)
/// - LDR (Load from memory to register)
/// - STR (Store from register to memory)
/// - CMP (Compare two registers and set flags)

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "cpu.h"
#include "decoder/decoder.h"
#include "instructions/instructions.h"
#include "faults/codes.h"



static uint32_t fetch(const ProgramMemory *mem, const uint32_t pc, FaultCode *fault_out) {
    // Validate inputs
    assert(mem != NULL);
    assert(mem->words != NULL);
    assert(fault_out != NULL);
    assert(*fault_out == FAULT_NONE);

    // Check for alignment (PC must be multiple of 4)
    if (pc & 3u) {
        *fault_out = FAULT_ALIGNMENT;
        return 0;
    }

    // Shift PC right by 2 (divide by 4) to get word index
    // We do it like this because PC is byte-addressed and
    // memory is word-addressed (4 bytes per word)
    const uint32_t index = pc >> 2;

    // If index is out of bounds, return fault
    if (index >= mem->word_count) {
        *fault_out = FAULT_OUT_OF_BOUNDS;
        return 0;
    }

    return mem->words[index];
}



int main(void) {
    // Initialize memory and registers
    const ProgramMemory memory = construct_memory();

    CpuState cpu = construct_cpu_state();

    cpu_set_pc(&cpu, INITIAL_PC);

    memory.words[0] = ENCODE(OP_MOV, 1, 1, 1);
    memory.words[1] = ENCODE(OP_AND, 1, 1, 1);
    memory.words[2] = ENCODE(OP_MUL, 1, 1, 1);
    memory.words[3] = ENCODE(OP_MOVI, 1, 1, 1);
    memory.words[4] = ENCODE(OP_ADD, 1, 1, 1);
    memory.words[5] = ENCODE(OP_BEQ, 1, 1, 1);
    memory.words[6] = ENCODE(OP_LDR, 1, 1, 1);
    memory.words[7] = ENCODE(OP_ORR, 1, 1, 1);

    FaultCode fault_out = FAULT_NONE;
    do {
        const uint32_t pc = cpu_get_pc(&cpu);
        const uint32_t raw_inst = fetch(&memory, pc, &fault_out);
        const DecodedInst inst = decode(raw_inst, &fault_out);
        const ExecResult result = execute_instruction(&cpu, &inst);
        if (result.error_code) return result.error_code;
        if (result.should_halt) break;
        cpu_set_pc(&cpu, result.next_pc);
    }
    while (fault_out == FAULT_NONE);
    return 0;
}
