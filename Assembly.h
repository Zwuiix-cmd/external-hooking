#include <vector>
#include <cstdint>
#include <Windows.h>

class Assembly {
public:
    static std::vector<BYTE> jmp(uintptr_t fromAddr, uintptr_t toAddr) {
        auto offset = static_cast<int32_t>(toAddr - (fromAddr + 5));
        std::vector<BYTE> jmpBytes = {
            0xE9,
            static_cast<BYTE>(offset & 0xFF),
            static_cast<BYTE>((offset >> 8) & 0xFF),
            static_cast<BYTE>((offset >> 16) & 0xFF),
            static_cast<BYTE>((offset >> 24) & 0xFF)
        };

        return jmpBytes;
    }

    static std::vector<BYTE> mov_ebp(int32_t relativeOffset) {
        std::vector<BYTE> bytes = { 0x89, 0x2D };
        for (int i = 0; i < 4; ++i) {
            bytes.push_back(static_cast<BYTE>((relativeOffset >> (8 * i)) & 0xFF));
        }

        return bytes;
    }

    static std::vector<BYTE> mov_rax(uintptr_t address) {
        std::vector<BYTE> dynamicBytes = {0x48, 0xA3};

        for (int i = 0; i < sizeof(address); ++i) {
            dynamicBytes.push_back(static_cast<BYTE>((address >> (8 * i)) & 0xFF));
        }

        return dynamicBytes;
    }
};