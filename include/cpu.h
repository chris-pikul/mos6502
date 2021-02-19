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
#include <array>
#include <set>
#include <memory>

#include "types.h"
#include "instructions.h"
#include "io_device.h"
#include "utils.h"

namespace mos6502 {

	// Emulates the CPU portion of the MOS6502 processor.
	class CPU : public IODevice {
	public:	// TYPES

		// Processor Status:
		// Represents the current processor status as an 8-bit bitfield value.
		// This status contains bits for each flag of the processor status.
		// Compressed as:
		//   0 1 2 3 4 5 6 7
		//   C Z I D B V N -
		//
		union Status {
			struct {
				// Carry Flag:
				// The carry flag is set to 1 if the last operation caused an overflow
				// fom bit 7 of the result, or an underflow from bit 0.
				// Can be manually set with 'SEC', or cleared with 'CLC'
				bool C : 1;

				// Zero Flag:
				// Will be 1 if the last operation resulted in 0.
				bool Z : 1;

				// Interrupt Disable:
				// If set to 1, the processor will not respond to interrupts.
				// Can be enabled with 'SEI', or cleared with 'CLI'.
				bool I : 1;

				// Decimal Mode:
				// While enabled, the processor will use Binary Coded Decimal (BCD)
				// arithmetic.
				// Can be enabled with 'SED', or cleared with 'CLD'.
				bool D : 1;

				// Break Command:
				// Will be 1 when a 'BRK' instruction has been executed and an
				// interrupt has been generated.
				bool B : 1;

				// Unused:
				// This value is not technically used in the processor.
				bool U : 1;

				// Overflow Flag:
				// Will be set if an arithmetic operations resulted in a byte-overflow.
				// Ie. The number rolled over and became negative.
				bool V : 1;

				// Negative Flag:
				// Will be set if the last result was a negative value.
				// Determined by bit 7 being 1.
				bool N : 1;
			};

			byte value;

			Status() = default;
			Status(int v) : value(v) {} //Allow for assignment
			Status(bool c, bool z, bool i, bool d, bool b, bool u, bool v, bool n)
				: C(c), Z(z), I(i), D(d), B(b), U(u), V(v), N(n) {} // Struct constructor

			// Formats the status as a string of the flag symbols.
			// Uppercase letters represent the flag position is on.
			// The left most character is the least signifigant bit.
			friend std::ostream& operator<<(std::ostream& os, const Status& s) {
				os << "["
					<< (s.C ? 'C' : 'c')
					<< (s.Z ? 'Z' : 'z')
					<< (s.I ? 'I' : 'i')
					<< (s.D ? 'D' : 'd')
					<< (s.B ? 'B' : 'b')
					<< (s.U ? 'U' : 'u')
					<< (s.V ? 'V' : 'v')
					<< (s.N ? 'N' : 'n')
					<< "]";
				return os;
			}
		};

		// Provided as an alternative means to figuring out the status bitfield.
		// Each flag is represented here as a single bit in a byte.
		enum class StatusFlag : byte {
			CARRY		= (1 << 0),	// Carry Bit
			ZERO		= (1 << 1), // Zero Flag
			INTERRUPT	= (1 << 2), // Interrupt Disable
			DECIMAL		= (1 << 3), // Decimal Mode
			BREAK		= (1 << 4), // Break
			UNUSED		= (1 << 5), // Ignored (un-used)
			INT_OVERFLOW= (1 << 6), // Overflow Flag
			NEGATIVE	= (1 << 7), // Negative Flag
		};

	public:

		// Construct using an IODevice pointer for mapping to memory/hardware
		CPU(ioptr bus);

		inline void MountBus(ioptr bus) { m_Bus.swap(bus); }

		// Reset Interrupt:
		// Forces the CPU into a known state.
		// This process includes the following steps:
		//  - Set registers A, X, and Y to 0.
		//  - Set program counter to 0xFFFC
		//  - Set stack pointer to 0xFD
		//  - The processor status is cleared except for the unsused bit (5)
		// This event represents a physical pin on the hardware.
		virtual void Reset();

		// Interrupt Request:
		// 
		virtual void IRQ();

		// Non-maskable Interrupt:
		// 
		virtual void NMI();

