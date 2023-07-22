#include "hc/hc.h"
#include "hc/debug.h"
#include "hc/mem.c"
#include "hc/math.c"
#include "hc/util.c"
#include "hc/compiler_rt/libc.c"
#include "hc/linux/linux.h"
#include "hc/linux/sys.c"
#include "hc/linux/debug.c"
#include "hc/linux/util.c"
#include "hc/linux/helpers/_start.c"

#include "hc/crypto/sha256.c"
#include "hc/crypto/sha512.c"
#include "hc/crypto/curve25519.c"
#include "hc/crypto/x25519.c"
#include "hc/crypto/ed25519.c"
#include "hc/crypto/chacha20.c"
#include "hc/crypto/poly1305.c"

#include "proto.h"
static int32_t pageSize;
#define client_PAGE_SIZE pageSize
#include "client.c"

static struct client client;

int32_t start(hc_UNUSED int32_t argc, hc_UNUSED char **argv, char **envp) {
    pageSize = util_getPageSize(util_getAuxv(envp));

    // TODO: Parse arguments.
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = hc_BSWAP16(2222),
        .sin_addr = { 127, 0, 0, 1 }
    };
    int32_t status = client_init(&client, addr.sin_family);
    if (status < 0) {
        debug_printNum("Failed to initialise client (", status, ")\n");
        return 1;
    }
    status = client_connect(&client, &addr, sizeof(addr));
    if (status < 0) {
        debug_printNum("Failed to connect to server (", status, ")\n");
        return 1;
    }
    client_deinit(&client);
    return 0;
}
