/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <memory>

#include "types.h"

#include "io_device.h"
#include "bus.h"
#include "memory.h"
#include "program.h"
#include "cpu.h"

namespace mos6502 {
	// Device is a convienience struct for holding
	// all the working pieces together as an almost
	// "virtual machine".
	// It's not formally neccessary.
	struct Device {
		std::shared_ptr<Program> program = nullptr;
		std::shared_ptr<Memory> memory = nullptr;
		std::shared_ptr<Bus> bus = nullptr;
		std::shared_ptr<CPU> cpu = nullptr;

		Device() {
			// Empty dummy program
			program = std::make_shared<Program>();

			// Default memory constructor allocates 64KB
			memory = std::make_shared<Memory>();

			// Make a psuedo-bus to connect memory and CPU
			bus = std::make_shared<Bus>(memory);

			// Make the CPU and connect it up to the bus for
			// memory access
			cpu = std::make_shared<CPU>(bus);
		}
	};
}