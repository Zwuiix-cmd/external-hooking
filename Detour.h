#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <windows.h>

#include "Assembly.h"

class Detour {
protected:
    HANDLE handle = nullptr;
    uintptr_t fromAddress = 0;
    uintptr_t toAddress = 0;

    size_t preserveSize = 0;
public:
    std::vector<BYTE> detourBytes = {};
    std::vector<BYTE> originalBytes = {};
public:
    Detour(HANDLE hProcess, uintptr_t from, uintptr_t to, size_t size) : handle(hProcess), fromAddress(from), toAddress(to), preserveSize(size) {make();};
    Detour(HANDLE hProcess, void* from, uintptr_t to, size_t size) : Detour(hProcess, from, reinterpret_cast<void*>(to), size) {}
    Detour(HANDLE hProcess, uintptr_t from, void* to, size_t size) : Detour(hProcess, reinterpret_cast<void*>(from), to, size) {}
    Detour(HANDLE hProcess, void* from, void* to, size_t size) : Detour(hProcess, reinterpret_cast<uintptr_t>(from), reinterpret_cast<uintptr_t>(to), size) {}

    void make() {
        readOriginalBytes(fromAddress, preserveSize);
        detourBytes = originalBytes;
    }

    void inject(std::vector<BYTE> &bytes) {
        detourBytes.insert(detourBytes.end(), bytes.begin(), bytes.end()); // inject custom bytes
    }

    void install() {
        std::vector<BYTE> jmpBack = Assembly::jmp(toAddress + detourBytes.size(), fromAddress + preserveSize); // back in rly code
        detourBytes.insert(detourBytes.end(), jmpBack.begin(), jmpBack.end());

        Memory::patchBytes(reinterpret_cast<void*>(toAddress), detourBytes.data(), detourBytes.size()); // write detour

        std::vector<BYTE> jmpToDetour = Assembly::jmp(fromAddress, toAddress);
        if (preserveSize > 5) {
            size_t nopCount = preserveSize - 5;
            jmpToDetour.insert(jmpToDetour.end(), nopCount, 0x90);
        }
        Memory::patchBytes(reinterpret_cast<void*>(fromAddress), jmpToDetour.data(), jmpToDetour.size());
    }

    void uninstall() {
        Memory::patchBytes(reinterpret_cast<void*>(fromAddress), originalBytes.data(), originalBytes.size());
    }

    std::vector<BYTE> getOriginalBytes() const {
        return originalBytes;
    }
private:
    void readOriginalBytes(uintptr_t src, size_t length) {
        originalBytes.resize(length);
        SIZE_T bytesRead;
        ReadProcessMemory(handle, reinterpret_cast<void*>(src), originalBytes.data(), length, &bytesRead);
    }
public: // Assembly func
    void mov_ebp(uintptr_t to) {
        auto bytes = Assembly::mov_ebp(static_cast<int32_t>(to - (toAddress + detourBytes.size() + 6)));
        inject(bytes);
    }

    void lea_rax_rbx() {
        std::vector<BYTE> bytes = {0x48, 0x8D, 0x03};
        inject(bytes);
    }

    void lea_rax_rdx() {
        std::vector<BYTE> bytes = {0x48, 0x8D, 0x02};
        inject(bytes);
    }

    void mov_rax(uintptr_t to) {
        auto bytes = Assembly::mov_rax(to);
        inject(bytes);
    }
};