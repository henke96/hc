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

static int32_t run(void) {
    #define run_SERVICENAME "ssh-userauth"
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t serviceNameSize;
            char serviceName[sizeof(run_SERVICENAME) - 1];
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } serviceRequest = {
        .message = {
            .opcode = proto_MSG_SERVICE_REQUEST,
            .serviceNameSize = hc_BSWAP32(sizeof(run_SERVICENAME) - 1),
            .serviceName = run_SERVICENAME
        }
    };
    int32_t status = client_sendMessage(&client, &serviceRequest.head, sizeof(serviceRequest.message));
    if (status < 0) return -1;

    struct {
        uint8_t opcode;
        uint32_t serviceNameSize;
        char serviceName[sizeof(run_SERVICENAME) - 1];
    } hc_PACKED(1) *serviceResponse;
    int32_t responseSize = client_waitForMessage(&client, (uint8_t **)&serviceResponse);
    if (responseSize != sizeof(*serviceResponse)) return -2;

    if (
        serviceResponse->opcode != proto_MSG_SERVICE_ACCEPT ||
        serviceResponse->serviceNameSize != hc_BSWAP32(sizeof(run_SERVICENAME) - 1) ||
        hc_MEMCMP(&serviceResponse->serviceName[0], run_SERVICENAME, sizeof(run_SERVICENAME) - 1) != 0
    ) return -3;
    #undef run_SERVICENAME

    // Send userauth request.
    #define run_USERNAME "root"
    #define run_SERVICENAME "ssh-connection"
    #define run_METHODNAME "none"
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t usernameSize;
            char username[sizeof(run_USERNAME) - 1];
            uint32_t servicenameSize;
            char servicename[sizeof(run_SERVICENAME) - 1];
            uint32_t methodnameSize;
            char methodname[sizeof(run_METHODNAME) - 1];
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } userauthRequest = {
        .message = {
            .opcode = proto_MSG_USERAUTH_REQUEST,
            .usernameSize = hc_BSWAP32(sizeof(run_USERNAME) - 1),
            .username = run_USERNAME,
            .servicenameSize = hc_BSWAP32(sizeof(run_SERVICENAME) - 1),
            .servicename = run_SERVICENAME,
            .methodnameSize = hc_BSWAP32(sizeof(run_METHODNAME) - 1),
            .methodname = run_METHODNAME
        }
    };
    status = client_sendMessage(&client, &userauthRequest.head, sizeof(userauthRequest.message));
    if (status < 0) return -1;

    uint8_t *response;
    responseSize = client_waitForMessage(&client, &response);
    if (responseSize != sizeof(*response)) return -2;
    if (response[0] != proto_MSG_USERAUTH_SUCCESS) return -3;

    // Open channel.
    #define run_CHANNEL_TYPE "session"
    #define run_CHANNEL 1
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t channelTypeSize;
            char channelType[sizeof(run_CHANNEL_TYPE) - 1];
            uint32_t senderChannel;
            uint32_t initialWindowSize;
            uint32_t maximumPacketSize;
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } channelOpen = {
        .message = {
            .opcode = proto_MSG_CHANNEL_OPEN,
            .channelTypeSize = hc_BSWAP32(sizeof(run_CHANNEL_TYPE) - 1),
            .channelType = run_CHANNEL_TYPE,
            .senderChannel = hc_BSWAP32(run_CHANNEL),
            .initialWindowSize = UINT32_MAX,
            .maximumPacketSize = (uint32_t)client.bufferSize
        }
    };
    status = client_sendMessage(&client, &channelOpen.head, sizeof(channelOpen.message));
    if (status < 0) return -4;

    struct {
        uint8_t opcode;
        uint32_t recipientChannel;
        uint32_t senderChannel;
        uint32_t initialWindowSize;
        uint32_t maximumPacketSize;
        uint8_t channelTypeSpecificData[];
    } hc_PACKED(1) *openConfirmation;
    responseSize = client_waitForMessage(&client, (uint8_t **)&openConfirmation);
    if (responseSize != sizeof(*openConfirmation)) return -5;
    uint32_t serverChannelBe = openConfirmation->senderChannel;

    // Request pty.
    #define run_TERM "xterm-256color"
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t recipientChannel;
            uint32_t requestTypeSize;
            uint8_t requestType[sizeof("pty-req") - 1];
            uint8_t wantReply;
            uint32_t termSize;
            uint8_t term[sizeof(run_TERM) - 1];
            uint32_t terminalWidth;
            uint32_t terminalHeight;
            uint32_t terminalWidthPixels;
            uint32_t terminalHeightPixels;
            uint32_t terminalModesSize;
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } ptyRequest = {
        .message = {
            .opcode = proto_MSG_CHANNEL_REQUEST,
            .recipientChannel = serverChannelBe,
            .requestTypeSize = hc_BSWAP32(sizeof("pty-req") - 1),
            .requestType = "pty-req",
            .wantReply = 1,
            .termSize = hc_BSWAP32(sizeof(run_TERM) - 1),
            .term = run_TERM
        }
    };
    status = client_sendMessage(&client, &ptyRequest.head, sizeof(ptyRequest.message));
    if (status < 0) return -100;

    struct {
        uint8_t opcode;
        uint32_t recipientChannel;
    } hc_PACKED(1) *ptyResponse;
    responseSize = client_waitForMessage(&client, (uint8_t **)&ptyResponse);
    if (responseSize != sizeof(*ptyResponse)) return -101;

    if (
        ptyResponse->opcode != proto_MSG_CHANNEL_SUCCESS ||
        ptyResponse->recipientChannel != hc_BSWAP32(run_CHANNEL)
    ) return -102;

    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t recipientChannel;
            uint32_t programSize;
            char program[sizeof("shell") - 1];
            uint8_t wantReply;
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } channelRequestShell = {
        .message = {
            .opcode = proto_MSG_CHANNEL_REQUEST,
            .recipientChannel = serverChannelBe,
            .programSize = hc_BSWAP32(sizeof("shell") - 1),
            .program = "shell",
            .wantReply = 1
        }
    };
    status = client_sendMessage(&client, &channelRequestShell.head, sizeof(channelRequestShell.message));
    if (status < 0) return -7;

    struct {
        uint8_t opcode;
        uint32_t recipientChannel;
    } hc_PACKED(1) *requestShellResponse;
    responseSize = client_waitForMessage(&client, (uint8_t **)&requestShellResponse);
    if (responseSize != sizeof(*requestShellResponse)) return -8;
    if (
        requestShellResponse->opcode != proto_MSG_CHANNEL_SUCCESS ||
        requestShellResponse->recipientChannel != hc_BSWAP32(run_CHANNEL)
    ) return -9;

    #define run_DATA "ls\n"
    struct {
        struct client_messageHead head;
        struct {
            uint8_t opcode;
            uint32_t recipientChannel;
            uint32_t dataSize;
            char data[sizeof(run_DATA) - 1];
        } hc_PACKED(1) message;
        struct client_messageTail tail;
    } channelDataOut = {
        .message = {
            .opcode = proto_MSG_CHANNEL_DATA,
            .recipientChannel = serverChannelBe,
            .dataSize = hc_BSWAP32(sizeof(run_DATA) - 1),
            .data = run_DATA
        }
    };
    struct timespec remaining = { .tv_sec = 1 };
    for (;;) {
        int32_t status = sys_clock_nanosleep(CLOCK_MONOTONIC, 0, &remaining, &remaining);
        if (status == 0) break;
        debug_ASSERT(status == -EINTR);
    }
    status = client_sendMessage(&client, &channelDataOut.head, sizeof(channelDataOut.message));
    if (status < 0) return -10;

    struct {
        uint8_t opcode;
        uint32_t recipientChannel;
        uint32_t dataSize;
        uint8_t data[];
    } hc_PACKED(1) *channelDataIn;
    for (;;) {
        responseSize = client_waitForMessage(&client, (uint8_t **)&channelDataIn);
        if (responseSize < (int32_t)sizeof(*channelDataIn)) return -11;
        if (
            channelDataIn->opcode != proto_MSG_CHANNEL_DATA ||
            channelDataIn->recipientChannel != hc_BSWAP32(run_CHANNEL) ||
            (uint64_t)responseSize != sizeof(*channelDataIn) + (uint64_t)hc_BSWAP32(channelDataIn->dataSize)
        ) return -12;

        sys_write(STDOUT_FILENO, &channelDataIn->data[0], hc_BSWAP32(channelDataIn->dataSize));
    }

    debug_printNum("cool: ", response[0], "\n");
    return 0;
}

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
    status = run();
    if (status < 0) {
        debug_printNum("Error while running (", status, ")\n");
        return 1;
    }
    client_deinit(&client);
    return 0;
}
