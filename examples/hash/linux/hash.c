#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"

#define write sys_write
#define read sys_read
#define ix_ERRNO(RET) (-RET)
#include "hc/ix/util.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"

#include "../common.c"
#define openat sys_openat
#define close sys_close
#include "../ix.c"
