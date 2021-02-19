/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "instructions.h"

namespace mos6502 {
	const std::string AddressMnmuemonics[] = {
		"ILL", // Illegal

		"ABS", // Absolute
		"ABX", // Absolute, offset by X
		"ABY", // Absolute, offset by Y
		"ACC", // Accumulator

		"IMM", // Immediate
		"IMP", // Implicit

		"IND", // Indirect
		"INX", // Indexed Indirect using X
		"INY", // Indirect Indexed using Y

		"REL", // Relative

		"ZPG", // Zero Page
		"ZPX", // Zero Page, offset by X
		"ZPY", // Zero Page, offset by Y
	};

	const std::string& GetAddressMnmuemonic(const AddressMode& mode) {
		return AddressMnmuemonics[(int)mode];
	}

	const std::string InstructionMnmuemonics[INSTR_MNMUEMONIC_SIZE] = {
		"ILL", // Illegal Operand

		"ADC", // Add with carry
		"AND", // AND (Acc)
		"ASL", // Arithmetic Shift Left
		"BCC", // Branch on Carry Clear
		"BCS", // Branch on Carry Set
		"BEQ", // Branch on Equal (zero set)
		"BIT", // Bit Test
		"BMI", // Branch on Minus (negative set)
		"BNE", // Branch on Not Equal (zero clear)
		"BPL", // Branch on Plus (negative clear)
		"BRK", // Break / Interrupt
		"BVC", // Branch on Overflow Clear
		"BVS", // Branch on Overflow Set
		"CLC", // Clear Carry
		"CLD", // Clear Decimal
		"CLI", // Clear Interrupt Disable
		"CLV", // Clear Overflow
		"CMP", // Compare w/Acc
		"CPX", // Compare w/X
		"CPY", // Compare w/Y
		"DEC", // Decrement
		"DEX", // Decrement X
		"DEY", // Decrement Y
		"EOR", // Exclusive OR
		"INC", // Increment
		"INX", // Increment X
		"INY", // Increment Y
		"JMP", // Jump
		"JSR", // Jump Subroutine
		"LDA", // Load Accumulator
		"LDX", // Load X
		"LDY", // Load Y
		"LSR", // Logical Shift R
		"NOP", // No-Operation
		"ORA", // Or w/Acc
		"PHA", // Push Acc
		"PHP", // Push PC
		"PLA", // Pull Acc
		"PLP", // Pull PC
		"ROL", // Rotate Left
		"ROR", // Rotate Right
		"RTI", // Return from Interrupt
		"RTS", // Return from Subroutine
		"SBC", // Subtract with Carry
		"SEC", // Set Carry
		"SED", // Set Decimal
		"SEI", // Set Interrupt Disable
		"STA", // Store Accumulator
		"STX", // Store X
		"STY", // Store Y
		"TAX", // Transfer Acc to X
		"TAY", // Transfer Acc to Y
		"TSX", // Transfer SP to X
		"TXA", // Transfer X to Acc
		"TXS", // Transfer X to SP
		"TYA", // Transfer Y to Acc
	};

	const std::string& GetInstructionMnmuemonic(const Instruction& inst) {
		return InstructionMnmuemonics[(int)inst];
	}

	bool HasInstructionMnmuemonic(const std::string& str) {
		if( str.length() != 3 || str == "ILL" )
			return false;

		//Skip first (0) because that is ILL
		for( int i = 1; i < INSTR_MNMUEMONIC_SIZE; i++ ) {
			if( str == InstructionMnmuemonics[i])
				return true;
		}
		return false;
	}

	Instruction MnmuemonicToInstruction(const std::string& str) {
		if( str.length() != 3 || str == "ILL" )
			return Instruction::ILL;

		//Skip first (0) because that is ILL
		for( int i = 1; i < INSTR_MNMUEMONIC_SIZE; i++ ) {
			if( str == InstructionMnmuemonics[i] )
				return static_cast<Instruction>(i);
		}
		return Instruction::ILL;
	}

