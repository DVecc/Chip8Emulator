#include "../src/emulator/chip8Emulator.h"
#include <filesystem>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
void platform_log(const wchar_t*, TextColor) {}
static void require(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
int main() {
  const auto path = std::filesystem::temp_directory_path() / "chip8-local-core-test.bin";
  try {
    auto run = [&](std::vector<uint8_t> bytes) {
      { std::ofstream f(path, std::ios::binary); f.write(reinterpret_cast<const char*>(bytes.data()), bytes.size()); }
      auto emu = std::make_unique<Chip8Emulator>(); emu->Chip8Init(); emu->ShutdownSound(); emu->LoadROM(path.string().c_str()); return emu;
    };
    // Draw zero twice, test VF, and draw it again only if collision was reported.
    auto draw = run({0x60,0,0x61,0,0xA0,0x50,0xD0,0x15,0xD0,0x15,0x3F,1,0x12,0x10,0xD0,0x15,0x12,0x10});
    for(int i=0;i<7;++i) draw->cycle();
    require(std::count(std::begin(draw->gfx),std::end(draw->gfx),1)==14,"font/XOR/collision failed");
    auto timer = run({0x60,2,0xF0,0x15,0xF0,7,0x30,2,0x12,0x12,0xA0,0x50,0x61,0,0xD0,0x15,0x12,4,0,0xE0,0x12,0x14});
    for(int i=0;i<8;++i) timer->cycle();
    require(timer->gfx[2]==1,"CPU cycles incorrectly decremented timer");
    timer->tickTimers();
    for(int i=0;i<5;++i) timer->cycle();
    require(std::count(std::begin(timer->gfx),std::end(timer->gfx),1)==0,"timer tick did not decrement delay register");
    auto key = run({0xF0,0x0A,0xF0,0x29,0x61,0,0x62,0,0xD1,0x25,0x12,0x0A});
    key->cycle(); key->cycle(); key->onKeyDown(0xA);
    for(int i=0;i<5;++i) key->cycle();
    key->onKeyUp(0xA);
    require(key->gfx[2*64+1]==1,"key wait/font lookup failed");
    bool rejected=false;
    try { auto oversized=run(std::vector<uint8_t>(3585)); } catch(const std::exception&) {rejected=true;}
    require(rejected,"oversized ROM accepted");
    std::filesystem::remove(path);
    std::cout << "PASS: font drawing, XOR/collision, separate timer tick, key wait, ROM size guard\n";
    return 0;
  } catch(const std::exception& e) { std::filesystem::remove(path); std::cerr<<e.what()<<'\n'; return 1; }
}

