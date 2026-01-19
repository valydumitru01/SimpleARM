//
// Created by valentin on 12/20/25.
//
#pragma once
#include <stddef.h>
#include <stdint.h>

#include <assert.h>
#include <stdbool.h>

typedef uint8_t byte_t;
typedef uint32_t word_t;
typedef uint16_t halfword_t;

static const size_t BYTE_SIZE_BYTES = 1u;
static const size_t HALFWORD_SIZE_BYTES = 2u;
static const size_t WORD_SIZE_BYTES = 4u;

static const size_t HALFWORD_ALIGN_MASK = HALFWORD_SIZE_BYTES - 1u;
static const size_t WORD_ALIGN_MASK = WORD_SIZE_BYTES - 1u;

static const size_t BYTE_SIZE_BITS = 8u;
static const size_t HALFWORD_SIZE_BITS = BYTE_SIZE_BITS * HALFWORD_SIZE_BYTES;
static const size_t WORD_SIZE_BITS = BYTE_SIZE_BITS * WORD_SIZE_BYTES;



#define MEMORY_SIZE 1024u

typedef struct ProgramMemory {
    /// Byte-addressed memory (ARM memory model is byte-addressable)
    byte_t *bytes;
    /// Number of bytes in memory
    size_t byte_count;
} ProgramMemory;

static byte_t g_mem[MEMORY_SIZE];

static inline ProgramMemory construct_memory(void) {
    const ProgramMemory m = { .bytes = g_mem, .byte_count = MEMORY_SIZE };
    return m;
}
static inline bool mem_in_bounds(const ProgramMemory *m, const word_t addr, const word_t size_bytes) {
    return m && size_bytes <= m->byte_count && addr <= (word_t)m->byte_count - size_bytes;
}

// -------------------------
// Reads (little-endian)
// -------------------------

static inline uint8_t mem_read8(const ProgramMemory *m, const word_t addr) {
    assert(mem_in_bounds(m, addr, BYTE_SIZE_BYTES));
    return m->bytes[addr];
}

static inline uint16_t mem_read16(const ProgramMemory *m, const word_t addr) {
    assert(mem_in_bounds(m, addr, HALFWORD_SIZE_BYTES));
    // Strict alignment check (halfword requires halfword alignment)
    assert((addr & HALFWORD_ALIGN_MASK) == 0);

    const halfword_t b0 = m->bytes[addr + 0];
    const halfword_t b1 = m->bytes[addr + 1];
    return (halfword_t)(b0 | (halfword_t)(b1 << 8));
}

static inline uint32_t mem_read32(const ProgramMemory *m, const word_t addr) {
    assert(mem_in_bounds(m, addr, WORD_SIZE_BYTES));
    // Strict alignment check (word requires word alignment)
    assert((addr & WORD_ALIGN_MASK) == 0);

    const uint32_t b0 = m->bytes[addr + 0];
    const uint32_t b1 = m->bytes[addr + 1];
    const uint32_t b2 = m->bytes[addr + 2];
    const uint32_t b3 = m->bytes[addr + 3];
    return b0 | (b1 << BYTE_SIZE_BITS) | (b2 << BYTE_SIZE_BITS * 2) | (b3 << BYTE_SIZE_BITS * 3);
}

// -------------------------
// Writes (little-endian)
// -------------------------

static inline void mem_write8(const ProgramMemory *m, const word_t addr, const word_t value) {
    assert(mem_in_bounds(m, addr, BYTE_SIZE_BYTES));
    // No alignment check needed for byte writes
    m->bytes[addr + 0] = (byte_t)((value >> BYTE_SIZE_BITS * 0) & 0xFFu);
}

static inline void mem_write16(const ProgramMemory *m, const word_t addr, const halfword_t value) {
    assert(mem_in_bounds(m, addr, HALFWORD_SIZE_BYTES));
    // Strict alignment check (halfword requires halfword alignment)
    assert((addr & HALFWORD_ALIGN_MASK) == 0);

    m->bytes[addr + 0] = (byte_t)((value >> BYTE_SIZE_BITS * 0) & 0xFFu);
    m->bytes[addr + 1] = (byte_t)((value >> BYTE_SIZE_BITS * 1) & 0xFFu);
}

static inline void mem_write32(const ProgramMemory *m, const uint32_t addr, const uint32_t value) {
    assert(mem_in_bounds(m, addr, WORD_SIZE_BYTES));
    // Strict alignment check (word requires word alignment)
    assert((addr & WORD_ALIGN_MASK) == 0);

    m->bytes[addr + 0] = (byte_t)((value >> BYTE_SIZE_BITS * 0) & 0xFFu);
    m->bytes[addr + 1] = (byte_t)((value >> BYTE_SIZE_BITS * 1) & 0xFFu);
    m->bytes[addr + 2] = (byte_t)((value >> BYTE_SIZE_BITS * 2) & 0xFFu);
    m->bytes[addr + 3] = (byte_t)((value >> BYTE_SIZE_BITS * 3) & 0xFFu);

}