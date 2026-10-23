#ifndef CHIP8EMULATOR
#define CHIP8EMULATOR

// Platform Libraries
#ifdef WINDOWS_BUILD
#include <windows.h>
#endif

// C++ libraries
#include <cstdint>
#include <fstream>
#include <random>

#include "../utils/logger.h"
#include"../input/input.h"

#include "../utils/miniaudio.h"

class Chip8Emulator : public Input
{
    public:
        #ifdef WINDOWS_BUILD
        HWND window;
        #endif
        uint8_t gfx[64 * 32];
        void Chip8Init();
        void LoadROM(char const* filename);
		void cycle();
        void tickTimers();
		void onKeyDown(uint8_t chip8Key) override;
		void onKeyUp(uint8_t chip8Key) override;
		void ShutdownSound();

    private:
        const static unsigned int START_ADDRESS = 0x200;
        const unsigned int FONTSET_START_ADDRESS = 0x50;

		ma_engine audioEngine;
		ma_sound beepSound;
		ma_waveform waveform;
		bool soundInitialized = false;
		bool InitSound();


		bool waitingForKey = false;
		bool keyReleased = true;
		uint8_t waitingRegister = 0;

        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t I{};
	    uint16_t pc{};
	    uint16_t stack[16]{};
	    uint8_t sp{};
	    uint8_t delayTimer{};
	    uint8_t soundTimer{};
	    uint8_t keypad[16]{};
		uint8_t stagingKeypad[16]{};
	    uint16_t opcode;
		std::default_random_engine rng;

        typedef void (Chip8Emulator::*Chip8Func)();
	    Chip8Func table[0xF + 1];
	    Chip8Func table0[16];
	    Chip8Func table8[16];
	    Chip8Func tableE[16];
	    Chip8Func tableF[256];

		void Table0();
		void Table8();
		void TableE();
		void TableF();
		void OP_NULL();

		void OP_00E0();
		void OP_00EE();

		void OP_1nnn();
		void OP_2nnn();
		void OP_3xkk();
		void OP_4xkk();
		void OP_5xy0();
		void OP_6xkk();
		void OP_7xkk();

		void OP_9xy0();
		void OP_Annn();
		void OP_Bnnn();
		void OP_Cxkk();
		void OP_Dxyn();
		
		void OP_8xy0();
		void OP_8xy1();
		void OP_8xy2();
		void OP_8xy3();
		void OP_8xy4();
		void OP_8xy5();
		void OP_8xy6();
		void OP_8xy7();
		void OP_8xyE();

		void OP_ExA1();
		void OP_Ex9E();

		void OP_Fx07();
		void OP_Fx0A();
		void OP_Fx15();
		void OP_Fx18();
		void OP_Fx1E();
		void OP_Fx29();
		void OP_Fx33();
		void OP_Fx55();
		void OP_Fx65();

		
};




#endif // !CHIP8EMULATOR
