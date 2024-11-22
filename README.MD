# Memory Hooking Project

## Overview

This project provides a memory hooking mechanism for 64-bit processes. It allows you to inject custom code (e.g., a detour function) into a target process’s memory, using direct memory manipulation and assembly instructions. The key functionality is the ability to safely replace target instructions with a jump (`JMP`) without corrupting adjacent instructions in the process's code.

## Features

- **Direct Memory Access:** Ability to read and write memory in a target process using Windows API (`ReadProcessMemory`, `WriteProcessMemory`).
- **Detour Installation:** Replace target instructions with a jump (JMP) to custom functions.
- **Customizable Preservation:** Users manually specify how many bytes should be preserved to avoid overwriting important instructions.
- **Memory Restoration:** Provides a mechanism to restore the original code in the process's memory after the hook is removed.

## Installation

1. Clone the repository and navigate to the project directory.

```bash
git clone https://github.com/Zwuiix-cmd/external-hooking.git
cd external-hooking
```

2. Open the project in your preferred C++ IDE and compile the project using a C++17-compatible compiler (MSVC is recommended on Windows).

## Dependencies

- Windows OS (for memory manipulation using Windows API).
- C++17 or later (for language features).
- Target 64-bit process to inject hooks into.

## How It Works

### Detour Installation

The detour system allows you to inject custom code by replacing an instruction at a specific address in the target process with a jump (JMP) to your custom code. This is accomplished by:
- Reading the original instruction from the target process.
- Replacing the instruction with a JMP instruction that redirects execution to your custom function.
- Preserving the number of bytes required to ensure no other instructions are overwritten.

### Custom Preservation

In this updated version, the user must manually input the number of bytes to preserve when creating a `Detour` object. This allows more control over the detour process but requires that the user ensures they are not overwriting important code. The preservation size is crucial because:
- It ensures that we don’t overwrite the next instruction(s) in the target process.
- The size must be at least 5 bytes to account for the standard `JMP` instruction.

### Restoring the Original Code

When the hook is no longer needed, the original bytes are restored in the target process’s memory. This ensures that the target process continues running as it originally did.

## Usage

### Creating and Installing a Detour

```cpp
#include "Detour.h"

uintptr_t targetAddress = 0x1234567890ABCDEF;  // Address of the instruction to hook
uintptr_t detourFunction = 0x12345678ABCDEF01; // Address of the custom detour function
size_t preserveSize = 7;  // Specify how many bytes to preserve (e.g., 7 in this case)

Detour detour(targetAddress, detourFunction, preserveSize);

// Make the detour and inject custom bytes
detour.make();

// Install the hook (replace original instruction with a jump)
detour.install();

// To uninstall the hook and restore the original code
detour.uninstall();
```

### Explanation

- `Detour` constructor: Takes the target address, the address of the custom detour function, and the number of bytes to preserve.
- `make()`: Reads the original bytes from the target address and prepares the detour bytes.
- `install()`: Replaces the original code with a jump to the detour function, ensuring the preserved bytes are not overwritten.
- `uninstall()`: Restores the original bytes to the target address.

### Error Handling

Exceptions may be thrown for:
- Failure to read the target memory with `ReadProcessMemory`.
- Invalid memory addresses.

Make sure to handle these exceptions appropriately.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository.
2. Create your feature branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -am 'Add new feature'`).
4. Push to the branch (`git push origin feature-name`).
5. Open a pull request.

## Acknowledgements

- The idea behind this project is inspired by techniques used in game hacking and debugging tools such as Cheat Engine and other memory manipulation software.
- The memory manipulation logic is based on the Windows API for direct memory reading and writing.