Cross compile freestanding C programs with clang and lld.

### Targets:
| Binary Format | Architectures             | Library Support |
| ------------- | ------------------------- | --------------- |
| Elf           | x86_64, aarch64, riscv64  | Linux           |
| Pe            | x86_64, aarch64, riscv64* | Windows, Uefi   |
| Wasm          | wasm32                    | Wasi            |

\* Missing support in LLVM as of now.
