#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "cpu/cpu.h"
#include "executor/executor.h"

typedef enum CondCode {
    /// Set Flags: Z set
    ///
    /// Meaning: equal
    EQ = 0x0000,
    /// Set Flags: Z clear not
    ///
    /// Meaning: equal
    NE = 0x0001,
    /// Set Flags: C set unsigned
    ///
    /// Meaning: higher or same
    CS = 0x0010,
    /// Set Flags: C clear unsigned
    ///
    /// Meaning: lower
    CC = 0x0011,
    /// Set Flags: N set
    ///
    /// Meaning: negative
    MI = 0x0100,
    /// Set Flags: N clear
    ///
    /// Meaning: positive or zero
    PL = 0x0101,
    /// Set Flags: V set
    /// Meaning: overflow
    VS = 0x0110,
    /// Set Flags: V clear
    ///
    /// Meaning: no overflow
    VC = 0x0111,
    /// Set Flags: C set and Z clear unsigned
    ///
    /// Meaning: higher
    HI = 0x1000,
    /// Set Flags: C clear or Z set unsigned
    ///
    /// Meaning: lower or same
    LS = 0x1001,
    /// Set Flags: N equals V
    ///
    /// Meaning: greater or equal
    GE = 0x1010,
    /// Set Flags: N not equal to V
    ///
    /// Meaning: less than
    LT = 0x1011,
    /// Set Flags: Z clear AND (N equals V)
    ///
    /// Meaning: greater than
    GT = 0x1100,
    /// Set Flags: Z set OR (N not equal to V)
    ///
    /// Meaning: less than or equal
    LE = 0x1101,
    /// Meaning: (ignored) always
    AL = 0x1110,
} CondCode;


static inline bool cond_passed(const CondCode cond, const Cpsr* cpsr) {
    const uint32_t N = cpsr_get_negative(cpsr);
    const uint32_t Z = cpsr_get_zero(cpsr);
    const uint32_t C = cpsr_get_carry(cpsr);
    const uint32_t V = cpsr_get_overflow(cpsr);

    // Compute common composites once (keeps cases simple)
    const uint32_t IS_GE = N == V; // signed >= (N == V)

    switch (cond) {
    case EQ: return Z;              // EQ: Z==1
    case NE: return !Z;             // NE: Z==0
    case CS: return C;              // CS/HS: C==1
    case CC: return !C;             // CC/LO: C==0
    case MI: return N;              // MI: N==1
    case PL: return !N;             // PL: N==0
    case VS: return V;              // VS: V==1
    case VC: return !V;             // VC: V==0
    case HI: return C && !Z;        // HI: C==1 && Z==0
    case LS: return !C || Z;        // LS: C==0 || Z==1
    case GE: return IS_GE;          // GE: N==V
    case LT: return !IS_GE;         // LT: N!=V
    case GT: return !Z && IS_GE;    // GT: Z==0 && N==V
    case LE: return Z || !IS_GE;    // LE: Z==1 || N!=V
    case AL: return true;           // AL: always
    default:
        assert(false && "Unexpected/unsupported condition");
        return false; // unreachable
    }
}