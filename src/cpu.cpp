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

namespace mos6502 {

	CPU::CPU(ioptr bus) : m_Bus(bus) {
		m_PC = 0;
		m_SP = 0;
		m_Acc = m_X = m_Y = 0;
		m_ProcStatus = 0;
	}

	void CPU::Reset() {
		// Clear the registers
		m_Acc = m_X = m_Y = 0;

		// Set the program counter
		m_PC = ReadWord(ADDRESS_RESET_VECTOR);

		// Set the stack pointer
		m_SP = 0xFD;

		// Clear processor status except for the "unused" bit
		m_ProcStatus = 0 | (byte)StatusFlag::UNUSED;
	}

	bool CPU::Tick() {
		// Increment total cycles for posterity
		m_CyclesExecuted++;

		// If we have cycles remaining from previous commands,
		// then pretend we are still working on them.
		// NOTE: This has the side effect of the results
		// being available in the state before the command
		// has actually concluded, but in the scope of the
		// emulation this is ok.
		if (m_CyclesRem > 0) {
			m_CyclesRem--;
			std::cout << "Cycles remaining = " << Hex(m_CyclesRem) << std::endl;
			return m_CyclesRem > 0;
		}

		std::cout << "Reading from PC " << Hex(m_PC) << std::endl;

		// Read the next opcode from memory
		const byte opcode = ReadByte(static_cast<address>(m_PC));

		std::cout << "OpCode = " << Hex(opcode) << std::endl;

		// Increase the program counter since something was read
		m_PC++;

		// Retrieve the instruction details
		const InstructionDetail& instruction = InstructionDetails[opcode];

		std::cout << "Executing Instruction \"" << GetInstructionMnmuemonic(instruction.instruction) 
			<< "\", with addressing " << GetAddressMnmuemonic(instruction.addressing) << std::endl;

		// Perform addressing
		fast_byte countAddressing = 0;
		auto addr = ExecuteAddressing(instruction.addressing, countAddressing);

		// Perform instruction
		fast_byte countInstruction = ExecuteInstruction(instruction.instruction, addr);

		// Add the cycle cost to the counter
		m_CyclesRem += countAddressing + countInstruction;

		std::cout << "Completed, remaining cycles = " << Hex(m_CyclesRem) << std::endl;

		// Ensure the unused flag is true, just for historics.
		SetStatusFlag(StatusFlag::UNUSED);

		return m_CyclesRem > 0;
	}

	void CPU::IRQ() {
		if (HasStatusFlag(StatusFlag::INTERRUPT) == true)
			return; // Interrupts are disabled, so no go.

		// Push the current program counter
		PushToStack(GET_HIGH_BYTE(m_PC));
		PushToStack(GET_LOW_BYTE(m_PC));

		// Set some flags and push it to the stack
		SetStatusFlag(StatusFlag::BREAK, false);
		SetStatusFlag(StatusFlag::INTERRUPT, true);
		SetStatusFlag(StatusFlag::UNUSED, true);
		PushToStack(m_ProcStatus.value);

		// Read new program location from interrupt vector
		m_PC = ReadWord(ADDRESS_IRQ_VECTOR);

		m_CyclesRem += 7; // 7 Cycles to complete
	}

	void CPU::NMI() {
		// Push the current program counter
		PushToStack(GET_HIGH_BYTE(m_PC));
		PushToStack(GET_LOW_BYTE(m_PC));

		// Set some flags and push it to the stack
		SetStatusFlag(StatusFlag::BREAK, false);
		SetStatusFlag(StatusFlag::INTERRUPT, true);
		SetStatusFlag(StatusFlag::UNUSED, true);
		PushToStack(m_ProcStatus.value);

		// Read new program location from interrupt vector
		m_PC = ReadWord(ADDRESS_NMI_VECTOR);

		m_CyclesRem += 8; // 8 Cycles to complete
	}

	byte CPU::FetchData(const address& addr) const {
		if (m_WasSupplied)
			return m_SuppliedValue;

		return ReadByte(addr);
	}

	void CPU::PushToStack(const byte data) {
		const address pointer = { ADDRESS_STACK + m_SP };
		m_SP--;
		WriteByte(pointer, data);
	}

	byte CPU::PullFromStack() {
		m_SP++;
		const address pointer = { ADDRESS_STACK + m_SP };
		return ReadByte(pointer);
	}

	byte CPU::ReadByte(const address& addr) const {
		if (m_Bus)
			return m_Bus->ReadByte(addr);

		std::cerr << "mos6502::CPU::ReadByte attempted to access bus that is not connected (nullptr)" << std::endl;
		return 0;
	}

	word CPU::ReadWord(const address& addr) const {
		if (m_Bus)
			return m_Bus->ReadWord(addr);

		std::cerr << "mos6502::CPU::ReadWord attempted to access bus that is not connected (nullptr)" << std::endl;
		return 0;
	}

	void CPU::WriteByte(const address& addr, const byte data) {
		if (m_Bus)
			m_Bus->WriteByte(addr, data);
		else
			std::cerr << "mos6502::CPU::WriteByte attempted to access bus that is not connected (nullptr)" << std::endl;
	}

	void CPU::WriteWord(const address& addr, const word data) {
		if (m_Bus)
			m_Bus->WriteWord(addr, data);
		else
			std::cerr << "mos6502::CPU::WriteWord attempted to access bus that is not connected (nullptr)" << std::endl;
	}

	void CPU::WriteBytes(const address& offset, const std::vector<byte>& bytes) {
		if (m_Bus)
			m_Bus->WriteBytes(offset, bytes);
		else
			std::cerr << "mos6502::CPU::WriteBytes attempted to access bus that is not connected (nullptr)" << std::endl;
	}
}