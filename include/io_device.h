/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <vector>

#include "types.h"

namespace mos6502 {

	// Virtual IO Device Interface
	// Classes wishing to connect to mos6502 bus or memory should
	// implement this interface to declare how memory is read/wrote.
	class IODevice {
	public:
		// Read a single 8-bit byte from the address specified and return it
		virtual byte ReadByte(const address& addr) const = 0;

		// Read a single 16-bit (2 byte) word from the address specified and return it
		virtual word ReadWord(const address& addr) const = 0;

		// Write a single 8-bit byte to the address specified
		virtual void WriteByte(const address& addr, const byte data) = 0;

		// Write a single 16-bit (2 byte) word to the address specified
		virtual void WriteWord(const address& addr, const word data) = 0;

		// Write a vector of bytes to the device, starting at the offset and consuming the whole vector
		virtual void WriteBytes(const address& offset, const std::vector<byte>& bytes) = 0;

		virtual void Write(const address& addr, const byte data) { WriteByte(addr, data); }
		virtual void Write(const address& addr, const word data) { WriteWord(addr, data); }
		virtual void Write(const address& addr, const std::vector<byte>& data) { WriteBytes(addr, data); }
	};

	// Shared pointer to an IODevice
	using ioptr = std::shared_ptr<IODevice>;
}