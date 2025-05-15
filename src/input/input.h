#ifndef INPUT_HANDLE
#define INPUT_HANDLE

#include <cstdint>

class Input {
public:
	virtual void onKeyDown(uint8_t chip8key) = 0;
	virtual void onKeyUp(uint8_t chip8Key) = 0;
	virtual ~Input() = default;
};

#endif