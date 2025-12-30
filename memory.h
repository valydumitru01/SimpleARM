//
// Created by valentin on 12/20/25.
//
#pragma once
#include <stddef.h>
#include <stdint.h>

#define MEMORY_SIZE 1024
#define WORD_SIZE 4
#define MEMORY_WORDS_SIZE (MEMORY_SIZE / WORD_SIZE)
typedef struct ProgramMemory {
    /// Pointer to the array of words in memory
    /// Each word is 4 bytes (32 bits)
    uint32_t *words;
    /// Number of words in the memory
    size_t word_count;
} ProgramMemory;

static uint32_t g_mem[MEMORY_WORDS_SIZE];

static inline ProgramMemory construct_memory(void) {
    const ProgramMemory m = { .words = g_mem, .word_count = MEMORY_WORDS_SIZE };
    return m;
}