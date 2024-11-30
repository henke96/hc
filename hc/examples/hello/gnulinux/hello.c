#include "hc/hc.h"
#include "hc/util.c"
#include "hc/elf.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/gnu/gnu.h"
#include "hc/linux/gnu/libc.so.6.h"
#define ix_ERRNO(RET) (*__errno_location())
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/linux/gnu/_start.c"

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, hc_UNUSED char **envp) {
    int32_t status = printf("Hello!\n");
    if (status < 0) return 1;
    return 0;
}
