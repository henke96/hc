// Static assert some compiler assumptions.
_Static_assert((char)-1 == 255, "char not unsigned");
_Static_assert(sizeof(long long) == 8, "long long not 8 bytes");
_Static_assert(sizeof(int) == 4, "int not 4 bytes");
_Static_assert(sizeof(short) == 2, "short not 2 bytes");
_Static_assert(sizeof(void *) == 4 || sizeof(void *) == 8, "void * not 4 or 8 bytes");
_Static_assert(-1 == ~0, "not two's complement");
_Static_assert((-1 >> 1) == -1, "not arithmetic shift right");
_Static_assert(sizeof(u""[0]) == 2, "u string literal not 2 bytes");
_Static_assert(sizeof(enum {A}) == 4, "enum not 4 bytes");

#if defined(__x86_64__)
    #define hc_X86_64 1
#elif defined(__aarch64__)
    #define hc_AARCH64 1
#elif defined(__riscv) && __riscv_xlen == 64
    #define hc_RISCV64 1
#elif defined(__wasm32__)
    #define hc_WASM32 1
#else
    #error "Unsupported architecture"
#endif

// Are size_t, int, long and pointer types 32 bit?
#if defined(__ILP32__)
    #define hc_ILP32 1
    #define hc_ILP32_PAD(NAME) int32_t NAME;
    #define hc_LP64_PAD(NAME)
#else
    #define hc_ILP32 0
    #define hc_ILP32_PAD(NAME)
    #define hc_LP64_PAD(NAME) int32_t NAME;
#endif

// Preprocessor helpers.
#define hc_STR(X) #X
#define hc_XSTR(X) hc_STR(X)
#define hc_ARRAY_LEN(ARRAY) (sizeof(ARRAY) / sizeof((ARRAY)[0]))
#define hc_STR_LEN(STR) (hc_ARRAY_LEN(STR) - 1)
#define hc_STR_COMMA_LEN(STR) (STR), hc_STR_LEN(STR)

// Attributes
#define hc_WEAK __attribute__((weak))
#define hc_ALIAS(NAME) __attribute__((alias(NAME)))
#define hc_FALLTHROUGH __attribute__((fallthrough))
#define hc_NONULL __attribute__((nonnull))
#define hc_UNUSED __attribute__((unused))
#define hc_PACKED(N) __attribute__((packed, aligned(N)))
#define hc_FALLTHROUGH __attribute__((fallthrough))
#define hc_ALIGNED(N) __attribute__((aligned(N)))
#define hc_SECTION(NAME) __attribute__((section(NAME)))
#define hc_INLINE __attribute__((always_inline)) inline
#define hc_NO_INLINE __attribute__((noinline))
#define hc_NO_BUILTIN __attribute__((no_builtin))
#define hc_NO_DISCARD __attribute__((warn_unused_result))
#if hc_X86_64
    #define hc_MS_ABI __attribute__((ms_abi))
    #define hc_SYSV_ABI __attribute__((sysv_abi))
#else
    #define hc_MS_ABI
    #define hc_SYSV_ABI
#endif

#if defined(_WIN32)
    #define hc_EXPORT __attribute__((dllexport))
#else
    #define hc_EXPORT __attribute__((visibility("default")))
#endif
#define hc_DLLIMPORT __attribute__((dllimport))
#define hc_WASM_IMPORT(MODULE, NAME) __attribute__((import_module(MODULE), import_name(NAME)))

