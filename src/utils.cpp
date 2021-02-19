/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "utils.h"

#include <iomanip>
#include <sstream>

namespace mos6502 {

	std::vector<byte> ConvertHexStringToBytes(const std::string& str) {
		std::stringstream ss;
		ss << str; // Load the string into the stream

		std::vector<byte> results;

		std::string tmp;
		while (!ss.eof()) {
			ss >> tmp;
			results.push_back(static_cast<byte>(std::stoul(tmp, nullptr, 16)));
		}

		return results;
	}

	std::string Hex(byte val) { return ToHex<byte>(val, 2); }
	std::string Hex(word val) { return ToHex<word>(val, 4); }
	std::string Hex(address val) { return ToHex<word>(val.value, 4); }
}