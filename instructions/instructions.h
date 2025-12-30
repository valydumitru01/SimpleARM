//
// Created by valentin on 12/20/25.
//
#pragma once
typedef enum InstructionType {
    /// ###################################
    ///            Data Processing
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:26] [25]   [24:21]  [20] [19:16] [15:12]   [11:0]
    ///  Cond     00      I     OpCode   S     Rn      Rd     Operand2
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 00 (bits 27:26)
    ///    Class selector for data-processing / PSR-transfer encodings (must be 00).
    ///
    /// 3) I (bit 25)
    ///    Operand2 type selector:
    ///      0 = Operand2 is a shifted register (Rm with optional shift)
    ///      1 = Operand2 is an immediate (8-bit Imm rotated right by 2*Rotate)
    ///
    /// 4) OpCode (bits 24:21)
    ///    ALU operation selector (AND/EOR/SUB/.../MOV/MVN etc.).
    ///
    /// 5) S (bit 20)
    ///    If set, updates CPSR flags from the result (see per-op rules).
    ///    For TST/TEQ/CMP/CMN, S is effectively required and Rd is not written.
    ///
    /// 6) Rn (bits 19:16)
    ///    First operand register (Op1). For MOV/MVN, Op1 is ignored.
    ///
    /// 7) Rd (bits 15:12)
    ///    Destination register. Not written by TST/TEQ/CMP/CMN.
    ///
    /// 8) Operand2 (bits 11:0) - “flexible second operand”
    ///    If I=0 (register form):
    ///      - Rm in bits [3:0]
    ///      - Shift type in bits [6:5]
    ///      - Shift amount is either:
    ///          * immediate (bits [11:7]) when bit[4]=0, or
    ///          * register Rs (bits [11:8]) when bit[4]=1 (and bit[7]=0)
    ///    If I=1 (immediate form):
    ///      - Imm8 in bits [7:0]
    ///      - Rotate in bits [11:8] (immediate value is ROR by 2*Rotate)
    ///
    /// Semantics (conceptually):
    ///   result := Op(Rn, Operand2)   // except MOV/MVN ignore Rn
    ///   if (writes_result) Rd := result
    ///   if (S) update CPSR flags as defined for the operation class
    DATA_PROCESSING,
    /// ###################################
    ///   Multiply / Multiply-Accumulate
    ///            (MUL, MLA)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:22] [21] [20] [19:16] [15:12] [11:8] [7:4] [3:0]
    ///  Cond   000000    A    S     Rd      Rn      Rs   1001   Rm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 000000 (bits 27:22)
    ///    Class selector for multiply encodings (must be 000000).
    ///
    /// 3) A (bit 21) - Accumulate selector
    ///    0 = MUL  (multiply only)
    ///    1 = MLA  (multiply and accumulate)
    ///
    /// 4) S (bit 20) - Flag update
    ///    If set, updates CPSR N and Z from the 32-bit result.
    ///    C becomes meaningless; V is unaffected.
    ///
    /// 5) Rd (bits 19:16)
    ///    Destination register (low 32 bits of the product / sum).
    ///
    /// 6) Rn (bits 15:12)
    ///    Accumulator input for MLA.
    ///    For MUL (A=0), Rn is ignored and should be 0 for compatibility.
    ///
    /// 7) Rs (bits 11:8)
    ///    Multiplier operand register.
    ///
    /// 8) 1001 (bits 7:4)
    ///    Fixed pattern identifying multiply instructions.
    ///
    /// 9) Rm (bits 3:0)
    ///    Multiplicand operand register.
    ///
    /// Semantics:
    ///   if (A==0) {            // MUL
    ///     Rd := (Rm * Rs) low32;        // Rn ignored
    ///   } else {               // MLA
    ///     Rd := ((Rm * Rs) + Rn) low32;
    ///   }
    ///
    /// Operand restrictions (ARM7TDMI):
    ///   - Rd must not be the same as Rm
    ///   - R15 must not be used as operand or destination
    MUL,
    /// ###################################
    ///  Multiply Long / Multiply-Accumulate Long
    ///     (UMULL/SMULL, UMLAL/SMLAL)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:8] [7:4] [3:0]
    ///  Cond     000     01    U    A    S    RdHi    RdLo     Rs   1001   Rm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 00001 (bits 27:23)
    ///    Class selector for multiply-long encodings (must match 00001).
    ///
    /// 3) U (bit 22) - Unsigned/Signed
    ///    0 = unsigned (UMULL / UMLAL)
    ///    1 = signed   (SMULL / SMLAL)
    ///
    /// 4) A (bit 21) - Accumulate
    ///    0 = multiply only
    ///    1 = multiply and accumulate (add 64-bit accumulator)
    ///
    /// 5) S (bit 20) - Flag update
    ///    If set, updates CPSR N and Z from the 64-bit result.
    ///    C and V become meaningless.
    ///
    /// 6) RdHi (bits 19:16)
    ///    Upper 32 bits destination register.
    ///
    /// 7) RdLo (bits 15:12)
    ///    Lower 32 bits destination register.
    ///
    /// 8) Rs (bits 11:8)
    ///    Multiplier operand register.
    ///
    /// 9) 1001 (bits 7:4)
    ///    Fixed pattern identifying multiply instructions.
    ///
    /// 10) Rm (bits 3:0)
    ///     Multiplicand operand register.
    ///
    /// Semantics:
    ///   // form 64-bit operands as signed or unsigned depending on U
    ///   prod64 = Mul64( Rm, Rs, signed = (U==1) )
    ///
    ///   if (A==0) {                 // UMULL / SMULL
    ///     result64 = prod64
    ///   } else {                    // UMLAL / SMLAL
    ///     acc64    = (uint64)(RdHi:RdLo)   // concatenation of current dest regs
    ///     result64 = prod64 + acc64
    ///   }
    ///
    ///   RdLo := (uint32)(result64 & 0xFFFFFFFF)
    ///   RdHi := (uint32)(result64 >> 32)
    ///
    /// Flags (if S==1):
    ///   N = result64[63]
    ///   Z = (result64 == 0)
    ///   C,V = meaningless
    ///
    /// Operand restrictions (ARM7TDMI-class):
    ///   - R15 must not be used as operand or destination.
    ///   - RdHi, RdLo, and Rm must all be different registers.
    MULL,
    /// ###################################
    ///     Branch / Branch with Link
    ///              (B, BL)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23:0]
    ///  Cond     101     L   imm24
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 101 (bits 27:25)
    ///    Class selector for branch encodings (must be 101).
    ///
    /// 3) L (bit 24) - Link bit
    ///    0 = B   (no link)
    ///    1 = BL  (write return address to LR / R14)
    ///
    /// 4) imm24 (bits 23:0)
    ///    Signed 24-bit 2’s-complement PC-relative offset.
    ///    Effective offset is (SignExtend(imm24) << 2).
    ///
    /// Semantics (conceptually):
    ///   next_pc = PC + 8 + (SignExtend(imm24) << 2)   // PC is prefetch-biased in ARM state
    ///   if (L==1) LR := (PC + 4)                      // address of the next instruction
    ///   PC := next_pc
    ///
    /// Notes:
    ///   - Branch range is approximately +/- 32 MiB (because imm24<<2).
    ///   - Branch updates PC and flushes/refills the pipeline.
    ///   - BL does not save CPSR; LR[1:0] are cleared (word-aligned return address).
    BRANCH,
    /// ###################################
    ///        Branch and Exchange
    ///                (BX)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:4]                 [3:0]
    ///  Cond   000100101111111111110001   Rn
    ///
    /// Equivalent bitfield view:
    /// [31:28] [27:25] [24:21] [20] [19:16] [15:12] [11:8]  [7:4]  [3:0]
    ///  Cond     000     1001   0     1111    1111    1111   0001    Rn
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) Fixed pattern (bits 27:4)
    ///    Must match the BX encoding constant shown above.
    ///
    /// 3) Rn (bits 3:0)
    ///    Operand register containing the branch target address.
    ///
    /// Semantics:
    ///   PC := Rn
    ///   if (Rn[0] == 1) next state = Thumb
    ///   else           next state = ARM
    ///
    /// Notes / restrictions:
    ///   - Causes a pipeline flush and refill from the target.
    ///   - Using R15 as the operand register is undefined.
    BRANCH_AND_EXCHANGE,
    /// ###################################
    ///        Single Data Transfer
    ///            (LDR, STR)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:26] [25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:0]
    ///  Cond     01      I    P    U    B    W    L     Rn      Rd     Offset12
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 01 (bits 27:26)
    ///    Class selector for single data transfer (must be 01).
    ///
    /// 3) I (bit 25) - Offset kind
    ///    0 = immediate offset (unsigned 12-bit)
    ///    1 = register offset (Rm with shift-by-immediate)
    ///
    /// 4) P (bit 24) - Pre/Post indexing
    ///    1 = pre-index:  addr = Rn +/- offset; transfer uses addr
    ///    0 = post-index: addr = Rn; transfer uses addr; then base updated
    ///
    /// 5) U (bit 23) - Up/Down
    ///    1 = add offset
    ///    0 = subtract offset
    ///
    /// 6) B (bit 22) - Byte/Word
    ///    0 = word (32-bit)
    ///    1 = byte (8-bit)
    ///
    /// 7) W (bit 21) - Write-back
    ///    1 = write updated address back into Rn
    ///    0 = do not write back
    ///    Note: for post-index (P=0), the base update happens after the transfer; in most
    ///          assemblers W is implied/required in that form.
    ///
    /// 8) L (bit 20) - Load/Store
    ///    1 = LDR  (load memory -> Rd)
    ///    0 = STR  (store Rd -> memory)
    ///
    /// 9) Rn (bits 19:16)
    ///    Base register.
    ///
    /// 10) Rd (bits 15:12)
    ///    Transfer register:
    ///      - LDR: destination
    ///      - STR: source
    ///
    /// 11) Offset12 (bits 11:0) - offset encoding
    ///    If I=0 (immediate):
    ///      offset = Unsigned12(Offset12)
    ///    If I=1 (register with shift-by-immediate):
    ///      bits[3:0]   = Rm
    ///      bits[4]     = 0 (shift amount is immediate, not register)
    ///      bits[6:5]   = shift type (LSL/LSR/ASR/ROR)
    ///      bits[11:7]  = imm5 shift amount
    ///      offset = Shift(Rm, shift_type, imm5)
    ///
    /// Addressing semantics (conceptual):
    ///   offset = (I==0) ? imm12 : Shift(Rm, shift_type, imm5)
    ///   delta  = (U==1) ? +offset : -offset
    ///
    ///   if (P==1) { addr = Rn + delta; if (W) Rn = addr; }
    ///   else      { addr = Rn;        Rn = Rn + delta;      }
    ///
    ///   if (L==1) Rd = Mem[addr] (byte/word per B)
    ///   else      Mem[addr] = Rd (byte/word per B)
    ///
    /// R15 (PC) notes (common emulator gotchas):
    ///   - If Rn == R15, the base value used is PC + 8 (ARM-state prefetch bias).
    ///   - R15 must not be used as the register offset (Rm).
    ///   - If Rd == R15 and L==1 (LDR PC), this is a control-flow change (pipeline flush).
    SINGLE_DATA_TRANSFER,
    /// ###################################
    ///        Block Data Transfer
    ///             (LDM, STM)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:0]
    ///  Cond     100     P    U    S    W    L     Rn    RegisterList
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 100 (bits 27:25)
    ///    Class selector for block data transfer instructions.
    ///
    /// 3) P (bit 24) - Pre/Post indexing
    ///    1 = pre-index
    ///    0 = post-index
    ///
    /// 4) U (bit 23) - Up/Down
    ///    1 = increment address
    ///    0 = decrement address
    ///
    /// 5) S (bit 22) - PSR / user-bank transfer
    ///    Meaning depends on L and whether R15 is in the register list:
    ///      - LDM + R15 in list + S=1: CPSR := SPSR_<mode> (mode/flag restore)
    ///      - STM + S=1: transfer user-bank registers (privileged modes only)
    ///      - Otherwise: transfer user-bank registers instead of current mode bank
    ///
    /// 6) W (bit 21) - Write-back
    ///    1 = write final address back into Rn
    ///    0 = do not write back
    ///
    /// 7) L (bit 20) - Load/Store
    ///    1 = LDM (load multiple from memory)
    ///    0 = STM (store multiple to memory)
    ///
    /// 8) Rn (bits 19:16)
    ///    Base register.
    ///
    /// 9) RegisterList (bits 15:0)
    ///    Bitmask of registers to transfer:
    ///      bit 0  -> R0
    ///      bit 1  -> R1
    ///      ...
    ///      bit 15 -> R15
    ///
    /// Transfer order & address sequencing:
    ///   - Registers are transferred in increasing register number order.
    ///   - Lowest register uses the lowest memory address.
    ///   - Address sequencing is determined by (P, U) combination.
    ///
    /// Address update semantics (conceptual):
    ///   n = popcount(RegisterList)
    ///
    ///   if (U==1) {
    ///     start = Rn + (P ? 4 : 0)
    ///     end   = Rn + (4*n) - (P ? 0 : 4)
    ///   } else {
    ///     start = Rn - (4*n) + (P ? 4 : 0)
    ///     end   = Rn - (P ? 0 : 4)
    ///   }
    ///
    ///   if (W==1) Rn := (U==1) ? (Rn + 4*n) : (Rn - 4*n)
    ///
    /// Notes / restrictions:
    ///   - RegisterList must not be empty.
    ///   - R15 must not be used as the base register (Rn).
    ///   - If R15 is loaded, this is a control-flow change (pipeline flush).
    ///   - Using write-back with user-bank transfers requires care in emulation.
    BLOCK_DATA_TRANSFER,

} InstructionType;

