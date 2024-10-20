// BOOTP message types.
#define dhcp_BOOTREQUEST 1
#define dhcp_BOOTREPLY 2

// HW address types.
#define dhcp_ETHERNET 1

// Options.
#define dhcp_SUBNET_MASK 1
#define dhcp_ROUTER 3
#define dhcp_DNS 6
#define dhcp_REQUESTED_IP_ADDRESS 50
#define dhcp_LEASE_TIME 51
#define dhcp_MESSAGE_TYPE 53
#define dhcp_SERVER_IDENTIFIER 54
#define dhcp_PARAM_REQUEST_LIST 55
#define dhcp_END 255

// DHCP message types.
#define dhcp_DISCOVER 1
#define dhcp_OFFER 2
#define dhcp_REQUEST 3
#define dhcp_DECLINE 4
#define dhcp_ACK 5
#define dhcp_NAK 6
#define dhcp_RELEASE 7
#define dhcp_INFORM 8

#define dhcp_MAGIC_COOKIE 0x63538263

struct dhcp_option {
    uint8_t code;
    uint8_t length;
    // uint8_t data[];
};

struct dhcp_header {
    uint8_t opcode;
    uint8_t hwAddrType;
    uint8_t hwAddrLen;
    uint8_t hops;
    uint32_t identifier;
    uint16_t seconds;
    uint16_t flags;
    uint8_t clientIp[4];
    uint8_t yourIp[4];
    uint8_t serverIp[4];
    uint8_t gatewayIp[4];
    uint8_t clientHwAddr[16];
    char serverName[64];
    char bootFileName[128];
    uint32_t magicCookie;
    // uint8_t options[];
};
