/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <string>
#include <array>

#include "types.h"

namespace mos6502 {
	// Declares the addressing mode the instruction will use.
	// Combined with the OpCode, the address mode may modify
	// the clock cylces required to perform the instruction.
	enum class AddressMode : byte {
		ILL,	// Illegal Address Mode

		ABS,	// Absolute
		ABX,	// Absolute, offset by X
		ABY,	// Absolute, offset by Y
		ACC,	// Accumulator

		IMM,	// Immediate
		IMP,	// Implicit

		IND,	// Indirect
		INX,	// Indexed Indirect using X
		INY,	// Indirect Indexed using Y

		REL,	// Relative

		ZPG,	// Zero Page
		ZPX,	// Zero Page, offset by X
		ZPY,	// Zero Page, offset by Y
	};

	// Array of the available addressing mode mnmuemonics as strings.
	// Each addressing mode is a 3-letter code, the indices of which
	// match the values provided by the AddressMode enum
	extern const std::string AddressMnmuemonics[14];

	// Retrives a 3-letter mnmuemonic string from the provided enum
	const std::string& GetAddressMnmuemonic(const AddressMode& mode);

	// Each instruction is represented as a 3-letter mnmuemonic code.
	// Combined with the addressing modes they form a complete
	// instruction. These can be translated to byte OpCodes as well
	// using the lookup table provided.
	enum class Instruction : byte {
		ILL, // Illegal Operand

		ADC, // Add with carry
		AND, // AND (Acc)
		ASL, // Arithmetic Shift Left
		BCC, // Branch on Carry Clear
		BCS, // Branch on Carry Set
		BEQ, // Branch on Equal (zero set)
		BIT, // Bit Test
		BMI, // Branch on Minus (negative set)
		BNE, // Branch on Not Equal (zero clear)
		BPL, // Branch on Plus (negative clear)
		BRK, // Break / Interrupt
		BVC, // Branch on Overflow Clear
		BVS, // Branch on Overflow Set
		CLC, // Clear Carry
		CLD, // Clear Decimal
		CLI, // Clear Interrupt Disable
		CLV, // Clear Overflow
		CMP, // Compare w/Acc
		CPX, // Compare w/X
		CPY, // Compare w/Y
		DEC, // Decrement
		DEX, // Decrement X
		DEY, // Decrement Y
		EOR, // Exclusive OR
		INC, // Increment
		INX, // Increment X
		INY, // Increment Y
		JMP, // Jump
		JSR, // Jump Subroutine
		LDA, // Load Accumulator
		LDX, // Load X
		LDY, // Load Y
		LSR, // Logical Shift R
		NOP, // No-Operation
		ORA, // Or w/Acc
		PHA, // Push Acc
		PHP, // Push PC
		PLA, // Pull Acc
		PLP, // Pull PC
		ROL, // Rotate Left
		ROR, // Rotate Right
		RTI, // Return from Interrupt
		RTS, // Return from Subroutine
		SBC, // Subtract with Carry
		SEC, // Set Carry
		SED, // Set Decimal
		SEI, // Set Interrupt Disable
		STA, // Store Accumulator
		STX, // Store X
		STY, // Store Y
		TAX, // Transfer Acc to X
		TAY, // Transfer Acc to Y
		TSX, // Transfer SP to X
		TXA, // Transfer X to Acc
		TXS, // Transfer X to SP
		TYA, // Transfer Y to Acc
	};

	constexpr fast_byte INSTR_MNMUEMONIC_SIZE = 57;

	// Array of the 3-letter mnmuemonic codes representing each address.
	// The values here, match indices with the values provided by the
	// Instruction enum.
	extern const std::string InstructionMnmuemonics[INSTR_MNMUEMONIC_SIZE];

	// Retrieves the string matching the provided Instruction enum.
	const std::string& GetInstructionMnmuemonic(const Instruction& inst);
	
	// Returns true if the input string matches any of the valid
	// instruction mnmuemonics (not "ILL").
	bool HasInstructionMnmuemonic(const std::string& str);

	// Returns a Instruction enum for the given input string.
	// Will return ILL if one does not match.
	Instruction MnmuemonicToInstruction(const std::string& str);

	// Holds the combined information for an instruction.
	// Joins the opcode, the instruction & addressing mnmuemonics
	// as well as the bytes used and cycle information.
	struct InstructionDetail {
		byte opCode;

		Instruction instruction;
		AddressMode addressing;

		fast_byte bytesUsed;
		fast_byte minCycles;
		bool variableCycles;
	};

	// Maximum number of instructions provided by the MOS-6502
	constexpr fast_byte MAX_INSTRUCTIONS = 255;

	// An array mapping the each opcode to an InstructionDetail
	// object. The index is the opcode itself, so lookup is easier
	extern const std::array<InstructionDetail, MAX_INSTRUCTIONS> InstructionDetails;

	// Returns the matching InstructionDetail struct for the given instruction and address mode
	// enums. If no match is found, then the first ILL slot is returned (opcode 0x02).
	const InstructionDetail& FindInstructionDetail(const Instruction& inst, const AddressMode& addr);
}