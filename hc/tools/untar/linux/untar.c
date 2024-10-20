#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"
#include "hc/tar.h"

#define ix_ERRNO(RET) (-RET)
#define write sys_write
#define read sys_read
#include "hc/ix/util.c"

#include "../common.c"

#define openat sys_openat
#define close sys_close
#define mkdirat sys_mkdirat
#include "../ix.c"
