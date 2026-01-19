//
// Created by valentin on 12/20/25.
//
#pragma once

typedef enum ReturnStatus {
    OK = 0,
    FAULT,
    ERROR,
    HALT,
} ReturnStatus;

typedef enum FaultCodeFetch {
    FAULT_ALIGNMENT,
} FaultCodeFetch;

typedef enum FaultCodeDecode {
    FAULT_INVALID_OPCODE,
} FaultCodeDecode;

typedef enum FaultCodeExecute {
    FAULT_OUT_OF_BOUNDS,
} FaultCodeExecute;

