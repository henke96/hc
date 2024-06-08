#include "hc/hc.h"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/debug.h"
#include "hc/compilerRt/mem.c"
#include "hc/freebsd/freebsd.h"
#include "hc/freebsd/libc.so.7.h"
#include "hc/freebsd/debug.c"
#include "hc/freebsd/_start.c"
#include "hc/tar.h"

#define ix_ERRNO(RET) errno
#include "hc/ix/util.c"

#include "../common.c"

#include "../ix.c"
