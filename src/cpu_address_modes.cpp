/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "cpu.h"

#include <iostream>

#include "utils.h"

namespace mos6502 {

	address CPU::ExecuteAddressing(const AddressMode addrMode, fast_byte& outCycles) {
		ClearSupplied();

		switch (addrMode) {
		case AddressMode::ILL:	// Illegal Address Mode
			std::cerr << "attempting to execute an illegal address mode in mos6502::CPU::ExecuteAddressing" << std::endl;
			break;

		case AddressMode::ABS:
			return Addr_ABS(outCycles);
		case AddressMode::ABX:
			return Addr_ABX(outCycles);
		case AddressMode::ABY:
			return Addr_ABY(outCycles);
		case AddressMode::ACC:
			return Addr_ACC(outCycles);
		case AddressMode::IMM:
			return Addr_IMM(outCycles);
		case AddressMode::IMP:
			return Addr_IMP(outCycles);
		case AddressMode::IND:
			return Addr_IND(outCycles);
		case AddressMode::INX:
			return Addr_INX(outCycles);
		case AddressMode::INY:
			return Addr_INY(outCycles);
		case AddressMode::REL:
			return Addr_REL(outCycles);
		case AddressMode::ZPG:
			return Addr_ZPG(outCycles);
		case AddressMode::ZPX:
			return Addr_ZPX(outCycles);
		case AddressMode::ZPY:
			return Addr_ZPY(outCycles);

		default:
			std::cerr << "warning: unknown or unimplemented addressing mode in mos6502::CPU::ExecuteAddressing" << std::endl;
		}
		return 0;
	}

	address CPU::Addr_ABS(fast_byte& outCycles) {
		const byte low = ReadByte(m_PC);
		m_PC++;
		const byte high = ReadByte(m_PC);
		m_PC++;
		const address addr = { MAKE_WORD(low, high) };

		outCycles = 3; // Because of a 16-bit read

		return addr;
	}

	address CPU::Addr_ABX(fast_byte& outCycles) {
		const byte low = ReadByte(m_PC);
		m_PC++;
		const byte high = ReadByte(m_PC);
		m_PC++;
		const address addr = { MAKE_WORD(low, high) + m_X }; // Add X Register

		//Calculate cost
		if (addr.page != high)// Check for page change
			outCycles = 4; // 16-bit address with a page change
		else
			outCycles = 3; // 16-bit address within the same page

		return addr;
	}

	address CPU::Addr_ABY(fast_byte& outCycles) {
		const byte low = ReadByte(m_PC);
		m_PC++;
		const byte high = ReadByte(m_PC);
		m_PC++;
		const address addr = { MAKE_WORD(low, high) + m_Y }; // Add Y Register

		//Calculate cost
		if (addr.page != high)// Check for page change
			outCycles = 4; // 16-bit address with a page change
		else
			outCycles = 3; // 16-bit address within the same page

		return addr;
	}

	address CPU::Addr_ACC(fast_byte& outCycles) {
		SetSupplied(m_Acc);

		//No cost
		outCycles = 1;

		return m_PC;
	}

	address CPU::Addr_IMM(fast_byte& outCycles) {
		// Next program value
		const address addr = { m_PC++ }; 

		// No additional cost
		outCycles = 1;

		return addr;
	}

	address CPU::Addr_IMP(fast_byte& outCycles) {
		SetSupplied(m_Acc);

		// No addressing needed
		outCycles = 1;

		return { 0 };
	}

	address CPU::Addr_IND(fast_byte& outCycles) {
		byte low = ReadByte(m_PC);
		m_PC++;
		byte high = ReadByte(m_PC);
		m_PC++;
		const address pointer = { MAKE_WORD(low, high) };
		
		// Read the final low byte from the pointer location
		low = ReadByte(pointer);
		
		//Check if the page-boundary bug is in effect.
		//If so, the page portion may wrap.
		if (pointer.record == 0xFF) {
			high = ReadByte(pointer.value & 0xFF00);
		} else {
			high = ReadByte(pointer.value + 1);
		}

		outCycles = 4; // Because of 2 16-bit reads

		return address{ MAKE_WORD(low, high) };
	}

	address CPU::Addr_INX(fast_byte& outCycles) {
		const word table = static_cast<word>( ReadByte(m_PC) );
		m_PC++;

		const word xword = static_cast<word>(m_X);

		const byte low = ReadByte( (table + xword) & 0x00FF );
		const byte high = ReadByte( (table + xword + 1) & 0xFF00);

		outCycles = 5; // reference data-tables ellude to 4 cycles

		return address{ MAKE_WORD(low, high) };
	}

	address CPU::Addr_INY(fast_byte& outCycles) {
		const word table = static_cast<word>(ReadByte(m_PC));
		m_PC++;

		const byte low = ReadByte(table & 0x00FF);
		const byte high = ReadByte((table + 1) & 0x00FF);

		const address addr = { MAKE_WORD(low, high) + m_Y };

		//Calculate cost
		if (addr.page != high)// Check for page change
			outCycles = 5; // 16-bit address with a page change
		else
			outCycles = 4; // 16-bit address within the same page

		return addr;
	}

	address CPU::Addr_REL(fast_byte& outCycles) {
		byte rel = ReadByte(m_PC);
		m_PC++;

		if (rel & 0x80)
			rel |= 0xFF00;

		// The branch itself will determine the cost
		outCycles = 1;

		return { rel };
	}

	address CPU::Addr_ZPG(fast_byte& outCycles) { 
		const address addr = static_cast<address>(ReadByte(m_PC));
		m_PC++;

		outCycles = 2; // 1 byte read

		return addr;
	}

	address CPU::Addr_ZPX(fast_byte& outCycles) { 
		const byte low = ReadByte(m_PC);
		m_PC++;

		// Add the X register, but don't leave the zero page.
		const address addr = { (low + m_X) & 0x00FF };

		outCycles = 3; // 1 byte read, 1 added

		return addr;
	}

	address CPU::Addr_ZPY(fast_byte& outCycles) { 
		const byte low = ReadByte(m_PC);
		m_PC++;

		// Add the Y register, but don't leave the zero page.
		const address addr = { (low + m_Y) & 0x00FF };

		outCycles = 3; // 1 byte read, 1 added

		return addr;
	}
}