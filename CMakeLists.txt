cmake_minimum_required(VERSION 4.1)
project(SimpleARM C)

set(CMAKE_C_STANDARD 11)

include_directories(".")


add_executable(SimpleARM main.c
        cpu/cpu.h
        cpu/cpsr.h
        memory.h
        instructions/instructions_enums.h
        faults/codes.h
        parser.h
        instructions/opcodes.h
        decoder/decoder.h
        executor/executor.h
        executor/instructions/branch/bx.h
        executor/instructions/branch/b_bl.h
        executor/instructions/data_processing/add.h
        executor/instructions/data_processing/subtract.h
        executor/instructions/data_processing/test.h
        executor/instructions/data_processing/compare.h
        executor/instructions/data_processing/bit_clear.h
        executor/instructions/data_processing/move.h
        executor/instructions/multiplication/mul.h
        cpu/cpsr.h
        cpu/cpsr.h
        cpu/cpsr.h
        cpu/registers.h
        executor/instructions/halt.h
        executor/nop.h
        instructions/branch/cond.h
        binary/utils.h
        instructions/data_processing/data_processing_decoder.h
        instructions/data_processing/operations/and.h
        instructions/data_processing/operations/eor.h
        instructions/data_processing/operations/rsb.h
        instructions/data_processing/operations/adc.h
        instructions/data_processing/operations.h
)
