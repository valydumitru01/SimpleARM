#pragma once
#include <stdint.h>
#define REGISTER_COUNT 16


/// Is of type uint8_t because we have only 16 registers (0-15)
/// And uint8_t is sufficient to index them all
typedef uint8_t register_index_t;
typedef enum RegisterIndex {
    LINK_REGISTER_INDEX = 14,
    PC_REGISTER_INDEX  = 15,
} RegisterIndex;


typedef struct Registers {
    word_t regs[REGISTER_COUNT];
} Registers;

static inline Registers construct_registers() {
    const Registers registers = {.regs = {0}};
    return registers;
}

static inline word_t regs_get(const Registers *registers, const RegisterIndex index) {
    return registers->regs[index];
}

static inline void regs_set(Registers *registers, const RegisterIndex index, const word_t value) {
    registers->regs[index] = value;
}

