/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>

#include "instructions.h"

namespace mos6502 {

	// A program holds the written assembly language (human readable)
	// source code, and compiles it to byte code (machine language)
	// for execution by the CPU.
	class Program {
	public:

	public:
		static std::optional<std::shared_ptr<Program>> CompileFile(const std::string& filepath);

		struct Line {
			const unsigned int lineNumber;
			int pcOffset;

			std::string raw, comment;

			byte opCode;
			Instruction instruction;
			AddressMode addressing;

			word value;
		};

	public:
		Program() = default;

		bool CompileSourceFile(const std::string& filepath);
		bool CompileString(const std::string& source);

		const std::string& GetPath() const { return m_Path; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const word& GetStartingPCOffset() const { return m_StartingPCOffset; }

		const std::vector<Line>& GetSourceCode() const { return m_SourceCode; }
		const std::vector<byte>& GetByteCode() const { return m_ByteCode; }

	protected:


	private:
		std::string m_Path, m_Name;

		word m_StartingPCOffset = 0x0200;

		std::vector<Line> m_SourceCode;
		std::vector<byte> m_ByteCode;
	};
	
	using progptr = std::shared_ptr<Program>;
}