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

namespace mos6502 {
	// The bus represents the address-bus portion of the MOS6502.
	// This class is responsible for mapping incoming address registers to
	// their respective hardware counterparts.
	class Bus : public IODevice {
	public:
		static ioptr Make(ioptr mem);

		Bus() = default;

		Bus(ioptr mem);

		// Return the pointer to the attached memory object (IODevice)
		virtual inline const ioptr GetMemory() const {
			return m_Memory;
		}

		// Replace the pointer to the attached memory object (IODevice)
		// with the given pointer.
		virtual inline void MountMemory(ioptr mem) {
			m_Memory.swap(mem);
		}

	public: // Implement IODevice

		// Read a single 8-bit byte from the address specified and return it
		virtual byte ReadByte(const address& addr) const override;

		// Read a single 16-bit (2 byte) word from the address specified and return it
		virtual word ReadWord(const address& addr) const override;

		// Write a single 8-bit byte to the address specified
		virtual void WriteByte(const address& addr, const byte data) override;

		// Write a single 16-bit (2 byte) word to the address specified
		virtual void WriteWord(const address& addr, const word data) override;

		// Write a vector of bytes to the device, starting at the offset and consuming the whole vector
		virtual void WriteBytes(const address& offset, const std::vector<byte>& bytes);

	protected:
		ioptr m_Memory;
	};
}
