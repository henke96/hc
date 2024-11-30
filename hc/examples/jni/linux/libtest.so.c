#include "hc/hc.h"
#include "hc/jni.h"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/util.c"
static noreturn void abort(void) {
    sys_kill(sys_getpid(), SIGABRT);
    sys_exit_group(137);
}
#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"
#include "hc/debug.c"

#include "../common.c"
