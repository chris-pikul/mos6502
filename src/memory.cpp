/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "memory.h"

#include <iostream>
#include <memory>
#include <algorithm>

namespace mos6502 {

	ioptr Memory::Make(const size_t size) {
		return std::make_shared<Memory>(size);
	}

	Memory::Memory(const size_t sizeBytes) : m_Size(sizeBytes) {
		m_Data.resize(m_Size);
	}

	void Memory::Print(const fast_byte start, const fast_byte end, const fast_byte bpl) {
		using uint = unsigned int;

		uint ustart = static_cast<uint>(start);
		uint uend = static_cast<uint>(end);
		uint ubpl = static_cast<uint>(bpl);

		uint numlines = ((uend + 1 - ustart) * 0xFF) / bpl;
		address addr = { start, 0 };
		for (uint line = 0; line <= numlines; line++) {
			std::cout << "[$" << Hex(addr) << "-$" << Hex(address{ addr.value + bpl - 1 }) << "] ";
			for (byte offset = 0; offset < bpl; offset++) {
				const auto data = ReadByte(address{ addr.value + offset });
				std::cout << Hex(data);
				if (offset < bpl - 1) std::cout << " ";
			}
			addr.value += bpl;
			std::cout << std::endl;
		}
	}

	byte Memory::ReadByte(const address& addr) const {
		if (addr.value < m_Size)
			return m_Data[addr.value];
		return 0;
	}

	word Memory::ReadWord(const address& addr) const {
		byte low = ReadByte(addr);
		byte high = ReadByte(addr.value + 1);
		return MAKE_WORD(low, high);
		/*
		word out = 0;
		if (addr.value < m_Size)
			out = m_Data[addr.value];
		if (static_cast<size_t>(addr.value) + 1 < m_Size)
			out |= m_Data[addr.value] << 8;

		return out;
		*/
	}

	void Memory::WriteByte(const address& addr, const byte data) {
		if (addr.value < m_Size)
			m_Data[addr.value] = data;
	}

	void Memory::WriteWord(const address& addr, const word data) {
		if (addr.value < m_Size)
			m_Data[addr.value] = GET_LOW_BYTE(data);
		if ((static_cast<size_t>(addr.value) + 1) < m_Size)
			m_Data[addr.value + 1] = GET_HIGH_BYTE(data);
	}

	void Memory::WriteBytes(const address& offset, const std::vector<byte>& bytes) {
		//Only itterate to the boundaries, no wrapping.
		const size_t maxlen = std::min(bytes.size(), m_Size - offset.value);

		// Fast C memory copy directly into the array.
		// We know the max boundaries, and these are primitive types (byte)
		memcpy(&m_Data[offset.value], bytes.data(), maxlen);
	}
} // END - namespace mos6502