#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/util.c"
#include "hc/math.c"
#include "hc/mem.c"
#include "hc/compilerRt/mem.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/util.c"
#include "hc/linux/debug.c"
#include "hc/linux/helpers/_start.c"

#include "hc/crypto/sha512.c"
#include "hc/crypto/sha256.c"
#include "hc/crypto/sha1.c"

#include "../common.c"

static int32_t fileFd;

static int32_t init(char *file) {
    fileFd = sys_openat(AT_FDCWD, file, O_RDONLY, 0);
    return fileFd;
}

static void deinit(void) {
    debug_CHECK(sys_close(fileFd), RES == 0);
}

static int32_t readIntoBuffer(void) {
    return (int32_t)sys_read(fileFd, &buffer[0], sizeof(buffer));
}

static int32_t printBuffer(int32_t size) {
    return util_writeAll(STDOUT_FILENO, &buffer[0], size);
}
