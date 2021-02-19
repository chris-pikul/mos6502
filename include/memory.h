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
#include "io_device.h"
#include "utils.h"

namespace mos6502 {
	// Holds the memory for the mos6502 system.
	// Originally the chip supported addressing for a max 64kb of memory.
	class Memory : public IODevice {
		using memory = std::vector<byte>;

	public:
		using iterator = memory::iterator;
		using const_iterator = memory::const_iterator;

		// Instantiates and allocates a memory block, returning the ioptr (std::shared_ptr)
		static ioptr Make(const size_t size = MAKE_KB(64));

		Memory(const size_t sizeBytes = MAKE_KB(64));

		//Allow Moving
		Memory(Memory&&) = default;
		Memory& operator=(Memory&&) = default;

		// No Copying
		Memory(const Memory&) = delete;
		Memory& operator=(const Memory&) = delete;

		// Resets each value in the memory scope with the specified value (default 0)
		virtual inline void Clear(const byte value = 0) {
			std::fill(m_Data.begin(), m_Data.end(), value);
		}

		// Return the fixed size of the memory
		inline const size_t GetSize() const { return m_Size; }

		// Return a pointer to the underlying data container
		inline byte* GetData() { return m_Data.data(); }

		// Prints the provided range of pages to the standard output
		void Print(const fast_byte start, const fast_byte end, const fast_byte bytesPerLine = 16);

		// Iterator implementations
		iterator begin() { return m_Data.begin(); }
		iterator end() { return m_Data.end(); }
		const_iterator begin() const { return m_Data.begin(); }
		const_iterator end() const { return m_Data.end(); }
		const_iterator cbegin() const { return m_Data.cbegin(); }
		const_iterator cend() const { return m_Data.cend(); }

		// Collection wrappers
		size_t size() const { return GetSize(); }
		byte* data() { return GetData(); }
		void clear() { Clear(0); }

		// Subscript operator
		byte& operator[](size_t i) { return m_Data[i]; }

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

	private:
		const size_t m_Size;

		memory m_Data;
	};
}

