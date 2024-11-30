#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#define ix_ERRNO(RET) errno
#include "hc/ix/util.c"
#include "hc/debug.c"
#include "hc/freebsd/_start.c"
#include "hc/tar.h"

#include "../common.c"

#include "../ix.c"