		// Tick, performs a single clock cycle.
		// Will return true if there are remaining
		// cycles for the instruction.
		virtual bool Tick();

		friend std::ostream& operator<<(std::ostream& os, const CPU& c) {
			os << "PS=" << c.m_ProcStatus;
			os << " PC=" << address(c.m_PC);
			os << " SP=" << Hex(c.m_SP);
			os << " A=" << Hex(c.m_Acc);
			os << " X=" << Hex(c.m_X);
			os << " Y=" << Hex(c.m_Y);
			os << " : CR=" << Hex(c.m_CyclesRem);
			os << " : CE=" << ToHex<unsigned int>(c.m_CyclesExecuted);
			return os;
		}

	public:		// REGISTERS

		// Returns the current Program Counter
		inline const word GetProgramCounter() const { return m_PC; }

		// Returns the current Stack Pointer
		inline const byte GetStackPointer() const { return m_SP; }

		// Returns the current Accumulator
		inline const byte GetAccumulator() const { return m_Acc; }

		// Returns the current X Register
		inline const byte GetX() const { return m_X; }

		// Returns the current Y Register
		inline const byte GetY() const { return m_Y; }

		// Returns the current Processor Status
		inline const Status GetStatus() const { return m_ProcStatus; }

		// Returns the specified flag bit as a byte value of 1 or 0.
		inline const byte GetStatusFlag(const StatusFlag f) const {
			return m_ProcStatus.value & static_cast<byte>(f) ? 1 : 0;
		}

		// Checks (returns true) if the current Processor Status has the given flag set
		inline bool HasStatusFlag(const StatusFlag f) const { 
			return (m_ProcStatus.value & static_cast<byte>(f)); 
		}

		// Sets the given flag to the status provided. Assumed default is to be set true/on.
		inline void SetStatusFlag(const StatusFlag f, const bool v = true) { 
			if (v) m_ProcStatus.value |= static_cast<byte>(f);
			else m_ProcStatus.value &= ~(static_cast<byte>(f));
		}

	protected:	// REGISTERS

		// Program Counter:
		// 16-bit register pointing to the next instruction for execution.
		// This value is automatically incremented when an instruction is executed,
		// but may be modified manually for jumps or interupts.
		word m_PC;

		// Stack Pointer:
		// Holds the lower 8-bits of the next free location on the stack.
		// The stack is generally mapped to 0x0100 - 0x01FF on the and represents
		// 256 bytes of memory.
		// Pushing bytes to the stack will cause the SP to decrement.
		// Pulling bytes to the stack will increment the SP.
		byte m_SP;

		// Accumulator:
		// 8-bits used in all math and logical operations.
		// Contents of which can be on either stack, or memory.
		byte m_Acc;

		// Registers X, and Y:
		// 8-bit memory register.
		byte m_X, m_Y;

		// Processor Status:
		// Represents the current processor status as an 8-bit bitfield value.
		Status m_ProcStatus;

	public: // Address modes

		virtual address ExecuteAddressing(const AddressMode addrMode, fast_byte& outCycles);

	protected:
		// Absolute:
		// References an absolute 16-bit address.
		virtual address Addr_ABS(fast_byte& outCycles);

		// Absolute w/X offset:
		// References an absolute 16-bit address.
		// Takes the incoming address and adds the 8-bit X register.
		virtual address Addr_ABX(fast_byte& outCycles);

		// Absolute w/Y offset:
		// References an absolute 16-bit address.
		// Takes the incoming address and adds the 8-bit Y register.
		virtual address Addr_ABY(fast_byte& outCycles);

		// Accumulator:
		// Work directly with the accumulator register
		virtual address Addr_ACC(fast_byte& outCycles);

		// Immediate:
		// The value required is directly supllied (literal)
		virtual address Addr_IMM(fast_byte& outCycles);

		// Implied:
		// Essentially no addressing is needed.
		// For simplicity, instructions using the accumulator
		// share this addressing mode. References are hard
		// at explaining if there is a difference at all.
		virtual address Addr_IMP(fast_byte& outCycles);

		// Indirect:
		// Takes a 16-bit address which identifies the location
		// containing the real 16-bit target address.
		// Used with JMP.
		virtual address Addr_IND(fast_byte& outCycles);

