cmake_minimum_required(VERSION 4.1)
project(SimpleARM C)

set(CMAKE_C_STANDARD 11)

add_executable(SimpleARM main.c
        cpu.h
        memory.h
        instructions/instructions.h
        faults/codes.h
        parser.h
        instructions/opcodes.h
        instructions/instruction_impls.h
        decoder/decoder.h
        decoder/decoded_inst.h
        executor/executor.h
        executor/instruction_impls/branch/bx.h
        executor/instruction_impls/branch/b_bl.h
        executor/instruction_impls/data_processing/add.h
        executor/instruction_impls/data_processing/subtract.h
        executor/instruction_impls/data_processing/test.h
        executor/instruction_impls/data_processing/compare.h
        executor/instruction_impls/data_processing/bit_clear.h
        executor/instruction_impls/data_processing/move.h
        executor/instruction_impls/multiplication/mul_32.h
        executor/instruction_impls/multiplication/mul_64.h
)
