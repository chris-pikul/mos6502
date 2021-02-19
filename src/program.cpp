/*
 *	MOS-6502 Emulator
 * ===================
 * Simple emulator recreating the processes of the famous MOS-6502 processor.
 *
 * Copyright (c) 2021 Chris Pikul.
 * MIT License. See file "LICENSE.txt" for full license.
 */
#include "program.h"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <map>

#include "instructions.h"
#include "utils.h"

std::string TrimStart(const std::string& str, const std::string& ws = " \t") {
	const auto loc = str.find_first_not_of(ws);
	if( loc != std::string::npos )
		return str.substr(loc);
	return str;
}

std::string TrimEnd(const std::string& str, const std::string& ws = " \t") {
	const auto loc = str.find_last_not_of(ws);
	if( loc != std::string::npos )
		return str.substr(0, loc);
	return str;
}

std::string TrimSpace(const std::string& str, const std::string& ws = " \t") {
	const auto start = str.find_first_not_of(ws);
	if( start == std::string::npos )
		return ""; // Nothing
	const auto end = str.find_last_not_of(ws);
	const auto len = end - start + 1;
	return str.substr(start, len);
}

std::string ReplaceAll(const std::string& str, const std::string& search, const std::string& repl) {
	std::string ret(str);

	size_t ind = 0;
	while( ind < str.length() ) {
		ind = ret.find(search, ind);
		if( ind == std::string::npos )
			break;
		ret.replace(ind, search.length(), repl);
		ind += repl.length();
	}

	return ret;
}

namespace mos6502 {
	std::optional<progptr> Program::CompileFile(const std::string& filepath) {
		progptr program = std::make_shared<Program>();
		if( program->CompileSourceFile(filepath) )
			return { program };
		return std::nullopt;
	}

	bool Program::CompileSourceFile(const std::string& filepath) {
		const std::filesystem::path path(filepath);

		//Verify the file exists
		if( std::filesystem::exists(path) == false ) {
			std::cerr << "mos6502::Program::CompileSourceFile system reports that the file \"" << filepath << "\" does not exist" << std::endl;
			return false;
		}

		m_Path = path.string();
		m_Name = path.stem().string();

		//Start reading the contents into a string using classic C fread

		// Get the file size now for use later
		const long fileSize = static_cast<long>(std::filesystem::file_size(path));
		if( fileSize <= 0 ) {
			std::cerr << "mos6502::Program::CompileSourceFile the supplied file \"" << filepath << "\" is empty" << std::endl;
			return false;
		}

		std::string sourceData;
		{ // C-style file reading
			// Lambda for closing a file
			const auto closeFile = [](FILE* file) { fclose(file); };

			FILE* file = nullptr;

			// Open the file, checking for errors as we go
			errno_t err = fopen_s(&file, path.string().c_str(), "rb");
			if( err || !file ) {
				std::cerr << "mos6502::Program::CompileSourceFile failed to open the file \"" << filepath << "\"" << std::endl;
				return false;
			}

			auto handle = std::unique_ptr<FILE, decltype(closeFile)>(file, closeFile);
			if( !handle ) {
				std::cerr << "mos6502::Program::CompileSourceFile failed to open a handle to the file \"" << filepath << "\"" << std::endl;
				return false;
			}

			// Make sure we got the data reserved for this
			sourceData.resize(fileSize);

			// Read the contents into the string directly.
			// We should make the pointer const for fread, C++17 doesn't do that with data()
			fread(const_cast<char*>(sourceData.data()), 1, fileSize, handle.get());

		}; //file handle is cleared

		return CompileString(sourceData); // Was ok, pass onto compilation
	}