// Builtins
#define hc_UNREACHABLE __builtin_unreachable()
#define hc_TRAP __builtin_trap()
#define hc_ASSUME __builtin_assume
#define hc_ASSUME_ALIGNED __builtin_assume_aligned
#define hc_ABS32 __builtin_abs
#define hc_ABS64 __builtin_llabs
#define hc_BSWAP16 __builtin_bswap16
#define hc_BSWAP32 __builtin_bswap32
#define hc_BSWAP64 __builtin_bswap64
#define hc_POPCOUNT32 __builtin_popcount
#define hc_POPCOUNT64 __builtin_popcountll
#define hc_ROTL8 __builtin_rotateleft8
#define hc_ROTL16 __builtin_rotateleft16
#define hc_ROTL32 __builtin_rotateleft32
#define hc_ROTL64 __builtin_rotateleft64
#define hc_ROTR8 __builtin_rotateright8
#define hc_ROTR16 __builtin_rotateright16
#define hc_ROTR32 __builtin_rotateright32
#define hc_ROTR64 __builtin_rotateright64
#define hc_MEMCPY __builtin_memcpy
#define hc_MEMMOVE __builtin_memmove
#define hc_MEMSET __builtin_memset
#define hc_WASM_MEMORY_SIZE __builtin_wasm_memory_size(0)
#define hc_WASM_MEMORY_GROW(DELTA) __builtin_wasm_memory_grow(0, DELTA)

// Atomics
#define hc_ATOMIC_RELAXED __ATOMIC_RELAXED
#define hc_ATOMIC_ACQUIRE __ATOMIC_ACQUIRE
#define hc_ATOMIC_RELEASE __ATOMIC_RELEASE
#define hc_ATOMIC_ACQ_REL __ATOMIC_ACQ_REL
#define hc_ATOMIC_SEQ_CST __ATOMIC_SEQ_CST

#define hc_ATOMIC_EXCHANGE __atomic_exchange_n
#define hc_ATOMIC_COMPARE_EXCHANGE __atomic_compare_exchange_n
#define hc_ATOMIC_STORE __atomic_store_n
#define hc_ATOMIC_LOAD __atomic_load_n

#if hc_X86_64
    #define hc_ATOMIC_PAUSE asm volatile("pause" ::: "memory")
#elif hc_AARCH64
    #define hc_ATOMIC_PAUSE asm volatile("isb" ::: "memory")
#elif hc_RISCV64
    // This is `pause`, but assemblers don't support it as of now.
    #define hc_ATOMIC_PAUSE asm volatile(".insn i 0x0F, 0, x0, x0, 0x010" ::: "memory")
#elif hc_WASM32
    #define hc_ATOMIC_PAUSE
#endif

typedef __int128_t int128_t;
typedef __uint128_t uint128_t;

#if hc_WASM32
    #define hc_MUL128_64x64(A, B) mul128_64x64((A), (B))
#else
    #define hc_MUL128_64x64(A, B) ((uint128_t)(A) * (B))
#endif

// Standard C
#define NULL ((void *)0)

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;

#define INT8_MIN (-1 - 0x7f)
#define INT16_MIN (-1 - 0x7fff)
#define INT32_MIN (-1 - 0x7fffffff)
#define INT64_MIN (-1 - 0x7fffffffffffffff)
#define INT8_MAX (0x7f)
#define INT16_MAX (0x7fff)
#define INT32_MAX (0x7fffffff)
#define INT64_MAX (0x7fffffffffffffff)
#define UINT8_MAX (0xff)
#define UINT16_MAX (0xffff)
#define UINT32_MAX (0xffffffffU)
#define UINT64_MAX (0xffffffffffffffffU)

#if hc_ILP32
    typedef int32_t ssize_t;
    typedef uint32_t size_t;
    #define SIZE_MAX UINT32_MAX
    #define SSIZE_MAX INT32_MAX
    #define SSIZE_MIN INT32_MIN
#else
    typedef int64_t ssize_t;
    typedef uint64_t size_t;
    #define SIZE_MAX UINT64_MAX
    #define SSIZE_MAX INT64_MAX
    #define SSIZE_MIN INT64_MIN
#endif

#define bool _Bool
#define false 0
#define true 1

#define static_assert _Static_assert
#define noreturn _Noreturn
#define alignas _Alignas
#define alignof _Alignof
#define thread_local _Thread_local
#define offsetof __builtin_offsetof

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_copy __builtin_va_copy
