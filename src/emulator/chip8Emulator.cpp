#include "chip8Emulator.h"

void Chip8Emulator::Chip8Init()
{
	pc = START_ADDRESS;

	std::random_device rd;
    rng = std::default_random_engine(rd());

	const unsigned int FONTSET_SIZE = 80;

	uint8_t fontset[FONTSET_SIZE] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Set up function pointer table
		table[0x0] = &Chip8Emulator::Table0;
		table[0x1] = &Chip8Emulator::OP_1nnn;
		table[0x2] = &Chip8Emulator::OP_2nnn;
		table[0x3] = &Chip8Emulator::OP_3xkk;
		table[0x4] = &Chip8Emulator::OP_4xkk;
		table[0x5] = &Chip8Emulator::OP_5xy0;
		table[0x6] = &Chip8Emulator::OP_6xkk;
		table[0x7] = &Chip8Emulator::OP_7xkk;
		table[0x8] = &Chip8Emulator::Table8;
		table[0x9] = &Chip8Emulator::OP_9xy0;
		table[0xA] = &Chip8Emulator::OP_Annn;
		table[0xB] = &Chip8Emulator::OP_Bnnn;
		table[0xC] = &Chip8Emulator::OP_Cxkk;
		table[0xD] = &Chip8Emulator::OP_Dxyn;
		table[0xE] = &Chip8Emulator::TableE;
		table[0xF] = &Chip8Emulator::TableF;

		for (size_t i = 0; i <= 0xE; i++)
		{
			table0[i] = &Chip8Emulator::OP_NULL;
			table8[i] = &Chip8Emulator::OP_NULL;
			tableE[i] = &Chip8Emulator::OP_NULL;
		}

		table0[0x0] = &Chip8Emulator::OP_00E0;
		table0[0xE] = &Chip8Emulator::OP_00EE;

		table8[0x0] = &Chip8Emulator::OP_8xy0;
		table8[0x1] = &Chip8Emulator::OP_8xy1;
		table8[0x2] = &Chip8Emulator::OP_8xy2;
		table8[0x3] = &Chip8Emulator::OP_8xy3;
		table8[0x4] = &Chip8Emulator::OP_8xy4;
		table8[0x5] = &Chip8Emulator::OP_8xy5;
		table8[0x6] = &Chip8Emulator::OP_8xy6;
		table8[0x7] = &Chip8Emulator::OP_8xy7;
		table8[0xE] = &Chip8Emulator::OP_8xyE;
		
		tableE[0x1] = &Chip8Emulator::OP_ExA1;
		tableE[0xE] = &Chip8Emulator::OP_Ex9E;

		for (size_t i = 0; i <= 0x65; i++)
		{
			tableF[i] = &Chip8Emulator::OP_NULL;
		}

		tableF[0x07] = &Chip8Emulator::OP_Fx07;
		tableF[0x0A] = &Chip8Emulator::OP_Fx0A;
		tableF[0x15] = &Chip8Emulator::OP_Fx15;
		tableF[0x18] = &Chip8Emulator::OP_Fx18;
		tableF[0x1E] = &Chip8Emulator::OP_Fx1E;
		tableF[0x29] = &Chip8Emulator::OP_Fx29;
		tableF[0x33] = &Chip8Emulator::OP_Fx33;
		tableF[0x55] = &Chip8Emulator::OP_Fx55;
		tableF[0x65] = &Chip8Emulator::OP_Fx65;

}

void Chip8Emulator::LoadROM(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
	}

	else
	{
		LOG_FATAL(L"Failed to load ROM");
	}

}

void Chip8Emulator::Table0()
	{
		((*this).*(table0[opcode & 0x000Fu]))();
	}
	
void Chip8Emulator::Table8()
	{
		((*this).*(table8[opcode & 0x000Fu]))();
	}
	 
void Chip8Emulator::TableE()
	{
		((*this).*(tableE[opcode & 0x000Fu]))();
	}
	 
void Chip8Emulator::TableF()
	{
		((*this).*(tableF[opcode & 0x00FFu]))();
	}
	 
void Chip8Emulator::OP_NULL()
	{}

void Chip8Emulator::cycle()
{
	memcpy(keypad, stagingKeypad, sizeof(keypad));
	
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Decode and Execute
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}

}

void Chip8Emulator::OP_00E0()
{
	std::fill((uint8_t*)&gfx, (uint8_t*)&gfx + sizeof(gfx), 0);
	pc += 2;
}

void Chip8Emulator::OP_00EE()
{
	--sp;
	pc = stack[sp];
	pc += 2;

}

void Chip8Emulator::OP_1nnn()
{
	pc = opcode & 0x0FFF;
}

void Chip8Emulator::OP_2nnn()
{
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFF;

}

void Chip8Emulator::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if (registers[Vx] == (opcode & 0x00FFu))
	{
		pc += 2;
	}

	pc += 2;
}

void Chip8Emulator::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if (registers[Vx] != (opcode & 0x00FFu))
	{
		pc += 2;
	}

	pc += 2;
}

void Chip8Emulator::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}

	pc += 2;
}

void Chip8Emulator::OP_6xkk()
{
	registers[(opcode & 0x0F00u) >> 8u] = (opcode & 0x00FFu);
	pc += 2;
}