	bool Program::CompileString(const std::string& source) {
		/*
			NOTES!
			- Need to do dual pass, where the labels and symbols are extracted
			first, then can be used within the expressions
			- Support for basic directives
			- Support for CHAR types
			- Possible expression support?
		*/

		//Reset everything needed
		m_SourceCode.clear();

		// Setup needed variables
		unsigned int lineNumber = 1;
		word pcOffset = 0x0200, startingPCOffset = 0x0200;
		std::string tempStr, comment, symbol;
		Instruction instruction = Instruction::ILL;
		bool setStartingPC = false;

		std::map<std::string, word> symbols;

		enum class Mode {
			IDLE,
			COMMENT,
			SYMBOL,
			ASSIGNMENT,
			INSTRUCTION,
			DIRECTIVE
		} mode = Mode::IDLE;

		auto replaceSymbols = [&](const std::string& str) -> std::string {
			std::string ret(str);
			for( const auto& [key, val] : symbols ) {
				ret = ReplaceAll(ret, key, std::to_string(val));
			}
			return ret;
		};

		auto parseValue = [&](const std::string& str) -> word {
			std::string ret = replaceSymbols(str);

			auto ltr = ret.at(0);
			if( ltr == '$' ) { //Hex
				return static_cast<word>(std::stoul(ret.substr(1), nullptr, 16));
			} else if( ltr == '%' ) { //Binary
				return static_cast<word>(std::stoul(ret.substr(1), nullptr, 2));
			} else if( ret == "*" ) { // Current PC
				return pcOffset;
			}

			word val = 0;
			try {
				 val = static_cast<word>(std::stoul(ret));
			} catch(std::invalid_argument ex) {
				std::cerr << "failed to parse value \"" << str << "\" on line " << lineNumber << std::endl;
			}

			return val;
		};

		const auto handleToken = [&](const std::string& token) {
			if( !symbol.empty() ) {
				// Hanging symbol needs to be a label now
				symbols.emplace(symbol, pcOffset);
				symbol = "";
			}

			auto inst = MnmuemonicToInstruction(token);
			if( inst != Instruction::ILL ) {
				mode = Mode::INSTRUCTION;
				instruction = inst;
			} else {
				mode = Mode::SYMBOL;

				symbol = token;
			}
		};

		const auto parseInstruction = [&](const std::string& str) {
			// Check if we haven't set the starting PC offset (first instruction)
			if( !setStartingPC )
				m_StartingPCOffset = pcOffset;

			word instrPC = pcOffset;

			//Increment PC by one initially
			pcOffset++;

			std::string addrStr = replaceSymbols(TrimSpace(str));

			AddressMode addressing = AddressMode::ILL;
			word value = 0;

			if( addrStr.empty() ) {
				addressing = AddressMode::IMP;
			} else if( addrStr == "A" ) {
				addressing = AddressMode::ACC;
			} else {
				const char& ltr = addrStr.at(0);
				if( ltr == '#' ) { // Immediate
					addressing = AddressMode::IMM;
					value = parseValue( addrStr.substr(1));

					pcOffset++;
					if( GET_HIGH_BYTE(value) > 0 ) pcOffset++;
				} else if( ltr == '*' || ltr == '+' || ltr == '-' ) { // Relative
					addressing = AddressMode::REL;
					if( ltr == '*' )
						value = parseValue(addrStr.substr(1));
					else
						value = parseValue(addrStr);
					value = GET_LOW_BYTE(value);
					pcOffset++;
				} else if( ltr == '(' ) { // Indirect
					auto ltrCom = addrStr.find(',', 1);
					auto ltrPar = addrStr.find(')', 1);
					if( ltrPar == std::string::npos ) { // No closing parenthasis
						//Invalid indirect value
						std::cerr << "invalid indirect address on line " << std::to_string(lineNumber) << std::endl;
					} else  if( ltrCom != std::string::npos ) { // Contains a comma (INX, INY)
						if( ltrCom < ltrPar ) { // Comma before parenthasis means INX
							addressing = AddressMode::INX;
							value = parseValue(addrStr.substr(1, ltrCom - 1));
						} else { // Comma after parenthasis means INY
							addressing = AddressMode::INY;
							value = parseValue(addrStr.substr(1, ltrPar - 1));
						}
						pcOffset++;
					} else { // No comma, just IND
						addressing = AddressMode::IND;
						value = parseValue(addrStr.substr(1, ltrPar - 1));

						pcOffset += 2;
					}
				} else { // Either absolute or zero-page
					auto ltrCom = addrStr.find(',');
					if( ltrCom == std::string::npos ) { // No comma means no X/Y offset
						value = parseValue(addrStr);
						if( GET_HIGH_BYTE(value) != 0 ) {
							addressing = AddressMode::ABS;
							pcOffset += 2;
						} else {
							addressing = AddressMode::ZPG;
							pcOffset++;
						}
					} else {
						value = parseValue(addrStr.substr(0, ltrCom));
						bool isX = addrStr.at(addrStr.length() - 1) == 'X';
						if( GET_HIGH_BYTE(value) != 0 ) {
							addressing = isX ? AddressMode::ABX : AddressMode::ABY;
							pcOffset += 2;
						} else {
							addressing = isX ? AddressMode::ZPX : AddressMode::ZPY;
							pcOffset++;
						}
					}
				}
			}

			//Branching optimization
			if( instruction == Instruction::BCC
				|| instruction == Instruction::BCS
				|| instruction == Instruction::BEQ
				|| instruction == Instruction::BMI
				|| instruction == Instruction::BNE
				|| instruction == Instruction::BPL
				|| instruction == Instruction::BVC
				|| instruction == Instruction::BVS ) {
				//If we can set to relative, which in almost every case we can,
				//then we should
				int diff = static_cast<int>(startingPCOffset) - static_cast<int>(instrPC);
				if( diff >= -126 && diff <= 129 ) {
					//Make a signed byte of the relative offset
					byte rel = diff < 0 ? (256 + diff) & 0xFF : diff & 0x7F;
					addressing = AddressMode::REL; 
					value = rel - 2; //Offset by the cycles to execute this instruction
				}
			}
			
			// Find the matching instruction details for the parsed values.
			const auto instrDetail = FindInstructionDetail(instruction, addressing);
			if( instrDetail.instruction != Instruction::ILL ) {
				//Write a program line, notice we only keep actionable lines
				m_SourceCode.push_back(Line{
						lineNumber,
						instrPC,
						"",			//Raw Source
						"",			//Comment
						instrDetail.opCode,
						instrDetail.instruction,
						instrDetail.addressing,
						value
					});

				// Write the byte code
				m_ByteCode.push_back(instrDetail.opCode);
				if( instrDetail.addressing != AddressMode::IMP && instrDetail.addressing != AddressMode::ACC ) {
					m_ByteCode.push_back(GET_LOW_BYTE(value));
					if( GET_HIGH_BYTE(value) != 0 )
						m_ByteCode.push_back(GET_HIGH_BYTE(value));
				}

				setStartingPC = true;
			}
		};

		const auto completeOp = [&]() {
			if( mode == Mode::ASSIGNMENT ) {
				const auto value = TrimSpace(tempStr);

				if( symbol == "*" ) {
					pcOffset = parseValue(value);
				} else {
					auto wordval = parseValue(value);
					symbols.emplace(symbol, wordval);
				}

				symbol = "";
			} else if( mode == Mode::INSTRUCTION ) {
				tempStr = replaceSymbols(tempStr);
				parseInstruction(tempStr);
			} else if( mode == Mode::SYMBOL ) {
				symbols.emplace(symbol, pcOffset);
				symbol = "";
			} else if( !tempStr.empty() ) {
				handleToken(tempStr);

				if( mode == Mode::INSTRUCTION ) // Hanging instruction
					parseInstruction("");
			}
		};

		// Walk each character at a time
		// to effectively normalize the source code
		// into an easier way to aggregate.
		// "Tokenizing"
		for(const char& c : source ) {
			// Ignore carriage return for new-lines
			if( c == '\r' ) continue;

			//New lines effectively force the instruction
			//to be complete.
			if( c == '\n' ) {
				if( mode == Mode::COMMENT ) {
					//Clean first leading space if it exists
					const auto letter = tempStr.at(0);
					if( letter == ' ' || letter == '\t' )
						tempStr = tempStr.substr(1);
				} else {
					completeOp();

					if( mode == Mode::SYMBOL ) {
						symbols.emplace(symbol, pcOffset);
					}
				}

				// Increment the line number
				lineNumber++;

				// Reset the states we need to
				mode = Mode::IDLE;
				tempStr = "";
				symbol = "";
				instruction = Instruction::ILL;

				continue;
			} 

			// Always handle comments first
			if( mode == Mode::COMMENT ) {
				tempStr.push_back(c);
			} else if( c == ';' ) {
				completeOp();

				mode = Mode::COMMENT;
				tempStr = "";
			} else if( mode == Mode::ASSIGNMENT || mode == Mode::INSTRUCTION ) {
				tempStr.push_back(std::toupper(c));
			} else if( c == ' ' || c == '\t' ) {
				// If there is something in the temp str,
				// then a token has just broken (changed)
				if( !tempStr.empty() )
					handleToken(tempStr);
				tempStr = "";
			} else if( c == '=' ) {
				// We are starting an assignment
				if( !tempStr.empty() )
					symbol = tempStr;
				mode = Mode::ASSIGNMENT;
				tempStr = "";
			} else if( c == ':' ) { //Explicit label
				symbols.emplace(tempStr, pcOffset);

				mode = Mode::IDLE;
				tempStr = "";
			} else {
				tempStr.push_back(std::toupper(c));
			}
		}

		if( !tempStr.empty() || !symbol.empty() )
			completeOp();

		return true;
	}
}