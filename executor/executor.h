//
// Created by valentin on 12/24/25.
//
#pragma once



typedef enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_UNKNOWN_OPCODE = -1,
    ERROR_INVALID_REGISTER_INDEX = -2,
    WARNING_PC_USED_AS_OPERAND = -999,
} ErrorCode;

static const char* WARNING_PC_USED_AS_OPERAND_MESSAGE = "Program Counter Register (R15) used as operand register, which is undefined behavior.";


typedef struct ExecResult {
    uint32_t next_pc;
    int should_halt;
    ErrorCode error_code;
} ExecResult;
