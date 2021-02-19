/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 * 
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */

#include <iostream>
#include <memory>
#include <string>
#include <algorithm>

#include "mos6502.h"

// Returns true if the running system is big-endian
bool isBigEndian() {
	union {
		uint32_t i;
		char c[4];
	} check = { 0x01020304 };
	return check.c[0] == 0x01;
}

int main() {
	std::cout << "MOS-6502 Processor Emulation" << std::endl;
	std::cout << "============================" << std::endl;

	// Compile the sample program
	auto optProgram = mos6502::Program::CompileFile("./program.asm");
	if( !optProgram )
		return EXIT_FAILURE;
	const auto program = optProgram.value();

	// Establish some memory
	auto memory = mos6502::Memory::Make();

	// Convert and load the program
	memory->Write(program->GetStartingPCOffset(), program->GetByteCode());

	// Set the reset vector
	memory->WriteWord(mos6502::ADDRESS_RESET_VECTOR, program->GetStartingPCOffset()); // Set address $FFFC/D to point to PC $8000

	// Cast so we can debug it
	std::shared_ptr<mos6502::Memory> memptr = std::static_pointer_cast<mos6502::Memory>(memory);

	// Construct a communication bus
	auto bus = mos6502::Bus::Make(memory);

	// Create the CPU
	auto cpu = mos6502::CPU(bus);
	cpu.Reset();

	std::cout << "Starting CPU State: " << cpu << std::endl << std::endl;

	std::cout << "Available commands:" << std::endl;
	std::cout << "\tR - Reset CPU" << std::endl;
	std::cout << "\tI - Interrupt Request" << std::endl;
	std::cout << "\tN - Non-Maskable Interrupt" << std::endl;
	std::cout << "\tE - Execute until cycles complete" << std::endl;
	std::cout << "\tP - Print program counter page" << std::endl;
	std::cout << "\tS - Print stack page" << std::endl;
	std::cout << "\tZ - Print zero-page" << std::endl;
	std::cout << "\tV - Print last page (vectors)" << std::endl;
	std::cout << "\tM - Print entire memory" << std::endl;
	std::cout << "\tX - Exit program" << std::endl;
	std::cout << "\tAny other character will run 1 cycle" << std::endl;

	bool running = true;
	std::string inp="";
	do {
		std::cout << cpu << " > ";
		std::cin >> inp;

		switch (toupper(inp.at(0))) {
		case 'R':
			std::cout << "Reset..." << std::endl;
			cpu.Reset();
			break;
		case 'I':
			std::cout << "Interrupt Request..." << std::endl;
			cpu.IRQ();
			break;
		case 'N':
			std::cout << "Non-Maskable Interrupt..." << std::endl;
			cpu.NMI();
			break;
		case 'E':
			while (cpu.Tick()) {}
			break;
		case 'P': {
			mos6502::fast_byte page = GET_HIGH_BYTE(cpu.GetProgramCounter());
			memptr->Print(page, page);
			break;
		}
		case 'S':
			memptr->Print(0x01, 0x01);
			break;
		case 'Z':
			memptr->Print(0, 0);
			break;
		case 'V':
			memptr->Print(0xFF, 0xFF);
			break;
		case 'M':
			memptr->Print(0, 0xFF);
			break;
		case 'X':
			running = false;
			break;
		default:
			cpu.Tick();
		}
	} while (running);

	std::cout << "Exiting program" << std::endl;
	std::cout << "Ending CPU State: " << cpu << std::endl;

	return EXIT_SUCCESS;
}