void Chip8Emulator::OP_7xkk()
{
	registers[(opcode & 0x0F00u) >> 8u] += (opcode & 0x00FFu);
	pc += 2;
}

void Chip8Emulator::OP_8xy0()
{
	registers[(opcode & 0x0F00u) >> 8u] = registers[(opcode & 0x00F0u) >> 4u];
	pc += 2;
}

void Chip8Emulator::OP_8xy1()
{
	registers[(opcode & 0x0F00u) >> 8u] |= registers[(opcode & 0x00F0u) >> 4u];
	pc += 2;
}

void Chip8Emulator::OP_8xy2()
{
	registers[(opcode & 0x0F00u) >> 8u] &= registers[(opcode & 0x00F0u) >> 4u];
	pc += 2;
}

void Chip8Emulator::OP_8xy3()
{
	registers[(opcode & 0x0F00u) >> 8u] ^= registers[(opcode & 0x00F0u) >> 4u];
	pc += 2;
}

void Chip8Emulator::OP_8xy4()
{
	uint16_t sum = registers[(opcode & 0x0F00u) >> 8u] + registers[(opcode & 0x00F0u) >> 4u];
	
	if (sum > UINT8_MAX)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[(opcode & 0x0F00u) >> 8u] = (sum & 0xFFu);
	pc += 2;
}

void Chip8Emulator::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
	pc += 2;
}

void Chip8Emulator::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
	pc += 2;
}

void Chip8Emulator::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
	pc += 2;
}

void Chip8Emulator::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x80u)>> 7u;

	registers[Vx] <<= 1;
	pc += 2;
}

void Chip8Emulator::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}

	pc += 2;
}

void Chip8Emulator::OP_Annn()
{
	I = (opcode & 0x0FFFu);
	pc += 2;
}

void Chip8Emulator::OP_Bnnn()
{
	pc = (opcode & 0x0FFFu) + registers[0];
}

void Chip8Emulator::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    // Generate a random 8-bit number (0 to 255)
    std::uniform_int_distribution<uint32_t> dist(0, 255);
    registers[Vx] = dist(rng) & byte;
	pc += 2;
}

void Chip8Emulator::OP_Dxyn() {
    uint8_t x = registers[(opcode & 0x0F00u) >> 8u] % 64;
    uint8_t y = registers[(opcode & 0x00F0u) >> 4u] % 32;
    uint8_t height = opcode & 0x000Fu;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[I + row];

        for (unsigned int col = 0; col < 8; col++) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            if (spritePixel) {
                uint32_t index = ((y + row) % 32) * 64 + ((x + col) % 64);
                if (gfx[index] == 1) {
                    registers[0xF] = 1; // collision
                }
                gfx[index] ^= 1;
            }
        }
    }
	pc += 2;
}

void Chip8Emulator::OP_Ex9E()
{
	uint8_t key = registers[(opcode & 0x0F00u) >> 8u];
	pc += 2;
	if (keypad[key])
	{
		pc += 2;
	}
	

}

void Chip8Emulator::OP_ExA1()
{
	uint8_t key = registers[(opcode & 0x0F00u) >> 8u];
	pc += 2;
	if (!keypad[key])
	{
		pc += 2;
	}

	
}

void Chip8Emulator::OP_Fx07()
{
	registers[(opcode & 0x0F00) >> 8u] = delayTimer;
	pc += 2;
}

void Chip8Emulator::OP_Fx0A()
{
    if (!waitingForKey) {
        waitingForKey = true;
        keyReleased = false;
        waitingRegister = (opcode & 0x0F00u) >> 8u;
        return;
    }

    for (uint8_t i = 0; i < 16; ++i) {
        if (keypad[i]) {
            if (keyReleased) {
                registers[waitingRegister] = i;      
                waitingForKey = false;
				pc += 2;
                return;
            }
        }
    }

    // If no key is pressed, allow next one to count
    bool anyKeyDown = false;
    for (int i = 0; i < 16; ++i) {
        if (keypad[i]) {
            anyKeyDown = true;
            break;
        }
    }

    if (!anyKeyDown) {
        keyReleased = true;
    }

    // Stay on this instruction until proper keypress+release
}


void Chip8Emulator::OP_Fx15()
{
	delayTimer = registers[(opcode & 0x0F00u) >> 8u];
	pc += 2;
}

void Chip8Emulator::OP_Fx18()
{
	soundTimer = registers[(opcode & 0x0F00u) >> 8u];
	pc += 2;
}

void Chip8Emulator::OP_Fx1E()
{
	I += registers[(opcode & 0x0F00u) >> 8u];
	pc += 2;
}

void Chip8Emulator::OP_Fx29()
{
	I = FONTSET_START_ADDRESS + (5 * registers[(opcode & 0x0F00u) >> 8u]);
	pc += 2;
}

void Chip8Emulator::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[I + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[I + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[I] = value % 10;
	pc += 2;
}

void Chip8Emulator::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[I + i] = registers[i];
	}
	pc += 2;
}

void Chip8Emulator::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[I + i];
	}
	pc += 2;
}

void Chip8Emulator::onKeyDown(uint8_t chip8Key) {
    stagingKeypad[chip8Key] = 1;
}

void Chip8Emulator::onKeyUp(uint8_t chip8Key) {
    stagingKeypad[chip8Key] = 0;
}