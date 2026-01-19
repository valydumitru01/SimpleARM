//
// Created by valentin on 12/24/25.
//
#pragma once

typedef enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_UNKNOWN_OPCODE = -11,
    ERROR_INVALID_REGISTER_INDEX = -12,
} ErrorCode;

typedef struct ExecResult {
    uint32_t next_pc;
    int should_halt;
    ErrorCode error_code;
} ExecResult;