		// Indexed Indirect:
		// Uses an address table on the zero page, and
		// adds the value at the X register (with zero-page wrapping)
		// to resolve the target address.
		virtual address Addr_INX(fast_byte& outCycles);

		// Indirect Indexed:
		// Instruction takes an 8-bit zero-page location, and
		// adds the Y register to resolve the final target address.
		virtual address Addr_INY(fast_byte& outCycles);

		// Relative:
		// Uses a signed 8-bit (-128 to +127) relative offset which is
		// added to the program counter (PC).
		// Used with BEQ and BNE branch instructions.
		//
		// Note: Because the PC is incremented by 2 during these instructions,
		// the effective range must be within -126 to +129 bytes of the
		// initial branch instruction.
		virtual address Addr_REL(fast_byte& outCycles);

		// Zero Page (no offset):
		// References the first page of memory ($0000 - $00FF)
		virtual address Addr_ZPG(fast_byte& outCycles);

		// Zero Page w/X offset:
		// References the first page of memory ($0000 - $00FF)
		// but offsets (+) using the X register.
		// This value wraps if $FF is exceeded.
		virtual address Addr_ZPX(fast_byte& outCycles);

		// Zero Page w/Y offset:
		// References the first page of memory ($0000 - $00FF)
		// but offsets (+) using the Y register.
		// This value wraps if $FF is exceeded.
		// Only used with LDX and STX instructions.
		virtual address Addr_ZPY(fast_byte& outCycles);

	public: // Instructions

		virtual fast_byte ExecuteInstruction(const Instruction& inst, const address& addr);

	protected:

		// Performs the actual branch operation
		virtual fast_byte Branch(const address& addr);

		// Add with Carry
		virtual fast_byte Ins_ADC(const address& addr);

		// Logical AND
		virtual fast_byte Ins_AND(const address& addr);

		// Arithmetic Shift Left
		virtual fast_byte Ins_ASL(const address& addr);

		// Branch If Carry Flag Clear
		virtual fast_byte Ins_BCC(const address& addr);

		// Branch If Carry Flag Set
		virtual fast_byte Ins_BCS(const address& addr);

		// Branch If Zero Flag Set
		virtual fast_byte Ins_BEQ(const address& addr);

		// Bit Test
		virtual fast_byte Ins_BIT(const address& addr);

		// Branch If Negative Flag Set
		virtual fast_byte Ins_BMI(const address& addr);

		// Branch If Zero Flag Clear
		virtual fast_byte Ins_BNE(const address& addr);

		// Brance If Negative Flag Clear
		virtual fast_byte Ins_BPL(const address& addr);

		// Break for Interrupt
		virtual fast_byte Ins_BRK(const address& addr);

		// Branch If Overflow Flag Clear
		virtual fast_byte Ins_BVC(const address& addr);

		// Branch If Overflow Flag Set
		virtual fast_byte Ins_BVS(const address& addr);

		// Clear Carry Flag
		virtual fast_byte Ins_CLC(const address& addr);

		// Clear Decimal Mode Flag
		virtual fast_byte Ins_CLD(const address& addr);

		// Clear Interrupt Disable Flag
		virtual fast_byte Ins_CLI(const address& addr);

		// Clear Overflow Flag
		virtual fast_byte Ins_CLV(const address& addr);

		// Compare Accumulator
		virtual fast_byte Ins_CMP(const address& addr);

		// Compare X Register
		virtual fast_byte Ins_CPX(const address& addr);

		// Compare Y Register
		virtual fast_byte Ins_CPY(const address& addr);

		// Decrement a Memory Location
		virtual fast_byte Ins_DEC(const address& addr);

		// Decrement X Register
		virtual fast_byte Ins_DEX(const address& addr);

		// Decrement Y Register
		virtual fast_byte Ins_DEY(const address& addr);

		// Logical Exclusive OR
		virtual fast_byte Ins_EOR(const address& addr);

		// Increment a Memory Location
		virtual fast_byte Ins_INC(const address& addr);

		// Increment X Register
		virtual fast_byte Ins_INX(const address& addr);

		// Increment Y Register
		virtual fast_byte Ins_INY(const address& addr);

		// Jump to Location
		virtual fast_byte Ins_JMP(const address& addr);

