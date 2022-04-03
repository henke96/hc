// Static assert some compiler assumptions.
_Static_assert(sizeof(long long) == 8, "long long not 8 bytes");
_Static_assert(sizeof(int) == 4, "int not 4 bytes");
_Static_assert(sizeof(short) == 2, "short not 2 bytes");
_Static_assert(-1 == ~0, "not two's complement");
_Static_assert((-1 >> 1) == -1, "not arithmetic shift right");
_Static_assert(sizeof(L""[0]) == 2, "wide char not 2 bytes");

#if defined(__x86_64__)
#define hc_X86_64 1
#elif defined(__aarch64__)
#define hc_AARCH64 1
#elif defined(__riscv)
#define hc_RISCV64 1
#else
#error "Unsupported architecture"
#endif

#define hc_UNREACHABLE __builtin_unreachable()
#define hc_UNUSED __attribute__((unused))
#define hc_PACKED __attribute__((packed))
#define hc_FALLTHROUGH __attribute__((fallthrough))
#define hc_ALIGNED(N) __attribute__((aligned(N)))
#define hc_ALWAYS_INLINE __attribute__((always_inline)) inline
#define hc_SECTION(NAME) __attribute__((section(NAME)))
#define hc_WEAK __attribute__((weak))
#define hc_ABS(N) __builtin_abs((N))
#define hc_ABS64(N) __builtin_labs((N))
#define hc_BSWAP16(N) __builtin_bswap16((N))
#define hc_BSWAP32(N) __builtin_bswap32((N))
#define hc_BSWAP64(N) __builtin_bswap64((N))
#define hc_MEMCPY(DEST, SRC, N) __builtin_memcpy((DEST), (SRC), (N))
#define hc_MEMMOVE(DEST, SRC, N) __builtin_memmove((DEST), (SRC), (N))
#define hc_MEMCMP(LEFT, RIGHT, N) __builtin_memcmp((LEFT), (RIGHT), (N))
#define hc_MEMSET(DEST, VALUE, N) __builtin_memset((DEST), (VALUE), (N))

// Atomics
#define hc_ATOMIC_RELAXED __ATOMIC_RELAXED
#define hc_ATOMIC_ACQUIRE __ATOMIC_ACQUIRE
#define hc_ATOMIC_RELEASE __ATOMIC_RELEASE
#define hc_ATOMIC_ACQ_REL __ATOMIC_ACQ_REL
#define hc_ATOMIC_SEQ_CST __ATOMIC_SEQ_CST

#define hc_ATOMIC_EXCHANGE(PTR, VAL, MEMORDER) __atomic_exchange_n((PTR), (VAL), (MEMORDER))
#define hc_ATOMIC_COMPARE_EXCHANGE(PTR, EXPECTED, DESIRED, SUCCESS_MEMORDER, FAILURE_MEMORDER) __atomic_compare_exchange_n((PTR), (EXPECTED), (DESIRED), 0, (SUCCESS_MEMORDER), (FAILURE_MEMORDER))
#define hc_ATOMIC_STORE(PTR, VAL, MEMORDER) __atomic_store_n((PTR), (VAL), (MEMORDER))
#define hc_ATOMIC_LOAD(PTR, MEMORDER) __atomic_load_n((PTR), (MEMORDER))

#if hc_X86_64
#define hc_ATOMIC_PAUSE asm volatile("pause" ::: "memory")
#elif hc_AARCH64
#define hc_ATOMIC_PAUSE asm volatile("yield" ::: "memory")
#elif hc_RISCV64
// This is `pause`, but assemblers don't support it as of now.
#define hc_ATOMIC_PAUSE asm volatile(".insn i 0x0F, 0, x0, x0, 0x010" ::: "memory")
#endif

// ALIGN must be power of 2.
#define hc_ALIGN_FORWARD(X, ALIGN) (((X) + ((ALIGN) - 1)) & ~((ALIGN) - 1))

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

#define bool _Bool
#define false 0
#define true 1

#define static_assert _Static_assert
#define noreturn _Noreturn
#define alignas _Alignas
#define alignof _Alignof
#define thread_local _Thread_local

// These need to exist even when compiling for freestanding.
void *memset(void *dest, int32_t c, uint64_t n);
void *memmove(void *dest, const void *src, uint64_t n);
void *memcpy(void *restrict dest, const void *restrict src, uint64_t n);
int32_t memcmp(const void *left, const void *right, uint64_t n);
