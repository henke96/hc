## HC
Freestanding C with LLVM.

### Targets
| Binary Format | Architectures            | OS Support              |
| ------------- | ------------------------ | ----------------------- |
| Elf           | x86_64, aarch64, riscv64 | Linux, Android, FreeBSD |
| Pe            | x86_64, aarch64          | Windows, Uefi           |
| Wasm          | wasm32                   | Wasi                    |

### Build Examples and Run Tests
**Linux, FreeBSD:**
`OUT=../output tools/test.sh`\
**Windows:**
`set "OUT=..\output" & call tools\test.bat`