		// Jump to Subroutine
		virtual fast_byte Ins_JSR(const address& addr);

		// Load Accumulator
		virtual fast_byte Ins_LDA(const address& addr);

		// Load X Register
		virtual fast_byte Ins_LDX(const address& addr);

		// Load Y Register
		virtual fast_byte Ins_LDY(const address& addr);

		// Logical Shift Right
		virtual fast_byte Ins_LSR(const address& addr);

		// No Operation
		virtual fast_byte Ins_NOP(const address& addr);

		// Logical Inclusive OR
		virtual fast_byte Ins_ORA(const address& addr);

		// Push Accumulator Onto Stack
		virtual fast_byte Ins_PHA(const address& addr);

		// Push Processor Status Onto Stack
		virtual fast_byte Ins_PHP(const address& addr);

		// Pull Accumulator From Stack
		virtual fast_byte Ins_PLA(const address& addr);

		// Pull Processor Status From Stack
		virtual fast_byte Ins_PLP(const address& addr);

		// Rotate Left
		virtual fast_byte Ins_ROL(const address& addr);

		// Rotate Right
		virtual fast_byte Ins_ROR(const address& addr);

		// Return from Interrupt
		virtual fast_byte Ins_RTI(const address& addr);

		// Return from Subroutine
		virtual fast_byte Ins_RTS(const address& addr);

		// Subtract with Carry
		virtual fast_byte Ins_SBC(const address& addr);

		// Set Carry Flag
		virtual fast_byte Ins_SEC(const address& addr);

		// Set Decimal Mode Flag
		virtual fast_byte Ins_SED(const address& addr);

		// Set Interrupt Disable Flag
		virtual fast_byte Ins_SEI(const address& addr);

		// Set Accumulator
		virtual fast_byte Ins_STA(const address& addr);

		// Set X Register
		virtual fast_byte Ins_STX(const address& addr);

		// Set Y Register
		virtual fast_byte Ins_STY(const address& addr);

		// Transfer Accumulator to X Register
		virtual fast_byte Ins_TAX(const address& addr);

		// Transfer Accumulator to Y Register
		virtual fast_byte Ins_TAY(const address& addr);

		// Transfer Stack Pointer (SP) to X Register
		virtual fast_byte Ins_TSX(const address& addr);

		// Transfer X Register to Accumulator
		virtual fast_byte Ins_TXA(const address& addr);

		// Transfer X Register tp Stack Pointer (SP)
		virtual fast_byte Ins_TXS(const address& addr);

		// Transfer Y Register to Accumulator
		virtual fast_byte Ins_TYA(const address& addr);

	protected: // Bus and IO systems

		inline byte ReadByte(const address& addr) const override;
		inline word ReadWord(const address& addr) const override;
		inline void WriteByte(const address& addr, const byte data) override;
		inline void WriteWord(const address& addr, const word data) override;
		inline void WriteBytes(const address& addr, const std::vector<byte>& bytes) override;

		ioptr m_Bus;

	protected: // General

		// Number of clock cycles remaining on the last operation.
		fast_byte m_CyclesRem = 0;

		// Number of clock cycles executed since object inseption.
		unsigned int m_CyclesExecuted = 0;

		// Whether the addressing mode had the value supplied
		bool m_WasSupplied = false;

		// The value that the instruction provided when implied
		byte m_SuppliedValue = 0;
		
		// Clears and resets the internal variables for supplied data
		inline void ClearSupplied() {
			m_WasSupplied = false;
			m_SuppliedValue = 0;
		}

		// Sets the supplied value to the provided, and enables the boolean switch
		inline void SetSupplied(const byte value) {
			m_WasSupplied = true;
			m_SuppliedValue = value;
		}

		// Fetches the data required for an instruction.
		// This either uses the supplied address, or the saved
		// value provided in m_SuppliedValue.
		// If the m_WasSupplied flag is true, then the member
		// variable value is preferred instead of lookup.
		byte FetchData(const address& addr) const;

		// Writes a byte to the stack at the pointer location offset by
		// 0x0100 (the stack page) and then decrements the stack pointer.
		void PushToStack(const byte data);

		byte PullFromStack();

	}; // class CPU

}; // namespace mos6502
