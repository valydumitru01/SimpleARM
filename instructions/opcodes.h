//
// Created by valentin on 12/24/25.
//
#pragma once


typedef enum Opcode {
    // Boolean

    /// AND - Bitwise AND.
    /// Semantics: Rd := Rn & Op2
    AND = 0x0,

    /// ORR - Bitwise OR.
    /// Semantics: Rd := Rn | Op2
    ORR,

    /// EOR - Bitwise XOR.
    /// Semantics: Rd := Rn ^ Op2
    EOR,


    // Add

    /// ADD - Add.
    /// Semantics: Rd := Rn + Op2
    ADD,

    /// ADC - Add with carry-in.
    /// Semantics: Rd := Rn + Op2 + C
    ADC,


    // Subtract

    /// SUB - Subtract.
    /// Semantics: Rd := Rn - Op2
    SUB,

    /// SBC - Subtract with carry/borrow.
    /// Semantics: Rd := Rn - Op2 - (1 - C)
    SBC,

    /// RSB - Reverse subtract.
    /// Semantics: Rd := Op2 - Rn
    RSB,

    /// RSC - Reverse subtract with carry/borrow.
    /// Semantics: Rd := Op2 - Rn - (1 - C)
    RSC,


    // Test

    /// TST - Test (AND, flags only; no Rd write).
    /// Semantics: flags := Rn & Op2
    TST,

    /// TEQ - Test equivalence (XOR, flags only; no Rd write).
    /// Semantics: flags := Rn ^ Op2
    TEQ,

    // Compare

    /// CMP - Compare (SUB, flags only; no Rd write).
    /// Semantics: flags := Rn - Op2
    CMP,

    /// CMN - Compare negative (ADD, flags only; no Rd write).
    /// Semantics: flags := Rn + Op2
    CMN,


    /// BIC - Bit clear (AND with inverted operand).
    /// Semantics: Rd := Rn & ~Op2
    BIC,

    // Move

    /// MOV — Move (copy operand).
    /// Semantics: Rd := Op2
    MOV,

    /// MVN - Move NOT (bitwise invert).
    /// Semantics: Rd := ~Op2
    MVN,

    /// No operation
    NOP,

    /// Halt, stops the program
    HALT,
} Opcode;