	// Declare the master instruction table for looking up instructions
	const std::array<InstructionDetail, MAX_INSTRUCTIONS> InstructionDetails = {
		InstructionDetail
		{ 0x00, Instruction::BRK, AddressMode::IMP, 1, 7, false },
		{ 0x01, Instruction::ORA, AddressMode::INX, 2, 6, false },
		{ 0x02, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x03, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x04, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x05, Instruction::ORA, AddressMode::ZPG, 2, 3, false },
		{ 0x06, Instruction::ASL, AddressMode::ZPG, 2, 5, false },
		{ 0x07, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x08, Instruction::PHP, AddressMode::IMP, 1, 3, false },
		{ 0x09, Instruction::ORA, AddressMode::IMM, 2, 2, false },
		{ 0x0A, Instruction::ASL, AddressMode::ACC, 1, 2, false },
		{ 0x0b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x0c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x0D, Instruction::ORA, AddressMode::ABS, 3, 4, false },
		{ 0x0E, Instruction::ASL, AddressMode::ABS, 3, 6, false },
		{ 0x0f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x10, Instruction::BPL, AddressMode::REL, 2, 2, true },
		{ 0x11, Instruction::ORA, AddressMode::INY, 2, 5, true },
		{ 0x12, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x13, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x14, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x15, Instruction::ORA, AddressMode::ZPX, 2, 4, false },
		{ 0x16, Instruction::ASL, AddressMode::ZPX, 2, 6, false },
		{ 0x17, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x18, Instruction::CLC, AddressMode::IMP, 1, 2, false },
		{ 0x19, Instruction::ORA, AddressMode::ABY, 3, 4, true },
		{ 0x1a, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x1b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x1c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x1D, Instruction::ORA, AddressMode::ABX, 3, 4, true },
		{ 0x1E, Instruction::ASL, AddressMode::ABX, 3, 7, false },
		{ 0x1f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x20, Instruction::JSR, AddressMode::ABS, 3, 6, false },
		{ 0x21, Instruction::AND, AddressMode::INX, 2, 6, false },
		{ 0x22, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x23, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x24, Instruction::BIT, AddressMode::ZPG, 2, 3, false },
		{ 0x25, Instruction::AND, AddressMode::ZPG, 2, 3, false },
		{ 0x26, Instruction::ROL, AddressMode::ZPG, 2, 5, false },
		{ 0x27, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x28, Instruction::PLP, AddressMode::IMP, 1, 4, false },
		{ 0x29, Instruction::AND, AddressMode::IMM, 2, 2, false },
		{ 0x2A, Instruction::ROL, AddressMode::ACC, 1, 2, false },
		{ 0x2b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x2C, Instruction::BIT, AddressMode::ABS, 3, 4, false },
		{ 0x2D, Instruction::AND, AddressMode::ABS, 3, 4, false },
		{ 0x2E, Instruction::ROL, AddressMode::ABS, 3, 6, false },
		{ 0x2f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x30, Instruction::BMI, AddressMode::REL, 2, 2, true },
		{ 0x31, Instruction::AND, AddressMode::INY, 2, 5, true },
		{ 0x32, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x33, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x34, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x35, Instruction::AND, AddressMode::ZPX, 2, 4, false },
		{ 0x36, Instruction::ROL, AddressMode::ZPX, 2, 6, false },
		{ 0x37, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x38, Instruction::SEC, AddressMode::IMP, 1, 2, false },
		{ 0x39, Instruction::AND, AddressMode::ABY, 3, 4, true },
		{ 0x3a, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x3b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x3c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x3D, Instruction::AND, AddressMode::ABX, 3, 4, true },
		{ 0x3E, Instruction::ROL, AddressMode::ABX, 3, 7, false },
		{ 0x3f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x40, Instruction::RTI, AddressMode::IMP, 1, 6, false },
		{ 0x41, Instruction::EOR, AddressMode::INX, 2, 6, false },
		{ 0x42, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x43, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x44, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x45, Instruction::EOR, AddressMode::ZPG, 2, 3, false },
		{ 0x46, Instruction::LSR, AddressMode::ZPG, 2, 5, false },
		{ 0x47, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x48, Instruction::PHA, AddressMode::IMP, 1, 3, false },
		{ 0x49, Instruction::EOR, AddressMode::IMM, 2, 2, false },
		{ 0x4A, Instruction::LSR, AddressMode::ACC, 1, 2, false },
		{ 0x4b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x4C, Instruction::JMP, AddressMode::ABS, 3, 3, false },
		{ 0x4D, Instruction::EOR, AddressMode::ABS, 3, 4, false },
		{ 0x4E, Instruction::LSR, AddressMode::ABS, 3, 6, false },
		{ 0x4f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x50, Instruction::BVC, AddressMode::REL, 2, 2, true },
		{ 0x51, Instruction::EOR, AddressMode::INY, 2, 5, true },
		{ 0x52, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x53, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x54, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x55, Instruction::EOR, AddressMode::ZPX, 2, 4, false },
		{ 0x56, Instruction::LSR, AddressMode::ZPX, 2, 6, false },
		{ 0x57, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x58, Instruction::CLI, AddressMode::IMP, 1, 2, false },
		{ 0x59, Instruction::EOR, AddressMode::ABY, 3, 4, true },
		{ 0x5a, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x5b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x5c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x5D, Instruction::EOR, AddressMode::ABX, 3, 4, true },
		{ 0x5E, Instruction::LSR, AddressMode::ABX, 3, 7, false },
		{ 0x5f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x60, Instruction::RTS, AddressMode::IMP, 1, 6, false },
		{ 0x61, Instruction::ADC, AddressMode::INX, 2, 6, false },
		{ 0x62, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x63, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x64, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x65, Instruction::ADC, AddressMode::ZPG, 2, 3, false },
		{ 0x66, Instruction::ROR, AddressMode::ZPG, 2, 5, false },
		{ 0x67, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x68, Instruction::PLA, AddressMode::IMP, 1, 4, false },
		{ 0x69, Instruction::ADC, AddressMode::IMM, 2, 2, false },
		{ 0x6A, Instruction::ROR, AddressMode::ACC, 1, 2, false },
		{ 0x6b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x6C, Instruction::JMP, AddressMode::IND, 3, 5, false },
		{ 0x6D, Instruction::ADC, AddressMode::ABS, 3, 4, false },
		{ 0x6E, Instruction::ROR, AddressMode::ABS, 3, 6, false },
		{ 0x6f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x70, Instruction::BVC, AddressMode::REL, 2, 2, true },
		{ 0x71, Instruction::ADC, AddressMode::INY, 2, 5, true },
		{ 0x72, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x73, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x74, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x75, Instruction::ADC, AddressMode::ZPX, 2, 4, false },
		{ 0x76, Instruction::ROR, AddressMode::ZPX, 2, 6, false },
		{ 0x77, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x78, Instruction::SEI, AddressMode::IMP, 1, 2, false },
		{ 0x79, Instruction::ADC, AddressMode::ABY, 3, 4, true },
		{ 0x7a, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x7b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x7c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x7D, Instruction::ADC, AddressMode::ABX, 3, 4, true },
		{ 0x7E, Instruction::ROR, AddressMode::ABX, 3, 7, false },
		{ 0x7f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x80, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x81, Instruction::STA, AddressMode::INX, 2, 6, false },
		{ 0x82, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x83, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x84, Instruction::STY, AddressMode::ZPG, 2, 3, false },
		{ 0x85, Instruction::STA, AddressMode::ZPG, 2, 3, false },
		{ 0x86, Instruction::STX, AddressMode::ZPG, 2, 3, false },
		{ 0x87, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x88, Instruction::DEY, AddressMode::IMP, 1, 2, false },
		{ 0x89, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x8A, Instruction::TXA, AddressMode::IMP, 1, 2, false },
		{ 0x8b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x8C, Instruction::STY, AddressMode::ABS, 3, 4, false },
		{ 0x8D, Instruction::STA, AddressMode::ABS, 3, 4, false },
		{ 0x8E, Instruction::STX, AddressMode::ABS, 3, 4, false },
		{ 0x8f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x90, Instruction::BCC, AddressMode::REL, 2, 2, true },
		{ 0x91, Instruction::STA, AddressMode::INY, 2, 6, false },
		{ 0x92, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x93, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x94, Instruction::STY, AddressMode::ZPX, 2, 4, false },
		{ 0x95, Instruction::STA, AddressMode::ZPX, 2, 4, false },
		{ 0x96, Instruction::STX, AddressMode::ZPY, 2, 4, false },
		{ 0x97, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x98, Instruction::TYA, AddressMode::IMP, 1, 2, false },
		{ 0x99, Instruction::STA, AddressMode::ABY, 3, 5, false },
		{ 0x9A, Instruction::TXS, AddressMode::IMP, 1, 2, false },
		{ 0x9b, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x9c, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x9D, Instruction::STA, AddressMode::ABX, 3, 5, false },
		{ 0x9e, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0x9f, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xA0, Instruction::LDY, AddressMode::IMM, 2, 2, false },
		{ 0xA1, Instruction::LDA, AddressMode::INX, 2, 6, false },
		{ 0xA2, Instruction::LDX, AddressMode::IMM, 2, 2, false },
		{ 0xa3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xA4, Instruction::LDY, AddressMode::ZPG, 2, 3, false },
		{ 0xA5, Instruction::LDA, AddressMode::ZPG, 2, 3, false },
		{ 0xA6, Instruction::LDX, AddressMode::ZPG, 2, 3, false },
		{ 0xa7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xA8, Instruction::TAY, AddressMode::IMP, 1, 2, false },
		{ 0xA9, Instruction::LDA, AddressMode::IMM, 2, 2, false },
		{ 0xAA, Instruction::TAX, AddressMode::IMP, 1, 2, false },
		{ 0xab, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xAC, Instruction::LDY, AddressMode::ABS, 3, 4, false },
		{ 0xAD, Instruction::LDA, AddressMode::ABS, 3, 4, false },
		{ 0xAE, Instruction::LDX, AddressMode::ABS, 3, 4, false },
		{ 0xaf, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xB0, Instruction::BCS, AddressMode::REL, 2, 2, true },
		{ 0xB1, Instruction::LDA, AddressMode::INY, 2, 5, true },
		{ 0xb2, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xb3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xB4, Instruction::LDY, AddressMode::ZPX, 2, 4, false },
		{ 0xB5, Instruction::LDA, AddressMode::ZPX, 2, 4, false },
		{ 0xB6, Instruction::LDX, AddressMode::ZPY, 2, 4, false },
		{ 0xb7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xB8, Instruction::CLV, AddressMode::IMP, 1, 2, false },
		{ 0xB9, Instruction::LDA, AddressMode::ABY, 3, 4, true },
		{ 0xBA, Instruction::TSX, AddressMode::IMP, 1, 2, false },
		{ 0xbb, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xBC, Instruction::LDY, AddressMode::ABX, 3, 4, true },
		{ 0xBD, Instruction::LDA, AddressMode::ABX, 3, 4, true },
		{ 0xBE, Instruction::LDX, AddressMode::ABY, 3, 4, true },
		{ 0xbf, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xC0, Instruction::CPY, AddressMode::IMM, 2, 2, false },
		{ 0xC1, Instruction::CMP, AddressMode::INX, 2, 6, false },
		{ 0xc2, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xc3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xC4, Instruction::CPY, AddressMode::ZPG, 2, 3, false },
		{ 0xC5, Instruction::CMP, AddressMode::ZPG, 2, 3, false },
		{ 0xC6, Instruction::DEC, AddressMode::ZPG, 2, 5, false },
		{ 0xc7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xC8, Instruction::INY, AddressMode::IMP, 1, 2, false },
		{ 0xC9, Instruction::CMP, AddressMode::IMM, 2, 2, false },
		{ 0xCA, Instruction::DEX, AddressMode::IMP, 1, 2, false },
		{ 0xcb, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xCC, Instruction::CPY, AddressMode::ABS, 3, 4, false },
		{ 0xCD, Instruction::CMP, AddressMode::ABS, 3, 4, false },
		{ 0xCE, Instruction::DEC, AddressMode::ABS, 3, 6, false },
		{ 0xcf, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xD0, Instruction::BNE, AddressMode::REL, 2, 2, true },
		{ 0xD1, Instruction::CMP, AddressMode::INY, 2, 5, true },
		{ 0xd2, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xd3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xd4, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xD5, Instruction::CMP, AddressMode::ZPX, 2, 4, false },
		{ 0xD6, Instruction::DEC, AddressMode::ZPX, 2, 6, false },
		{ 0xd7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xD8, Instruction::CLD, AddressMode::IMP, 1, 2, false },
		{ 0xD9, Instruction::CMP, AddressMode::ABY, 3, 4, true },
		{ 0xda, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xdb, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xdc, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xDD, Instruction::CMP, AddressMode::ABX, 3, 4, true },
		{ 0xDE, Instruction::DEC, AddressMode::ABX, 3, 7, false },
		{ 0xdf, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xE0, Instruction::CPX, AddressMode::IMM, 2, 2, false },
		{ 0xE1, Instruction::SBC, AddressMode::INX, 2, 6, false },
		{ 0xe2, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xe3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xE4, Instruction::CPX, AddressMode::ZPG, 2, 3, false },
		{ 0xE5, Instruction::SBC, AddressMode::ZPG, 2, 3, false },
		{ 0xE6, Instruction::INC, AddressMode::ZPG, 2, 5, false },
		{ 0xe7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xE8, Instruction::INX, AddressMode::IMP, 1, 2, false },
		{ 0xE9, Instruction::SBC, AddressMode::IMM, 2, 2, false },
		{ 0xEA, Instruction::NOP, AddressMode::IMP, 1, 2, false },
		{ 0xeb, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xEC, Instruction::CPX, AddressMode::ABS, 3, 4, false },
		{ 0xED, Instruction::SBC, AddressMode::ABS, 3, 4, false },
		{ 0xEE, Instruction::INC, AddressMode::ABS, 3, 6, false },
		{ 0xef, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xF0, Instruction::BEQ, AddressMode::REL, 2, 2, true },
		{ 0xF1, Instruction::SBC, AddressMode::INY, 2, 5, true },
		{ 0xf2, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xf3, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xf4, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xF5, Instruction::SBC, AddressMode::ZPX, 2, 4, false },
		{ 0xF6, Instruction::INC, AddressMode::ZPX, 2, 6, false },
		{ 0xf7, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xF8, Instruction::SED, AddressMode::IMP, 1, 2, false },
		{ 0xF9, Instruction::SBC, AddressMode::ABY, 3, 4, true },
		{ 0xfa, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xfb, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xfc, Instruction::ILL, AddressMode::ILL, 1, 2, false }, // ILLEGAL
		{ 0xFD, Instruction::SBC, AddressMode::ABX, 3, 4, true },
		{ 0xFE, Instruction::INC, AddressMode::ABX, 3, 7, false },
	}; //END InstructionDetails initializer

	const InstructionDetail& FindInstructionDetail(const Instruction& inst, const AddressMode& addr) {
		for( const auto& itr : InstructionDetails ) {
			if( itr.instruction == inst && itr.addressing == addr )
				return itr;
		}
		return InstructionDetails[2]; // First ILL slot
	}
}