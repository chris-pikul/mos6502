/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "bus.h"

#include <iostream>

namespace mos6502 {
	ioptr Bus::Make(ioptr mem) {
		return std::make_shared<Bus>(mem);
	}

	Bus::Bus(ioptr mem) : m_Memory(mem) {
		//STUB
	}

	byte Bus::ReadByte(const address& addr) const {
		if (m_Memory)
			return m_Memory->ReadByte(addr);
		else
			std::cerr << "mos6502::Bus::ReadByte attempted to use memory that is not connected (nullptr)." << std::endl;
		return 0;
	}

	word Bus::ReadWord(const address& addr) const {
		if(m_Memory)
			return m_Memory->ReadWord(addr);
		else
			std::cerr << "mos6502::Bus::ReadWord attempted to use memory that is not connected (nullptr)." << std::endl;
		return 0;
	}

	void Bus::WriteByte(const address& addr, const byte data) {
		if(m_Memory)
			m_Memory->WriteByte(addr, data);
		else
			std::cerr << "mos6502::Bus::WriteByte attempted to use memory that is not connected (nullptr)." << std::endl;
	}

	void Bus::WriteWord(const address& addr, const word data) {
		if (m_Memory)
			m_Memory->WriteWord(addr, data);
		else
			std::cerr << "mos6502::Bus::WriteWord attempted to use memory that is not connected (nullptr)." << std::endl;
	}

	void Bus::WriteBytes(const address& offset, const std::vector<byte>& bytes) {
		if (m_Memory)
			m_Memory->WriteBytes(offset, bytes);
		else
			std::cerr << "mos6502::Bus::WriteBytes attempted to use memory that is not connected (nullptr)." << std::endl;
	}

} // END - namespace mos6502