/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "types.h"

#define KB 0x400
#define MB 0x100000
#define GB 0x40000000

#define MAKE_KB(n) n * KB
#define MAKE_MB(n) n * MB
#define MAKE_GB(n) n * GB

#define BYTES_TO_KB(n) static_cast<float>(n) / static_cast<float>(KB)
#define BYTES_TO_MB(n) static_cast<float>(n) / static_cast<float>(MB)
#define BYTES_TO_GB(n) static_cast<float>(n) / static_cast<float>(GB)

#define MAKE_WORD(low, high) ((high << 8) | low)

#define GET_HIGH_BYTE(val) ((val >> 8) & 0xFF)
#define GET_LOW_BYTE(val) (val & 0xFF)
#define IS_NEGATIVE(val) (val & 0x80)

namespace mos6502 {
	// Starting location of the stack.
	// The MOS6502 features a 256 byte stack starting at 0x0100
	// and reaching to 0x01FF.
	const word ADDRESS_STACK = 0x0100;

	// Memory address holding a pointer to the next PC when
	// a Non-maskable Interrupt is fired.
	const word ADDRESS_NMI_VECTOR = 0xFFFA;

	// Memory address holding a pointer to the next PC when
	// a CPU Reset is fired.
	const word ADDRESS_RESET_VECTOR = 0xFFFC;

	// Memory address holding a pointer tp the next PC when
	// a Interrupt Request is fired.
	const word ADDRESS_IRQ_VECTOR = 0xFFFE;

	// Takes in a string of space-separated hexidecimal values
	// and converts that into a vector of bytes.
	std::vector<byte> ConvertHexStringToBytes(const std::string&);

	// Converts an incoming value into a hex string.
	// The string will be padded with '0' to the provided width.
	// If the width provided is 0, then the sizeof will be used
	// to calculate the hex value length required.
	template<typename T, class = typename std::enable_if_t<std::is_integral_v<T>>>
	std::string ToHex(T value, size_t width = 0) {
		//Ensure the value can fit
		if (width == 0)
			width = sizeof(value) * 2;

		//Pre-pad the string with 0
		std::string str(width, '0');
		for (int i = width - 1; i >= 0; i--, value >>= 4)
			str[i] = "0123456789ABCDEF"[value & 0xF];
		return str;
	}

	std::string Hex(byte value);
	std::string Hex(word value);
	std::string Hex(address addr);
}