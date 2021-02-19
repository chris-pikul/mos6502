/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>

namespace mos6502 {
	// single 8-bit unsigned integer
	using byte = uint8_t;

	// single 16-bit unsigned integer
	using word = uint16_t;

	// single 8-bit unsigned integer,
	// using the compilers prefered type for quickness
	using fast_byte = uint_fast8_t;

	// single 16-bit unsigned integer,
	// using the compilers prefered type for quickness
	using fast_word = uint_fast16_t;

	// Represents a 16-bit (word) address.
	// Seperates the page and record by each byte component
	union address {
		word value;
		struct {
			byte record;
			byte page;
		};

		address() = default;
		address(int val) : value(val) {}; // allows assignment
		address(byte page, byte record) : record(record), page(page) {}; // struct assignment

		// Formats the address as a 4 character uppercase hexidecimal string prefixed with $.
		friend std::ostream& operator<<(std::ostream& os, const address& addr) {
			os << std::hex 
				<< std::uppercase 
				<< std::setfill('0') 
				<< std::setw(4) 
				<< addr.value;
			return os;
		}
	};
}