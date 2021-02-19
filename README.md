# MOS-6502 Emulator

A simple, but effective, emulator for the MOS Technology 6502  8-bit processor.
This processor was featured in many famous machines, including (but not limited to)
the: Apple I, Apple II, Atari 2600, Atari 5200, BBC Micro,  Commodore 64,
Nintendo Family Computer (Famicon), Nintendo Entertainment System (NES),
TurboGrafx-16, and many more. Launched in 1975, it remained in usage for decades,
and has since cemented itself as legendary.

## About This Emulator

Writting this emulator was a passion-project for me. I started it with the motivation
of potentially using this in a full NES emulator. Additionally, I wanted to test my
C++ knowledge and learn more about 8-bit systems and how embedded hardware used tp work.

For the code-layout, I wanted to do a more "forward-thinking" way of building my classes.
It should resemble a compartmentalized version of the physical hardware with the bus,
memory, cpu, and program aspects all being separate classes. Additionally, the bus and
memory classes implement a abstract base class (IODevice) so that future work can expand
and replace the basic systems I have written here, seamlessly.

## Installation & Running

Since this is a toy/hobby project, it features a `main.cpp` file to run the emulation.
But nothing is stopping someone from compiling it as a library (static or dynamic) and
using it that way.

The only dependency should be a C++17 capable compiler with the standard library (STD)
available.

This was created with Microsoft Visual Studio 2019, so opening the solution file
`mos6502.sln` should take care of everything for you and you can build/run from there.

### Porting considerations

In the MSVS project, the `include/` folder is in the search path, so the CPP files
do not explicitly specify the folder locations.