typedef enum Instruction {
    /// ADD - Add two values.
    /// Use: basic arithmetic, pointer/index math, building addresses.
    /// Semantics: Rd := Rn + Op2
    /// Example:   R0 := R1 + R2
    ADD = 0x0,

    /// ADC - Add with carry-in.
    /// Use: multi-word (64-bit+) addition, checksums, big integer math.
    /// Semantics: Rd := Rn + Op2 + C
    /// Example:   R0 := R1 + R2 + C
    ADC,

    /// SUB - Subtract.
    /// Use: arithmetic, comparisons via flags (when S=1), pointer/index math.
    /// Semantics: Rd := Rn - Op2
    /// Example:   R0 := R1 - R2
    SUB,

    /// SBC - Subtract with carry/borrow.
    /// Use: multi-word (64-bit+) subtraction, big integer math.
    /// Semantics: Rd := Rn - Op2 - (1 - C)
    /// Example:   R0 := R1 - R2 - (1 - C)
    SBC,

    /// MUL - Multiply two registers (low 32-bit result).
    /// Use: scaling, fixed-point math, array indexing with strides.
    /// Semantics: Rd := (Rm * Rs) low32
    /// Example:   R0 := R1 * R2
    MUL,

    /// MLA - Multiply then add accumulator (low 32-bit result).
    /// Use: dot-products, MAC operations, polynomial evaluation.
    /// Semantics: Rd := ((Rm * Rs) + Rn) low32
    /// Example:   R0 := (R1 * R2) + R3
    MLA,


    /// ###################################
    ///  Multiply Long / Multiply-Accumulate Long
    ///     (UMULL/SMULL, UMLAL/SMLAL)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:8] [7:4] [3:0]
    ///  Cond     000     01    U    A    S    RdHi    RdLo     Rs   1001   Rm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 00001 (bits 27:23)
    ///    Class selector for multiply-long encodings (must match 00001).
    ///
    /// 3) U (bit 22) - Unsigned/Signed
    ///    0 = unsigned (UMULL / UMLAL)
    ///    1 = signed   (SMULL / SMLAL)
    ///
    /// 4) A (bit 21) - Accumulate
    ///    0 = multiply only
    ///    1 = multiply and accumulate (add 64-bit accumulator)
    ///
    /// 5) S (bit 20) - Flag update
    ///    If set, updates CPSR N and Z from the 64-bit result.
    ///    C and V become meaningless.
    ///
    /// 6) RdHi (bits 19:16)
    ///    Upper 32 bits destination register.
    ///
    /// 7) RdLo (bits 15:12)
    ///    Lower 32 bits destination register.
    ///
    /// 8) Rs (bits 11:8)
    ///    Multiplier operand register.
    ///
    /// 9) 1001 (bits 7:4)
    ///    Fixed pattern identifying multiply instructions.
    ///
    /// 10) Rm (bits 3:0)
    ///     Multiplicand operand register.
    ///
    /// Semantics:
    ///   // form 64-bit operands as signed or unsigned depending on U
    ///   prod64 = Mul64( Rm, Rs, signed = (U==1) )
    ///
    ///   if (A==0) {                 // UMULL / SMULL
    ///     result64 = prod64
    ///   } else {                    // UMLAL / SMLAL
    ///     acc64    = (uint64)(RdHi:RdLo)   // concatenation of current dest regs
    ///     result64 = prod64 + acc64
    ///   }
    ///
    ///   RdLo := (uint32)(result64 & 0xFFFFFFFF)
    ///   RdHi := (uint32)(result64 >> 32)
    ///
    /// Flags (if S==1):
    ///   N = result64[63]
    ///   Z = (result64 == 0)
    ///   C,V = meaningless
    ///
    /// Operand restrictions (ARM7TDMI-class):
    ///   - R15 must not be used as operand or destination.
    ///   - RdHi, RdLo, and Rm must all be different registers.

    /// UMULL - Unsigned 32x32 -> 64 multiply (writes RdHi:RdLo).
    /// Use: full-precision products, 64-bit math on 32-bit cores.
    /// Semantics: (RdHi:RdLo) := (uint64)Rm * (uint64)Rs
    /// Example:   (R1:R0) := (uint64)R2 * (uint64)R3
    UMULL,

    /// SMULL - Signed 32x32 -> 64 multiply (writes RdHi:RdLo).
    /// Use: signed 64-bit products, fixed-point math.
    /// Semantics: (RdHi:RdLo) := (int64)Rm * (int64)Rs
    /// Example:   (R1:R0) := (int64)R2 * (int64)R3
    SMULL,

    /// UMLAL - Unsigned long multiply-accumulate into 64-bit accumulator.
    /// Use: wide dot-products, big integer multiply-add loops.
    /// Semantics: (RdHi:RdLo) := (RdHi:RdLo) + (uint64)Rm * (uint64)Rs
    /// Example:   (R1:R0) := (R1:R0) + (uint64)R2 * (uint64)R3
    UMLAL,

    /// SMLAL - Signed long multiply-accumulate into 64-bit accumulator.
    /// Use: signed wide MAC, fixed-point filters.
    /// Semantics: (RdHi:RdLo) := (RdHi:RdLo) + (int64)Rm * (int64)Rs
    /// Example:   (R1:R0) := (R1:R0) + (int64)R2 * (int64)R3
    SMLAL,


    /// B - Branch (jump) to target.
    /// Use: if/else, loops, unconditional jumps.
    /// Semantics: PC := target
    /// Example:   PC := label
    B,

    /// BL - Branch and save return address in LR.
    /// Use: function calls.
    /// Semantics: LR := return_addr; PC := target
    /// Example:   LR := PC_next; PC := function
    BL,


    /// BX - Branch to register and optionally switch ARM/Thumb state.
    /// Use: returns (BX LR), indirect calls/jumps, mode switches (ARM<->Thumb).
    /// Semantics: PC := Rn; T := Rn[0]
    /// Example:   PC := LR; T := LR[0]
    BX,

    /// LDR - Load a word/byte from memory into a register.
    /// Use: reading variables, pointers, struct fields, array elements.
    /// Semantics: Rd := Mem[addr]
    /// Example:   R0 := Mem[R1 + 4]
    LDR,

    /// STR - Store a word/byte from a register into memory.
    /// Use: writing variables, pointers, struct fields, array elements.
    /// Semantics: Mem[addr] := Rd
    /// Example:   Mem[R1 + 4] := R0
    STR,


    /// LDM - Load multiple registers from memory (block load).
    /// Use: function epilogue/restore, fast memcpy-like loads, context restore.
    /// Semantics: {regs} := MemBlock[addr..]
    /// Example:   {R4,R5,LR} := MemBlock[SP..]; SP := SP + 12
    LDM,

    /// STM - Store multiple registers to memory (block store).
    /// Use: function prologue/save, fast structure saves, context save.
    /// Semantics: MemBlock[addr..] := {regs}
    /// Example:   MemBlock[SP-12..] := {R4,R5,LR}; SP := SP - 12
    STM,

    /// ###################################
    ///   Halfword & Signed Data Transfer
    ///   (LDRH/STRH/LDRSB/LDRSH)
    /// ###################################
    ///
    /// Two encodings exist: register offset and immediate offset.
    /// Both use the distinctive ".... 0000 .... 1 S H 1 ...." pattern.
    ///
    /// ---------------------------
    /// A) Register offset form
    /// ---------------------------
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:8] [7] [6] [5] [4] [3:0]
    ///  Cond     000     P    U    0    W    L     Rn      Rd    0000   1   S   H   1    Rm
    ///
    /// ---------------------------
    /// B) Immediate offset form
    /// ---------------------------
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:8] [7] [6] [5] [4] [3:0]
    ///  Cond     000     P    U    1    W    L     Rn      Rd    imm4H  1   S   H   1   imm4L
    ///
    /// Immediate offset assembly:
    ///   offset8 = (imm4H << 4) | imm4L     // 8-bit unsigned byte offset
    ///
    /// Field meanings (common):
    /// 1) Cond (bits 31:28)
    ///    Condition field.
    ///
    /// 2) P (bit 24) - Pre/Post indexing
    ///    1 = pre-index, 0 = post-index.
    ///
    /// 3) U (bit 23) - Up/Down
    ///    1 = add offset, 0 = subtract offset.
    ///
    /// 4) bit22 selects immediate vs register offset:
    ///    0 = register offset form (Rm)
    ///    1 = immediate offset form (offset8)
    ///
    /// 5) W (bit 21) - Write-back
    ///    1 = write updated address back into Rn.
    ///
    /// 6) L (bit 20) - Load/Store
    ///    1 = load (LDRH/LDRSB/LDRSH)
    ///    0 = store (STRH)  // stores are only valid for unsigned halfword
    ///
    /// 7) Rn (bits 19:16)
    ///    Base register.
    ///
    /// 8) Rd (bits 15:12)
    ///    Transfer register (dest for loads, source for stores).
    ///
    /// 9) S,H (bits 6:5) - Transfer type selector
    ///    S=0, H=1 : LDRH / STRH   (unsigned halfword)
    ///    S=1, H=0 : LDRSB         (signed byte, sign-extend to 32)
    ///    S=1, H=1 : LDRSH         (signed halfword, sign-extend to 32)
    ///    S=0, H=0 : reserved/other encoding space (not these ops)
    ///
    /// Addressing semantics (conceptual):
    ///   offset = (imm-form) ? offset8 : Rm
    ///   delta  = (U==1) ? +offset : -offset
    ///
    ///   if (P==1) { addr = Rn + delta; if (W) Rn = addr; }
    ///   else      { addr = Rn;        Rn = Rn + delta;     }
    ///
    ///   if (L==1) {
    ///     if (S==0 && H==1) Rd = ZeroExtend16(Mem16[addr])        // LDRH
    ///     if (S==1 && H==0) Rd = SignExtend8 (Mem8 [addr])        // LDRSB
    ///     if (S==1 && H==1) Rd = SignExtend16(Mem16[addr])        // LDRSH
    ///   } else {
    ///     // store
    ///     // valid store case is unsigned halfword (S=0,H=1)
    ///     Mem16[addr] = Rd[15:0]                                   // STRH
    ///   }
    ///
    /// Notes / restrictions (good emulator checks):
    ///   - L must be 1 for signed variants (S=1); storing with S=1 is invalid.
    ///   - Address must be halfword-aligned for halfword transfers; misalignment yields
    ///     unpredictable behavior on ARM7TDMI-class cores.
    ///   - R15 handling is special in general (avoid using PC as base/offset unless you
    ///     emulate the documented PC+8 behavior and restrictions).

    /// LDRH - Load unsigned 16-bit halfword (zero-extend to 32-bit).
    /// Use: reading 16-bit data (UTF-16, shorts, packed fields).
    /// Semantics: Rd := ZeroExtend16(Mem16[addr])
    /// Example:   R0 := ZeroExtend16(Mem16[R1 + 2])
    LDRH,

    /// STRH - Store 16-bit halfword (low 16 bits).
    /// Use: writing 16-bit data, packed structures.
    /// Semantics: Mem16[addr] := Rd[15:0]
    /// Example:   Mem16[R1 + 2] := R0[15:0]
    STRH,

    /// LDRSB - Load signed 8-bit byte (sign-extend to 32-bit).
    /// Use: reading int8_t/char data where sign matters.
    /// Semantics: Rd := SignExtend8(Mem8[addr])
    /// Example:   R0 := SignExtend8(Mem8[R1])
    LDRSB,

    /// LDRSH - Load signed 16-bit halfword (sign-extend to 32-bit).
    /// Use: reading int16_t data where sign matters.
    /// Semantics: Rd := SignExtend16(Mem16[addr])
    /// Example:   R0 := SignExtend16(Mem16[R1])
    LDRSH,

    /// ###################################
    ///          Single Data Swap
    ///                (SWP)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:23] [22] [21:20] [19:16] [15:12] [11:8] [7:4] [3:0]
    ///  Cond    00010    B     00       Rn      Rd     0000   1001    Rm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 00010 (bits 27:23)
    ///    Class selector for single data swap instructions.
    ///
    /// 3) B (bit 22) - Byte/Word
    ///    0 = swap word (32-bit)
    ///    1 = swap byte (8-bit)
    ///
    /// 4) 00 (bits 21:20)
    ///    Must be 00 for SWP encodings.
    ///
    /// 5) Rn (bits 19:16)
    ///    Base register holding the memory address.
    ///
    /// 6) Rd (bits 15:12)
    ///    Destination register (receives the old memory value).
    ///
    /// 7) 0000 (bits 11:8)
    ///    Must be zero.
    ///
    /// 8) 1001 (bits 7:4)
    ///    Fixed pattern identifying swap instructions.
    ///
    /// 9) Rm (bits 3:0)
    ///    Source register (value written to memory).
    ///
    /// Semantics (atomic, conceptual):
    ///   tmp := Mem[Rn]           // byte or word depending on B
    ///   Mem[Rn] := Rm
    ///   Rd := tmp
    ///
    /// Notes / restrictions:
    ///   - Implemented as a locked read followed by a locked write (atomic w.r.t. interrupts).
    ///   - R15 must not be used as Rd, Rn, or Rm.
    ///   - Rd and Rm may be the same register.
    ///   - Used primarily for software semaphores on ARMv4 systems.

    /// SWP - Atomic swap between register and memory (word/byte).
    /// Use: simple locks/semaphores on older ARM (pre-LL/SC), atomic exchange.
    /// Semantics: tmp := Mem[addr]; Mem[addr] := Rm; Rd := tmp
    /// Example:   R0 := Mem[R1]; Mem[R1] := R2
    SWP,

    /// ###################################
    ///     Coprocessor Data Operation
    ///                (CDP)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:24] [23:20] [19:16] [15:12] [11:8] [7:5] [4] [3:0]
    ///  Cond     1110     Op1      CRn      CRd     CP#    Op2   0    CRm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 1110 (bits 27:24)
    ///    Class selector for coprocessor data operations.
    ///
    /// 3) Op1 (bits 23:20)
    ///    Coprocessor-specific operation code (primary opcode).
    ///
    /// 4) CRn (bits 19:16)
    ///    First coprocessor operand register.
    ///
    /// 5) CRd (bits 15:12)
    ///    Coprocessor destination register.
    ///
    /// 6) CP# (bits 11:8)
    ///    Coprocessor number (0–15). Instruction is ignored by other coprocessors.
    ///
    /// 7) Op2 (bits 7:5)
    ///    Coprocessor-specific secondary opcode.
    ///
    /// 8) 0 (bit 4)
    ///    Must be zero for CDP encodings.
    ///
    /// 9) CRm (bits 3:0)
    ///    Second coprocessor operand register.
    ///
    /// Semantics:
    ///   Request coprocessor CP# to perform an internal operation specified by
    ///   (Op1, Op2) on operands CRn and CRm, and place the result in CRd.
    ///
    /// Notes:
    ///   - No result is returned to the ARM core directly.
    ///   - The ARM core does not wait for completion; coprocessor may execute in parallel.
    ///   - Meaning of Op1, Op2, CRn, CRm, CRd is entirely coprocessor-defined.
    ///   - If the addressed coprocessor is not present, the instruction is undefined.

    /// CDP - Ask a coprocessor to perform an internal operation.
    /// Use: FPU/accelerator ops on systems that implement coprocessors.
    /// Semantics: CoprocOp(CP#, Op1, CRd, CRn, CRm, Op2)
    /// Example:   CP15 does an internal op on CRn/CRm -> CRd
    CDP,


    /// ###################################
    ///         Software Interrupt
    ///                (SWI)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:24] [23:0]
    ///  Cond     1111     imm24
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 1111 (bits 27:24)
    ///    Class selector for software interrupt instructions.
    ///
    /// 3) imm24 (bits 23:0)
    ///    24-bit immediate comment field.
    ///    This field is ignored by the core and passed to supervisor software.
    ///
    /// Semantics:
    ///   If condition passes:
    ///     - Enter Supervisor mode (SVC).
    ///     - PC is forced to the SWI vector address (0x08).
    ///     - CPSR is saved into SPSR_svc.
    ///     - Return address (PC of next instruction) is saved in LR_svc.
    ///
    /// Notes:
    ///   - The immediate value is typically used by the OS to select a service.
    ///   - SWI always causes a mode change and pipeline flush.
    ///   - Return is normally performed with MOVS PC, LR_svc (restores CPSR).

    /// SWI - Trap into the OS/monitor (software exception).
    /// Use: syscalls, BIOS calls, emulator “services”.
    /// Semantics: Enter SVC; LR_svc := return_addr; PC := vector(0x08)
    /// Example:   Trap(service_id)
    SWI,

    /// ###################################
    ///     Coprocessor Data Transfer
    ///            (LDC, STC)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24] [23] [22] [21] [20] [19:16] [15:12] [11:8] [7:0]
    ///  Cond     110     P    U    N    W    L     Rn      CRd     CP#   imm8
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 110 (bits 27:25)
    ///    Class selector for coprocessor data transfers.
    ///
    /// 3) P (bit 24) - Pre/Post indexing
    ///    1 = pre-index:  base +/- offset used for first transfer address
    ///    0 = post-index: base used first, then base updated after transfer
    ///
    /// 4) U (bit 23) - Up/Down
    ///    1 = add offset
    ///    0 = subtract offset
    ///
    /// 5) N (bit 22) - Transfer length selector ("long"/"short")
    ///    Coprocessor-defined meaning, conventionally:
    ///      0 = short transfer
    ///      1 = long transfer
    ///
    /// 6) W (bit 21) - Write-back
    ///    1 = write updated address back into Rn
    ///    0 = do not write back
    ///    Note: unlike LDR/STR, post-indexed coprocessor transfers may require explicit W.
    ///
    /// 7) L (bit 20) - Load/Store direction
    ///    1 = LDC (load from memory to coprocessor)
    ///    0 = STC (store from coprocessor to memory)
    ///
    /// 8) Rn (bits 19:16)
    ///    Base register (ARM core register).
    ///
    /// 9) CRd (bits 15:12)
    ///    Coprocessor register involved in the transfer (often first register of a block).
    ///
    /// 10) CP# (bits 11:8)
    ///     Coprocessor number (0–15).
    ///
    /// 11) imm8 (bits 7:0) - Immediate offset (in *words*)
    ///     Effective byte offset = imm8 << 2
    ///
    /// Addressing semantics (conceptual):
    ///   offset_bytes = (imm8 << 2)
    ///   delta        = (U==1) ? +offset_bytes : -offset_bytes
    ///
    ///   if (P==1) { addr = Rn + delta; if (W) Rn = addr; }
    ///   else      { addr = Rn;        if (W) Rn = Rn + delta; }
    ///
    /// Transfer semantics:
    ///   if (L==1) coprocessor loads data from memory starting at addr (LDC)
    ///   else      coprocessor stores data to memory starting at addr (STC)
    ///   Coprocessor controls how many words are transferred; N often selects the length.
    ///
    /// Notes / R15:
    ///   - If Rn == R15, the base value used is PC + 8 (ARM-state prefetch bias).
    ///   - Write-back to R15 must not be specified.
    ///   - The exact interpretation of CRd/N and the number of words transferred is
    ///     coprocessor-defined.

    /// LDC - Load data from memory into coprocessor registers.
    /// Use: moving blocks to an attached coprocessor (e.g., FPU/control coproc).
    /// Semantics: CoprocLoad(CP#, CRd, Mem[addr..])
    /// Example:   CP15 loads from Mem[R0..] into CRd..
    LDC,

    /// STC - Store data from coprocessor registers into memory.
    /// Use: dumping coprocessor state/blocks to RAM.
    /// Semantics: Mem[addr..] := CoprocStore(CP#, CRd..)
    /// Example:   Mem[R0..] := CP15 registers starting at CRd
    STC,

    /// ###################################
    ///   Coprocessor Register Transfers
    ///            (MRC, MCR)
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:24] [23:21] [20] [19:16] [15:12] [11:8] [7:5] [4] [3:0]
    ///  Cond     1110     Op1     L     CRn      Rd     CP#    Op2   1    CRm
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. Controls conditional execution.
    ///
    /// 2) 1110 (bits 27:24)
    ///    Class selector for coprocessor register transfers.
    ///
    /// 3) Op1 (bits 23:21)
    ///    Coprocessor-specific primary opcode.
    ///
    /// 4) L (bit 20) - Transfer direction
    ///    1 = MRC (Move from coprocessor to ARM register)
    ///    0 = MCR (Move from ARM register to coprocessor)
    ///
    /// 5) CRn (bits 19:16)
    ///    Coprocessor register (often “primary” register operand, coprocessor-defined).
    ///
    /// 6) Rd (bits 15:12)
    ///    ARM core register (destination for MRC, source for MCR).
    ///
    /// 7) CP# (bits 11:8)
    ///    Coprocessor number (0–15).
    ///
    /// 8) Op2 (bits 7:5)
    ///    Coprocessor-specific secondary opcode.
    ///
    /// 9) 1 (bit 4)
    ///    Must be one for MRC/MCR encodings (distinguishes from CDP).
    ///
    /// 10) CRm (bits 3:0)
    ///     Second coprocessor register operand (coprocessor-defined).
    ///
    /// Semantics (conceptual):
    ///   if (L==1) {            // MRC
    ///     Rd := CoprocRead(CP#, Op1, CRn, CRm, Op2);
    ///   } else {               // MCR
    ///     CoprocWrite(CP#, Op1, CRn, CRm, Op2, Rd);
    ///   }
    ///
    /// Special handling of Rd == R15 (ARM7TDMI behavior):
    ///   - MRC with Rd==R15: bits[31:28] of the transferred word update CPSR NZCV;
    ///     the other bits are ignored; PC and other CPSR bits are unaffected.
    ///   - MCR with Rd==R15: the value transferred is PC + 12.
    ///
    /// Notes:
    ///   - Meanings of Op1/Op2/CRn/CRm are coprocessor-defined.
    ///   - If the addressed coprocessor is not present, the instruction is undefined.

    /// MRC - Move from coprocessor register to ARM register.
    /// Use: reading system control/state (commonly CP15), feature/ID registers.
    /// Semantics: Rd := CoprocRead(CP#, Op1, CRn, CRm, Op2)
    /// Example:   R0 := CP15_read(...)
    MRC,

    /// MCR - Move from ARM register to coprocessor register.
    /// Use: configuring system control/state (commonly CP15), caches/MMU control.
    /// Semantics: CoprocWrite(CP#, Op1, CRn, CRm, Op2, Rd)
    /// Example:   CP15_write(..., R0)
    MCR,

    /// ###################################
    ///         Undefined Instruction
    /// ###################################
    /// Encoding (A32, 32 bits):
    /// [31:28] [27:25] [24]   [23:0]
    ///  Cond     011     1   xxxxxxxxxxxxxxxxxxxxxxxx
    ///
    /// 1) Cond (bits 31:28)
    ///    Condition field. If the condition fails, the instruction is ignored like any other.
    ///
    /// 2) 0111 (bits 27:24)
    ///    Major encoding for a block of undefined instruction space.
    ///
    /// 3) x...x (bits 23:0)
    ///    Currently unused / undefined by the core. Software should not rely on any effect.
    ///
    /// Semantics:
    ///   If condition passes -> take Undefined Instruction exception (trap).
    ///
    /// Notes:
    ///   - This space exists to allow future encodings and for OS/debug uses (e.g. breakpoints
    ///     via undefined-instruction trapping on cores without a dedicated breakpoint insn).
    ///   - If you need to *emit* an undefined instruction, a conservative convention is to
    ///     set all x bits to 0 (i.e., use imm24 = 0).

    /// UNDEFINED - Default enum value for when the instruction is not a
    /// defined one.
    UNDEFINED,
} Instruction;







#define ENCODE(op, rd, ra, rb) \
    ( ((uint32_t)(op) << INST_OPCODE_SHIFT) | \
      ((uint32_t)(rd) << INST_RD_SHIFT)     | \
      ((uint32_t)(ra) << INST_RA_SHIFT)     | \
      ((uint32_t)(rb) << INST_RB_SHIFT) )
