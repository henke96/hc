static_assert(!hc_PTR32, "Pointers not 64 bit");

// errno.h
#define EPERM 1 // Operation not permitted
#define ENOENT 2 // No such file or directory
#define ESRCH 3 // No such process
#define EINTR 4 // Interrupted system call
#define EIO 5 // I/O error
#define ENXIO 6 // No such device or address
#define E2BIG 7 // Argument list too long
#define ENOEXEC 8 // Exec format error
#define EBADF 9 // Bad file number
#define ECHILD 10 // No child processes
#define EAGAIN 11 // Try again
#define ENOMEM 12 // Out of memory
#define EACCES 13 // Permission denied
#define EFAULT 14 // Bad address
#define ENOTBLK 15 // Block device required
#define EBUSY 16 // Device or resource busy
#define EEXIST 17 // File exists
#define EXDEV 18 // Cross-device link
#define ENODEV 19 // No such device
#define ENOTDIR 20 // Not a directory
#define EISDIR 21 // Is a directory
#define EINVAL 22 // Invalid argument
#define ENFILE 23 // File table overflow
#define EMFILE 24 // Too many open files
#define ENOTTY 25 // Not a typewriter
#define ETXTBSY 26 // Text file busy
#define EFBIG 27 // File too large
#define ENOSPC 28 // No space left on device
#define ESPIPE 29 // Illegal seek
#define EROFS 30 // Read-only file system
#define EMLINK 31 // Too many links
#define EPIPE 32 // Broken pipe
#define EDOM 33 // Math argument out of domain of func
#define ERANGE 34 // Math result not representable
#define EDEADLK 35 // Resource deadlock would occur
#define ENAMETOOLONG 36 // File name too long
#define ENOLCK 37 // No record locks available
#define ENOSYS 38 // Invalid system call number
#define ENOTEMPTY 39 // Directory not empty
#define ELOOP 40 // Too many symbolic links encountered
#define EWOULDBLOCK 11 // Operation would block
#define ENOMSG 42 // No message of desired type
#define EIDRM 43 // Identifier removed
#define ECHRNG 44 // Channel number out of range
#define EL2NSYNC 45 // Level 2 not synchronized
#define EL3HLT 46 // Level 3 halted
#define EL3RST 47 // Level 3 reset
#define ELNRNG 48 // Link number out of range
#define EUNATCH 49 // Protocol driver not attached
#define ENOCSI 50 // No CSI structure available
#define EL2HLT 51 // Level 2 halted
#define EBADE 52 // Invalid exchange
#define EBADR 53 // Invalid request descriptor
#define EXFULL 54 // Exchange full
#define ENOANO 55 // No anode
#define EBADRQC 56 // Invalid request code
#define EBADSLT 57 // Invalid slot
#define EDEADLOCK 35
#define EBFONT 59 // Bad font file format
#define ENOSTR 60 // Device not a stream
#define ENODATA 61 // No data available
#define ETIME 62 // Timer expired
#define ENOSR 63 // Out of streams resources
#define ENONET 64 // Machine is not on the network
#define ENOPKG 65 // Package not installed
#define EREMOTE 66 // Object is remote
#define ENOLINK 67 // Link has been severed
#define EADV 68 // Advertise error
#define ESRMNT 69 // Srmount error
#define ECOMM 70 // Communication error on send
#define EPROTO 71 // Protocol error
#define EMULTIHOP 72 // Multihop attempted
#define EDOTDOT 73 // RFS specific error
#define EBADMSG 74 // Not a data message
#define EOVERFLOW 75 // Value too large for defined data type
#define ENOTUNIQ 76 // Name not unique on network
#define EBADFD 77 // File descriptor in bad state
#define EREMCHG 78 // Remote address changed
#define ELIBACC 79 // Can not access a needed shared library
#define ELIBBAD 80 // Accessing a corrupted shared library
#define ELIBSCN 81 // .lib section in a.out corrupted
#define ELIBMAX 82 // Attempting to link in too many shared libraries
#define ELIBEXEC 83 // Cannot exec a shared library directly
#define EILSEQ 84 // Illegal byte sequence
#define ERESTART 85 // Interrupted system call should be restarted
#define ESTRPIPE 86 // Streams pipe error
#define EUSERS 87 // Too many users
#define ENOTSOCK 88 // Socket operation on non-socket
#define EDESTADDRREQ 89 // Destination address required
#define EMSGSIZE 90 // Message too long
#define EPROTOTYPE 91 // Protocol wrong type for socket
#define ENOPROTOOPT 92 // Protocol not available
#define EPROTONOSUPPORT 93 // Protocol not supported
#define ESOCKTNOSUPPORT 94 // Socket type not supported
#define EOPNOTSUPP 95 // Operation not supported on transport endpoint
#define EPFNOSUPPORT 96 // Protocol family not supported
#define EAFNOSUPPORT 97 // Address family not supported by protocol
#define EADDRINUSE 98 // Address already in use
#define EADDRNOTAVAIL 99 // Cannot assign requested address
#define ENETDOWN 100 // Network is down
#define ENETUNREACH 101 // Network is unreachable
#define ENETRESET 102 // Network dropped connection because of reset
#define ECONNABORTED 103 // Software caused connection abort
#define ECONNRESET 104 // Connection reset by peer
#define ENOBUFS 105 // No buffer space available
#define EISCONN 106 // Transport endpoint is already connected
#define ENOTCONN 107 // Transport endpoint is not connected
#define ESHUTDOWN 108 // Cannot send after transport endpoint shutdown
#define ETOOMANYREFS 109 // Too many references: cannot splice
#define ETIMEDOUT 110 // Connection timed out
#define ECONNREFUSED 111 // Connection refused
#define EHOSTDOWN 112 // Host is down
#define EHOSTUNREACH 113 // No route to host
#define EALREADY 114 // Operation already in progress
#define EINPROGRESS 115 // Operation now in progress
#define ESTALE 116 // Stale file handle
#define EUCLEAN 117 // Structure needs cleaning
#define ENOTNAM 118 // Not a XENIX named type file
#define ENAVAIL 119 // No XENIX semaphores available
#define EISNAM 120 // Is a named type file
#define EREMOTEIO 121 // Remote I/O error
#define EDQUOT 122 // Quota exceeded
#define ENOMEDIUM 123 // No medium found
#define EMEDIUMTYPE 124 // Wrong medium type
#define ECANCELED 125 // Operation Canceled
#define ENOKEY 126 // Required key not available
#define EKEYEXPIRED 127 // Key has expired
#define EKEYREVOKED 128 // Key has been revoked
#define EKEYREJECTED 129 // Key was rejected by service
#define EOWNERDEAD 130 // Owner died
#define ENOTRECOVERABLE 131 // State not recoverable
#define ERFKILL 132 // Operation not possible due to RF-kill
#define EHWPOISON 133 // Memory page has hardware error

// limits.h
#define NAME_MAX 255 /* # chars in a file name */
#define PATH_MAX 4096 /* # chars in a path name including nul */

// net.h
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_SEQPACKET 5
#define SOCK_DCCP 6
#define SOCK_PACKET 10

#define SOCK_CLOEXEC 02000000
#define SOCK_NONBLOCK 00004000

// socket.h
#define AF_UNSPEC 0
#define AF_UNIX 1 // Unix domain sockets
#define AF_INET 2 // Internet IP Protocol
#define AF_BRIDGE 7 // Multiprotocol bridge
#define AF_INET6 10 // IP version 6
#define AF_NETLINK 16
#define AF_PACKET 17 // Packet family
#define AF_ALG 38 // Algorithm sockets

#define SOL_IP 0
#define SOL_SOCKET 1

#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_TYPE 3
#define SO_ERROR 4
#define SO_DONTROUTE 5
#define SO_BROADCAST 6
#define SO_SNDBUF 7
#define SO_RCVBUF 8
#define SO_SNDBUFFORCE 32
#define SO_RCVBUFFORCE 33
#define SO_KEEPALIVE 9
#define SO_OOBINLINE 10
#define SO_NO_CHECK 11
#define SO_PRIORITY 12
#define SO_LINGER 13
#define SO_BSDCOMPAT 14
#define SO_REUSEPORT 15
#define SO_PASSCRED 16
#define SO_PEERCRED 17
#define SO_RCVLOWAT 18
#define SO_SNDLOWAT 19

#define SO_BINDTODEVICE 25

/* Socket filtering */
#define SO_ATTACH_FILTER 26
#define SO_DETACH_FILTER 27
#define SO_GET_FILTER SO_ATTACH_FILTER
#define SO_PEERNAME 28
#define SO_ACCEPTCONN 30
#define SO_PEERSEC 31
#define SO_PASSSEC 34
#define SO_MARK 36
#define SO_PROTOCOL 38
#define SO_DOMAIN 39
#define SO_RXQ_OVFL             40
#define SO_WIFI_STATUS 41
#define SO_PEEK_OFF 42

/* Instruct lower device to use last 4-bytes of skb data as FCS */
#define SO_NOFCS 43

#define SO_LOCK_FILTER 44
#define SO_SELECT_ERR_QUEUE 45
#define SO_BUSY_POLL 46
#define SO_MAX_PACING_RATE 47
#define SO_BPF_EXTENSIONS 48
#define SO_INCOMING_CPU 49
#define SO_ATTACH_BPF 50
#define SO_DETACH_BPF SO_DETACH_FILTER
#define SO_ATTACH_REUSEPORT_CBPF 51
#define SO_ATTACH_REUSEPORT_EBPF 52
#define SO_CNX_ADVICE 53
#define SCM_TIMESTAMPING_OPT_STATS 54
#define SO_MEMINFO 55
#define SO_INCOMING_NAPI_ID 56
#define SO_COOKIE 57
#define SCM_TIMESTAMPING_PKTINFO 58
#define SO_PEERGROUPS 59
#define SO_ZEROCOPY 60
#define SO_TXTIME 61
#define SCM_TXTIME SO_TXTIME

#define SO_BINDTOIFINDEX 62

#define SO_RCVTIMEO_NEW         66
#define SO_SNDTIMEO_NEW         67

#define SO_DETACH_REUSEPORT_BPF 68

#define SO_PREFER_BUSY_POLL 69
#define SO_BUSY_POLL_BUDGET 70

#define SO_NETNS_COOKIE 71
#define SO_BUF_LOCK 72
#define SO_RESERVE_MEM 73
#define SO_TXREHASH 74

#define SO_TIMESTAMP 29
#define SO_TIMESTAMPNS 35
#define SO_TIMESTAMPING 37

#define SO_RCVTIMEO 20
#define SO_SNDTIMEO 21

#define MSG_OOB 1
#define MSG_PEEK 2
#define MSG_DONTROUTE 4
#define MSG_CTRUNC 8
#define MSG_PROBE 0x10 // Do not send. Only probe path f.e. for MTU
#define MSG_TRUNC 0x20
#define MSG_DONTWAIT 0x40 // Nonblocking io
#define MSG_EOR 0x80 // End of record
#define MSG_WAITALL 0x100 // Wait for a full request
#define MSG_FIN 0x200
#define MSG_SYN 0x400
#define MSG_CONFIRM 0x800 // Confirm path validity
#define MSG_RST 0x1000
#define MSG_ERRQUEUE 0x2000 // Fetch message from error queue
#define MSG_NOSIGNAL 0x4000 // Do not generate SIGPIPE
#define MSG_MORE 0x8000 // Sender will send more
#define MSG_WAITFORONE 0x10000 // recvmmsg(): block until 1+ packets avail
#define MSG_BATCH 0x40000 // sendmmsg(): more messages coming
#define MSG_SENDPAGE_DECRYPTED 0x100000
#define MSG_ZEROCOPY 0x4000000 // Use user data in kernel path
#define MSG_FASTOPEN 0x20000000 // Send data in TCP SYN

struct iovec {
    void *iov_base;
    int64_t iov_len;
};

struct iovec_const {
    const void *iov_base;
    int64_t iov_len;
};

struct msghdr {
    void *msg_name;
    int32_t msg_namelen;
    int32_t __pad1;
    struct iovec *msg_iov;
    int64_t msg_iovlen;
    void *msg_control;
    int64_t msg_controllen;
    uint32_t msg_flags;
    int32_t __pad2;
};

struct msghdr_const {
    void *msg_name;
    int32_t msg_namelen;
    int32_t __pad1;
    struct iovec_const *msg_iov;
    int64_t msg_iovlen;
    void *msg_control;
    int64_t msg_controllen;
    uint32_t msg_flags;
    int32_t __pad2;
};

struct cmsghdr {
    uint64_t cmsg_len; /* data byte count, including hdr */
    int32_t cmsg_level; /* originating protocol */
    int32_t cmsg_type; /* protocol-specific type */
};

// net_tstamp.h
/* SO_TIMESTAMPING flags */
enum {
    SOF_TIMESTAMPING_TX_HARDWARE = (1<<0),
    SOF_TIMESTAMPING_TX_SOFTWARE = (1<<1),
    SOF_TIMESTAMPING_RX_HARDWARE = (1<<2),
    SOF_TIMESTAMPING_RX_SOFTWARE = (1<<3),
    SOF_TIMESTAMPING_SOFTWARE = (1<<4),
    SOF_TIMESTAMPING_SYS_HARDWARE = (1<<5),
    SOF_TIMESTAMPING_RAW_HARDWARE = (1<<6),
    SOF_TIMESTAMPING_OPT_ID = (1<<7),
    SOF_TIMESTAMPING_TX_SCHED = (1<<8),
    SOF_TIMESTAMPING_TX_ACK = (1<<9),
    SOF_TIMESTAMPING_OPT_CMSG = (1<<10),
    SOF_TIMESTAMPING_OPT_TSONLY = (1<<11),
    SOF_TIMESTAMPING_OPT_STATS = (1<<12),
    SOF_TIMESTAMPING_OPT_PKTINFO = (1<<13),
    SOF_TIMESTAMPING_OPT_TX_SWHW = (1<<14),
    SOF_TIMESTAMPING_BIND_PHC = (1<<15),

    SOF_TIMESTAMPING_LAST = SOF_TIMESTAMPING_BIND_PHC,
    SOF_TIMESTAMPING_MASK = (SOF_TIMESTAMPING_LAST - 1) | SOF_TIMESTAMPING_LAST
};

/*
 * SO_TIMESTAMPING flags are either for recording a packet timestamp or for
 * reporting the timestamp to user space.
 * Recording flags can be set both via socket options and control messages.
 */
#define SOF_TIMESTAMPING_TX_RECORD_MASK (SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_TX_SCHED | SOF_TIMESTAMPING_TX_ACK)

/**
 * struct so_timestamping - SO_TIMESTAMPING parameter
 *
 * flags: SO_TIMESTAMPING flags
 * bind_phc: Index of PTP virtual clock bound to sock. This is available if flag SOF_TIMESTAMPING_BIND_PHC is set.
 */
struct so_timestamping {
    uint32_t flags;
    uint32_t bind_phc;
};

/**
 * struct hwtstamp_config - %SIOCGHWTSTAMP and %SIOCSHWTSTAMP parameter
 *
 * flags: one of HWTSTAMP_FLAG_*
 * tx_type: one of HWTSTAMP_TX_*
 * rx_filter: one of HWTSTAMP_FILTER_*
 *
 * %SIOCGHWTSTAMP and %SIOCSHWTSTAMP expect a &struct ifreq with a
 * ifr_data pointer to this structure. For %SIOCSHWTSTAMP, if the
 * driver or hardware does not support the requested rx_filter value,
 * the driver may use a more general filter mode. In this case
 * rx_filter will indicate the actual mode on return.
 */
struct hwtstamp_config {
    uint32_t flags;
    uint32_t tx_type;
    uint32_t rx_filter;
};

/* possible values for hwtstamp_config->flags */
enum hwtstamp_flags {
    /*
     * With this flag, the user could get bond active interface's
     * PHC index. Note this PHC index is not stable as when there
     * is a failover, the bond active interface will be changed, so
     * will be the PHC index.
     */
    HWTSTAMP_FLAG_BONDED_PHC_INDEX = (1<<0),
#define HWTSTAMP_FLAG_BONDED_PHC_INDEX HWTSTAMP_FLAG_BONDED_PHC_INDEX

    HWTSTAMP_FLAG_LAST = HWTSTAMP_FLAG_BONDED_PHC_INDEX,
    HWTSTAMP_FLAG_MASK = (HWTSTAMP_FLAG_LAST - 1) | HWTSTAMP_FLAG_LAST
};

/* possible values for hwtstamp_config->tx_type */
enum hwtstamp_tx_types {
    /*
     * No outgoing packet will need hardware time stamping;
     * should a packet arrive which asks for it, no hardware
     * time stamping will be done.
     */
    HWTSTAMP_TX_OFF,

    /*
     * Enables hardware time stamping for outgoing packets;
     * the sender of the packet decides which are to be
     * time stamped by setting %SOF_TIMESTAMPING_TX_SOFTWARE
     * before sending the packet.
     */
    HWTSTAMP_TX_ON,

    /*
     * Enables time stamping for outgoing packets just as
     * HWTSTAMP_TX_ON does, but also enables time stamp insertion
     * directly into Sync packets. In this case, transmitted Sync
     * packets will not received a time stamp via the socket error
     * queue.
     */
    HWTSTAMP_TX_ONESTEP_SYNC,

    /*
     * Same as HWTSTAMP_TX_ONESTEP_SYNC, but also enables time
     * stamp insertion directly into PDelay_Resp packets. In this
     * case, neither transmitted Sync nor PDelay_Resp packets will
     * receive a time stamp via the socket error queue.
     */
    HWTSTAMP_TX_ONESTEP_P2P,

    /* add new constants above here */
    __HWTSTAMP_TX_CNT
};

/* possible values for hwtstamp_config->rx_filter */
enum hwtstamp_rx_filters {
    /* time stamp no incoming packet at all */
    HWTSTAMP_FILTER_NONE,

    /* time stamp any incoming packet */
    HWTSTAMP_FILTER_ALL,

    /* return value: time stamp all packets requested plus some others */
    HWTSTAMP_FILTER_SOME,

    /* PTP v1, UDP, any kind of event packet */
    HWTSTAMP_FILTER_PTP_V1_L4_EVENT,
    /* PTP v1, UDP, Sync packet */
    HWTSTAMP_FILTER_PTP_V1_L4_SYNC,
    /* PTP v1, UDP, Delay_req packet */
    HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ,
    /* PTP v2, UDP, any kind of event packet */
    HWTSTAMP_FILTER_PTP_V2_L4_EVENT,
    /* PTP v2, UDP, Sync packet */
    HWTSTAMP_FILTER_PTP_V2_L4_SYNC,
    /* PTP v2, UDP, Delay_req packet */
    HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ,

    /* 802.AS1, Ethernet, any kind of event packet */
    HWTSTAMP_FILTER_PTP_V2_L2_EVENT,
    /* 802.AS1, Ethernet, Sync packet */
    HWTSTAMP_FILTER_PTP_V2_L2_SYNC,
    /* 802.AS1, Ethernet, Delay_req packet */
    HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ,

    /* PTP v2/802.AS1, any layer, any kind of event packet */
    HWTSTAMP_FILTER_PTP_V2_EVENT,
    /* PTP v2/802.AS1, any layer, Sync packet */
    HWTSTAMP_FILTER_PTP_V2_SYNC,
    /* PTP v2/802.AS1, any layer, Delay_req packet */
    HWTSTAMP_FILTER_PTP_V2_DELAY_REQ,

    /* NTP, UDP, all versions and packet modes */
    HWTSTAMP_FILTER_NTP_ALL,

    /* add new constants above here */
    __HWTSTAMP_FILTER_CNT
};

/* SCM_TIMESTAMPING_PKTINFO control message */
struct scm_ts_pktinfo {
    uint32_t if_index;
    uint32_t pkt_length;
    uint32_t reserved[2];
};

/*
 * SO_TXTIME gets a struct sock_txtime with flags being an integer bit
 * field comprised of these values.
 */
enum txtime_flags {
    SOF_TXTIME_DEADLINE_MODE = (1 << 0),
    SOF_TXTIME_REPORT_ERRORS = (1 << 1),

    SOF_TXTIME_FLAGS_LAST = SOF_TXTIME_REPORT_ERRORS,
    SOF_TXTIME_FLAGS_MASK = (SOF_TXTIME_FLAGS_LAST - 1) | SOF_TXTIME_FLAGS_LAST
};

struct sock_txtime {
    uint32_t clockid; /* reference clockid */
    uint32_t flags; /* as defined by enum txtime_flags */
};

// in.h
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define IPPROTO_IPV6 41
#define IPPROTO_RAW 255

#define IP_TOS 1
#define IP_TTL 2
#define IP_HDRINCL 3
#define IP_OPTIONS 4
#define IP_ROUTER_ALERT 5
#define IP_RECVOPTS 6
#define IP_RETOPTS 7
#define IP_PKTINFO 8
#define IP_PKTOPTIONS 9
#define IP_MTU_DISCOVER 10
#define IP_RECVERR 11
#define IP_RECVTTL 12
#define IP_RECVTOS 13
#define IP_MTU 14
#define IP_FREEBIND 15
#define IP_IPSEC_POLICY 16
#define IP_XFRM_POLICY 17
#define IP_PASSSEC 18
#define IP_TRANSPARENT 19

/* BSD compatibility */
#define IP_RECVRETOPTS IP_RETOPTS

#define IP_ORIGDSTADDR       20
#define IP_RECVORIGDSTADDR   IP_ORIGDSTADDR

#define IP_MINTTL       21
#define IP_NODEFRAG     22
#define IP_CHECKSUM 23
#define IP_BIND_ADDRESS_NO_PORT 24
#define IP_RECVFRAGSIZE 25
#define IP_RECVERR_RFC4884 26

/* IP_MTU_DISCOVER values */
#define IP_PMTUDISC_DONT 0 /* Never send DF frames */
#define IP_PMTUDISC_WANT 1 /* Use per route hints */
#define IP_PMTUDISC_DO 2 /* Always DF */
#define IP_PMTUDISC_PROBE 3 /* Ignore dst pmtu */
#define IP_PMTUDISC_INTERFACE 4
#define IP_PMTUDISC_OMIT 5

#define IP_MULTICAST_IF 32
#define IP_MULTICAST_TTL 33
#define IP_MULTICAST_LOOP 34
#define IP_ADD_MEMBERSHIP 35
#define IP_DROP_MEMBERSHIP 36
#define IP_UNBLOCK_SOURCE 37
#define IP_BLOCK_SOURCE 38
#define IP_ADD_SOURCE_MEMBERSHIP 39
#define IP_DROP_SOURCE_MEMBERSHIP 40
#define IP_MSFILTER 41
#define MCAST_JOIN_GROUP 42
#define MCAST_BLOCK_SOURCE 43
#define MCAST_UNBLOCK_SOURCE 44
#define MCAST_LEAVE_GROUP 45
#define MCAST_JOIN_SOURCE_GROUP 46
#define MCAST_LEAVE_SOURCE_GROUP 47
#define MCAST_MSFILTER 48
#define IP_MULTICAST_ALL 49
#define IP_UNICAST_IF 50

#define MCAST_EXCLUDE 0
#define MCAST_INCLUDE 1

struct ip_mreqn {
    uint8_t imr_multiaddr[4];
    uint8_t imr_address[4];
    int32_t imr_ifindex;
};

struct sockaddr_in {
    uint16_t sin_family;
    uint16_t sin_port;
    uint8_t sin_addr[4];
    uint8_t sin_zero[8];
};

struct in_pktinfo {
    int32_t ipi_ifindex;
    uint8_t ipi_spec_dst[4];
    uint8_t ipi_addr[4];
};

// un.h
#define UNIX_PATH_MAX 108

struct sockaddr_un {
    uint16_t sun_family;
    char sun_path[UNIX_PATH_MAX];
};

// tcp.h
#define TCP_NODELAY 1

// fcntl.h
#define O_ACCMODE 00000003
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#define O_CREAT 00000100
#define O_EXCL 00000200
#define O_NOCTTY 00000400
#define O_TRUNC 00001000
#define O_APPEND 00002000
#define O_NONBLOCK 00004000
#define O_DSYNC 00010000
#define FASYNC 00020000
#define O_NOATIME 01000000
#define O_CLOEXEC 02000000
#if hc_AARCH64
#define O_DIRECTORY 040000 // must be a directory
#define O_NOFOLLOW 0100000 // don't follow links
#define O_DIRECT 0200000 // direct disk access hint - currently ignored
#define O_LARGEFILE 0400000
#else
#define O_DIRECTORY 00200000
#define O_NOFOLLOW 00400000
#define O_DIRECT 00040000
#define O_LARGEFILE 00100000
#endif

#define AT_FDCWD -100 // Special value used to indicate openat should use the current working directory.
#define AT_SYMLINK_NOFOLLOW 0x100 /* Do not follow symbolic links. */
#define AT_EACCESS 0x200 /* Test access permitted for effective IDs, not real IDs. */
#define AT_REMOVEDIR 0x200 /* Remove directory instead of unlinking file. */
#define AT_SYMLINK_FOLLOW 0x400 /* Follow symbolic links. */
#define AT_NO_AUTOMOUNT 0x800 /* Suppress terminal automount traversal */
#define AT_EMPTY_PATH 0x1000 /* Allow empty relative pathname */
#define AT_STATX_SYNC_TYPE 0x6000 /* Type of synchronisation required from statx() */
#define AT_STATX_FORCE_SYNC 0x2000 /* - Force the attributes to be sync'd with the server */
#define AT_STATX_DONT_SYNC 0x4000 /* - Don't sync attributes with the server */
#define AT_RECURSIVE 0x8000 /* Apply to the entire subtree */

// time.h
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7
#define CLOCK_REALTIME_ALARM 8
#define CLOCK_BOOTTIME_ALARM 9

struct timespec {
    int64_t tv_sec;
    int64_t tv_nsec;
};

struct timeval {
    int64_t tv_sec;
    int64_t tv_usec;
};

struct itimerspec {
    struct timespec it_interval; // Timer period
    struct timespec it_value; // Timer expiration
};

// timerfd.h
#define TFD_TIMER_ABSTIME (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#define TFD_CLOEXEC 02000000
#define TFD_NONBLOCK 00004000

// poll.h
#define POLLIN 0x0001
#define POLLPRI 0x0002
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020

#define POLLRDNORM 0x0040
#define POLLRDBAND 0x0080
#define POLLWRNORM 0x0100
#define POLLWRBAND 0x0200
#define POLLRDHUP 0x2000

struct pollfd {
    int32_t fd;
    uint16_t events;
    uint16_t revents;
};

// eventpoll.h
#define EPOLL_CLOEXEC 02000000
#define EPOLL_NONBLOCK 00004000

#define EPOLLIN 0x001
#define EPOLLPRI 0x002
#define EPOLLOUT 0x004
#define EPOLLRDNORM 0x040
#define EPOLLNVAL 0x020
#define EPOLLRDBAND 0x080
#define EPOLLWRNORM 0x100
#define EPOLLWRBAND 0x200
#define EPOLLMSG 0x400
#define EPOLLERR 0x008
#define EPOLLHUP 0x010
#define EPOLLRDHUP 0x2000
#define EPOLLEXCLUSIVE (1U<<28)
#define EPOLLWAKEUP (1U<<29)
#define EPOLLONESHOT (1U<<30)
#define EPOLLET (1U<<31)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

union epoll_data {
    void *ptr;
    int32_t fd;
    uint32_t u32;
    uint64_t u64;
} hc_PACKED(4);

struct epoll_event {
    uint32_t events;
#if !hc_X86_64
    uint32_t __pad;
#endif
    union epoll_data data;
};

// if_alg.h
struct sockaddr_alg {
    uint16_t salg_family;
    uint8_t salg_type[14];
    uint32_t salg_feat;
    uint32_t salg_mask;
    uint8_t salg_name[64];
};

// mman.h + mman-common.h
#define MREMAP_MAYMOVE 1
#define MREMAP_FIXED 2
#define MREMAP_DONTUNMAP 4

#define MAP_GROWSDOWN 0x0100 /* stack-like segment */
#define MAP_DENYWRITE 0x0800 /* ETXTBSY */
#define MAP_EXECUTABLE 0x1000 /* mark it as an executable */
#define MAP_LOCKED 0x2000 /* pages are locked */
#define MAP_NORESERVE 0x4000 /* don't check for reservations */

#define MAP_SHARED 0x01 // Share changes
#define MAP_PRIVATE 0x02 // Changes are private

#define PROT_READ 0x1 // page can be read
#define PROT_WRITE 0x2 // page can be written
#define PROT_EXEC 0x4 // page can be executed
#define PROT_SEM 0x8 // page may be used for atomic ops
#define PROT_NONE 0x0 // page can not be accessed
#define PROT_GROWSDOWN 0x01000000 // mprotect flag: extend change to start of growsdown vma
#define PROT_GROWSUP 0x02000000 // mprotect flag: extend change to end of growsup vma

#define MAP_TYPE 0x0f // Mask for type of mapping
#define MAP_FIXED 0x10 // Interpret addr exactly
#define MAP_ANONYMOUS 0x20 // don't use a file

#define MAP_POPULATE 0x008000 // populate (prefault) pagetables
#define MAP_NONBLOCK 0x010000 // do not block on IO
#define MAP_STACK 0x020000 // give out an address that is best suited for process/thread stacks
#define MAP_HUGETLB 0x040000 // create a huge page mapping
#define MAP_SYNC 0x080000 // perform synchronous page faults for the mapping
#define MAP_FIXED_NOREPLACE 0x100000 // MAP_FIXED which doesn't unmap underlying mapping

#define MAP_UNINITIALIZED 0x4000000 // For anonymous mmap, memory could be uninitialized

#define MS_ASYNC 1 /* sync memory asynchronously */
#define MS_INVALIDATE 2 /* invalidate the caches */
#define MS_SYNC 4 /* synchronous memory sync */

#define MADV_NORMAL 0 // no further special treatment
#define MADV_RANDOM 1 // expect random page references
#define MADV_SEQUENTIAL 2 // expect sequential page references
#define MADV_WILLNEED 3 // will need these pages
#define MADV_DONTNEED 4 // don't need these pages
#define MADV_FREE 8 // free pages only if memory pressure
#define MADV_REMOVE 9 // remove these pages & resources
#define MADV_DONTFORK 10 // don't inherit across fork
#define MADV_DOFORK 11 // do inherit across fork
#define MADV_HWPOISON 100 // poison a page for testing
#define MADV_SOFT_OFFLINE 101 // soft offline page for testing
#define MADV_MERGEABLE 12 // KSM may merge identical pages
#define MADV_UNMERGEABLE 13 // KSM may not merge identical pages
#define MADV_HUGEPAGE 14 // Worth backing with hugepages
#define MADV_NOHUGEPAGE 15 // Not worth backing with hugepages
#define MADV_DONTDUMP 16 // Explicity exclude from the core dump, overrides the coredump filter bits
#define MADV_DODUMP 17 // Clear the MADV_DONTDUMP flag
#define MADV_WIPEONFORK 18 // Zero memory on fork, child only
#define MADV_KEEPONFORK 19 // Undo MADV_WIPEONFORK
#define MADV_COLD 20 // deactivate these pages
#define MADV_PAGEOUT 21 // reclaim these pages
#define MADV_POPULATE_READ 22 // populate (prefault) page tables readable
#define MADV_POPULATE_WRITE 23 // populate (prefault) page tables writable

// signal.h + siginfo.h + signal-defs.h
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGIOT 6
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL SIGIO

#define SA_RESTORER 0x04000000
#define SA_NOCLDSTOP 0x00000001 // flag to turn off SIGCHLD when children stop.
#define SA_NOCLDWAIT 0x00000002 // flag on SIGCHLD to inhibit zombies.
#define SA_SIGINFO 0x00000004 // delivers the signal with SIGINFO structs.
#define SA_EXPOSE_TAGBITS 0x00000800 // exposes an architecture-defined set of tag bits in siginfo.si_addr.
#define SA_ONSTACK 0x08000000 // indicates that a registered stack_t will be used.
#define SA_RESTART 0x10000000 // flag to get restarting signals (which were the default long ago)
#define SA_NODEFER 0x40000000 // prevents the current signal from being masked in the handler.
#define SA_RESETHAND 0x80000000 // clears the handler when the signal is delivered.

#define SA_NOMASK 0x40000000
#define SA_ONESHOT 0x80000000

#define SIG_BLOCK 0 // for blocking signals
#define SIG_UNBLOCK 1 // for unblocking signals
#define SIG_SETMASK 2 // for setting the signal mask

#define SIG_DFL ((void *)0) // default signal handling
#define SIG_IGN ((void *)1) // ignore signal
#define SIG_ERR ((void *)-1) // error return from signal

union sigval {
    int32_t sival_int;
    void *sival_ptr;
};

struct siginfo {
    int32_t si_signo;
    int32_t si_errno;
    int32_t si_code;
    int32_t __pad;
    union {
        char __pad[128 - 16];
        // kill()
        struct {
            int32_t _pid; // sender's pid
            uint32_t _uid; // sender's uid
        } _kill;

        // POSIX.1b timers
        struct {
            int32_t _tid; // timer id
            int32_t _overrun; // overrun count
            union sigval _sigval; // same as below
            int32_t _sys_private; // not to be passed to user
            int32_t __pad;
        } _timer;

        // POSIX.1b signals
        struct {
            int32_t _pid; // sender's pid
            uint32_t _uid; // sender's uid
            union sigval _sigval;
        } _rt;

        // SIGCHLD
        struct {
            int32_t _pid; // which child
            uint32_t _uid; // sender's uid
            int32_t _status; // exit code
            int32_t __pad;
            int64_t _utime;
            int64_t _stime;
        } _sigchld;

        // SIGILL, SIGFPE, SIGSEGV, SIGBUS, SIGTRAP, SIGEMT
        struct {
            void *_addr; // faulting insn/memory ref.

            union {
                // used on alpha and sparc
                int32_t _trapno; // TRAP # which caused the signal
                /*
                * used when si_code=BUS_MCEERR_AR or
                * used when si_code=BUS_MCEERR_AO
                */
                int16_t _addr_lsb; // LSB of the reported address
                // used when si_code=SEGV_BNDERR
                struct {
                    char _dummy_bnd[8];
                    void *_lower;
                    void *_upper;
                } _addr_bnd;
                // used when si_code=SEGV_PKUERR
                struct {
                    char _dummy_pkey[8];
                    uint32_t _pkey;
                } _addr_pkey;
                // used when si_code=TRAP_PERF
                struct {
                    uint64_t _data;
                    uint32_t _type;
                    int32_t __pad;
                } _perf;
            };
        } _sigfault;

        // SIGPOLL
        struct {
            int64_t _band; // POLL_IN, POLL_OUT, POLL_MSG
            int32_t _fd;
            int32_t __pad;
        } _sigpoll;

        // SIGSYS
        struct {
            void *_call_addr; // calling user insn
            int32_t _syscall; // triggering system call number
            uint32_t _arch; // AUDIT_ARCH_* of syscall
        } _sigsys;
    } _sifields;
}
#if hc_X86_64
hc_ALIGNED(8)
#endif
;

#define si_pid _sifields._kill._pid
#define si_uid _sifields._kill._uid
#define si_tid _sifields._timer._tid
#define si_overrun _sifields._timer._overrun
#define si_sys_private _sifields._timer._sys_private
#define si_status _sifields._sigchld._status
#define si_utime _sifields._sigchld._utime
#define si_stime _sifields._sigchld._stime
#define si_value _sifields._rt._sigval
#define si_int _sifields._rt._sigval.sival_int
#define si_ptr _sifields._rt._sigval.sival_ptr
#define si_addr _sifields._sigfault._addr
#define si_trapno _sifields._sigfault._trapno
#define si_addr_lsb _sifields._sigfault._addr_lsb
#define si_lower _sifields._sigfault._addr_bnd._lower
#define si_upper _sifields._sigfault._addr_bnd._upper
#define si_pkey _sifields._sigfault._addr_pkey._pkey
#define si_perf_data _sifields._sigfault._perf._data
#define si_perf_type _sifields._sigfault._perf._type
#define si_band _sifields._sigpoll._band
#define si_fd _sifields._sigpoll._fd
#define si_call_addr _sifields._sigsys._call_addr
#define si_syscall _sifields._sigsys._syscall
#define si_arch _sifields._sigsys._arch

/*
 * si_code values
 * Digital reserves positive values for kernel-generated signals.
 */
#define SI_USER 0 // sent by kill, sigsend, raise
#define SI_KERNEL 0x80 // sent by the kernel from somewhere
#define SI_QUEUE -1 // sent by sigqueue
#define SI_TIMER -2 // sent by timer expiration
#define SI_MESGQ -3 // sent by real time mesq state change
#define SI_ASYNCIO -4 // sent by AIO completion
#define SI_SIGIO -5 // sent by queued SIGIO
#define SI_TKILL -6 // sent by tkill system call
#define SI_DETHREAD -7 // sent by execve() killing subsidiary threads
#define SI_ASYNCNL -60 // sent by glibc async name lookup completion

/*
 * SIGCHLD si_codes
 */
#define CLD_EXITED 1 // child has exited
#define CLD_KILLED 2 // child was killed
#define CLD_DUMPED 3 // child terminated abnormally
#define CLD_TRAPPED 4 // traced child has trapped
#define CLD_STOPPED 5 // child has stopped
#define CLD_CONTINUED 6 // stopped child has continued

struct sigaction {
    union {
        void (*sa_handler)(int32_t sig);
        void (*sa_sigaction)(int32_t sig, struct siginfo *info, void *ucontext);
    };
    uint64_t sa_flags;
#if hc_X86_64
    void (*sa_restorer)(void);
#endif
    uint64_t sa_mask;
};

// random.h
#define GRND_NONBLOCK 0x0001
#define GRND_RANDOM 0x0002
#define GRND_INSECURE 0x0004

// auxvec.h
#define AT_NULL 0 // end of vector
#define AT_IGNORE 1 // entry should be ignored
#define AT_EXECFD 2 // file descriptor of program
#define AT_PHDR 3 // program headers for program
#define AT_PHENT 4 // size of program header entry
#define AT_PHNUM 5 // number of program headers
#define AT_PAGESZ 6 // system page size
#define AT_BASE 7 // base address of interpreter
#define AT_FLAGS 8 // flags
#define AT_ENTRY 9 // entry point of program
#define AT_NOTELF 10 // program is not ELF
#define AT_UID 11 // real uid
#define AT_EUID 12 // effective uid
#define AT_GID 13 // real gid
#define AT_EGID 14 // effective gid
#define AT_PLATFORM 15 // string identifying CPU for optimizations
#define AT_HWCAP 16 // arch dependent hints at CPU capabilities
#define AT_CLKTCK 17 // frequency at which times() increments
#define AT_SECURE 23 // secure mode boolean
#define AT_BASE_PLATFORM 24 // string identifying real platform, may differ from AT_PLATFORM
#define AT_RANDOM 25 // address of 16 random bytes
#define AT_HWCAP2 26 // extension of AT_HWCAP
#define AT_EXECFN 31 // filename of program
#define AT_SYSINFO_EHDR 33 // vDSO address
#define AT_MINSIGSTKSZ 51 // minimal stack size for signal delivery

// sched.h
#define CSIGNAL 0x000000ff
#define CLONE_VM 0x00000100 // set if VM shared between processes
#define CLONE_FS 0x00000200 // set if fs info shared between processes
#define CLONE_FILES 0x00000400 // set if open files shared between processes
#define CLONE_SIGHAND 0x00000800 // set if signal handlers and blocked signals shared
#define CLONE_PIDFD 0x00001000 // set if a pidfd should be placed in parent
#define CLONE_PTRACE 0x00002000 // set if we want to let tracing continue on the child too
#define CLONE_VFORK 0x00004000 // set if the parent wants the child to wake it up on mm_release
#define CLONE_PARENT 0x00008000 // set if we want to have the same parent as the cloner
#define CLONE_THREAD 0x00010000 // Same thread group?
#define CLONE_NEWNS 0x00020000 // New mount namespace group
#define CLONE_SYSVSEM 0x00040000 // share system V SEM_UNDO semantics
#define CLONE_SETTLS 0x00080000 // create a new TLS for the child
#define CLONE_PARENT_SETTID 0x00100000 // set the TID in the parent
#define CLONE_CHILD_CLEARTID 0x00200000 // clear the TID in the child
#define CLONE_UNTRACED 0x00800000 // set if the tracing process can't force CLONE_PTRACE on this clone
#define CLONE_CHILD_SETTID 0x01000000 // set the TID in the child
#define CLONE_NEWCGROUP 0x02000000 // New cgroup namespace
#define CLONE_NEWUTS 0x04000000 // New utsname namespace
#define CLONE_NEWIPC 0x08000000 // New ipc namespace
#define CLONE_NEWUSER 0x10000000 // New user namespace
#define CLONE_NEWPID 0x20000000 // New pid namespace
#define CLONE_NEWNET 0x40000000 // New network namespace
#define CLONE_IO 0x80000000 // Clone io context
// clone3 only:
#define CLONE_CLEAR_SIGHAND 0x100000000ULL // Clear any signal handler and reset to SIG_DFL.
#define CLONE_INTO_CGROUP 0x200000000ULL // Clone into a specific cgroup given the right permissions.
#define CLONE_NEWTIME 0x00000080 // New time namespace

struct clone_args {
    uint64_t flags;
    int32_t *pidfd;
    int32_t *child_tid;
    int32_t *parent_tid;
    int64_t exit_signal;
    void *stack;
    int64_t stack_size;
    uint64_t tls;
    int32_t *set_tid;
    int64_t set_tid_size;
    int64_t cgroup;
};

// futex.h
#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_FD 2
#define FUTEX_REQUEUE 3
#define FUTEX_CMP_REQUEUE 4
#define FUTEX_WAKE_OP 5
#define FUTEX_LOCK_PI 6
#define FUTEX_UNLOCK_PI 7
#define FUTEX_TRYLOCK_PI 8
#define FUTEX_WAIT_BITSET 9
#define FUTEX_WAKE_BITSET 10
#define FUTEX_WAIT_REQUEUE_PI 11
#define FUTEX_CMP_REQUEUE_PI 12
#define FUTEX_LOCK_PI2 13

#define FUTEX_PRIVATE_FLAG 128
#define FUTEX_CLOCK_REALTIME 256
#define FUTEX_CMD_MASK ~(FUTEX_PRIVATE_FLAG | FUTEX_CLOCK_REALTIME)

#define FUTEX_WAIT_PRIVATE (FUTEX_WAIT | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAKE_PRIVATE (FUTEX_WAKE | FUTEX_PRIVATE_FLAG)
#define FUTEX_REQUEUE_PRIVATE (FUTEX_REQUEUE | FUTEX_PRIVATE_FLAG)
#define FUTEX_CMP_REQUEUE_PRIVATE (FUTEX_CMP_REQUEUE | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAKE_OP_PRIVATE (FUTEX_WAKE_OP | FUTEX_PRIVATE_FLAG)
#define FUTEX_LOCK_PI_PRIVATE (FUTEX_LOCK_PI | FUTEX_PRIVATE_FLAG)
#define FUTEX_LOCK_PI2_PRIVATE (FUTEX_LOCK_PI2 | FUTEX_PRIVATE_FLAG)
#define FUTEX_UNLOCK_PI_PRIVATE (FUTEX_UNLOCK_PI | FUTEX_PRIVATE_FLAG)
#define FUTEX_TRYLOCK_PI_PRIVATE (FUTEX_TRYLOCK_PI | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAIT_BITSET_PRIVATE (FUTEX_WAIT_BITSET | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAKE_BITSET_PRIVATE (FUTEX_WAKE_BITSET | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAIT_REQUEUE_PI_PRIVATE (FUTEX_WAIT_REQUEUE_PI | FUTEX_PRIVATE_FLAG)
#define FUTEX_CMP_REQUEUE_PI_PRIVATE (FUTEX_CMP_REQUEUE_PI | FUTEX_PRIVATE_FLAG)

// ioctl.h
#define _IOC_NRBITS 8
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14
#define _IOC_DIRBITS 2

#define _IOC_NRMASK ((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK ((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK ((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK ((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT 0
#define _IOC_TYPESHIFT (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC_NONE 0U
#define _IOC_WRITE 1U
#define _IOC_READ 2U

#define _IOC(dir,type,nr,size) (((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) | ((nr) << _IOC_NRSHIFT) | ((size) << _IOC_SIZESHIFT))
#define _IOC_TYPECHECK(t) (sizeof(t))

#define _IO(type,nr) _IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size) _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size) _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOWR(type,nr,size) _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

// termbits.h
#define NCCS 19
struct termios {
    uint32_t c_iflag; /* input mode flags */
    uint32_t c_oflag; /* output mode flags */
    uint32_t c_cflag; /* control mode flags */
    uint32_t c_lflag; /* local mode flags */
    uint8_t c_line; /* line discipline */
    uint8_t c_cc[NCCS]; /* control characters */
};

struct termios2 {
    uint32_t c_iflag; /* input mode flags */
    uint32_t c_oflag; /* output mode flags */
    uint32_t c_cflag; /* control mode flags */
    uint32_t c_lflag; /* local mode flags */
    uint8_t c_line; /* line discipline */
    uint8_t c_cc[NCCS]; /* control characters */
    uint32_t c_ispeed; /* input speed */
    uint32_t c_ospeed; /* output speed */
};

/* c_cc characters */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSWTC 7
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define VEOL 11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

/* c_iflag bits */
#define IGNBRK 0000001
#define BRKINT 0000002
#define IGNPAR 0000004
#define PARMRK 0000010
#define INPCK 0000020
#define ISTRIP 0000040
#define INLCR 0000100
#define IGNCR 0000200
#define ICRNL 0000400
#define IUCLC 0001000
#define IXON 0002000
#define IXANY 0004000
#define IXOFF 0010000
#define IMAXBEL 0020000
#define IUTF8 0040000

/* c_oflag bits */
#define OPOST 0000001U
#define OLCUC 0000002U
#define ONLCR 0000004U
#define OCRNL 0000010U
#define ONOCR 0000020U
#define ONLRET 0000040U
#define OFILL 0000100U
#define OFDEL 0000200U
#define NLDLY 0000400U
#define NL0 0000000U
#define NL1 0000400U
#define CRDLY 0003000U
#define CR0 0000000U
#define CR1 0001000U
#define CR2 0002000U
#define CR3 0003000U
#define TABDLY 0014000U
#define TAB0 0000000U
#define TAB1 0004000U
#define TAB2 0010000U
#define TAB3 0014000U
#define XTABS 0014000U
#define BSDLY 0020000U
#define BS0 0000000U
#define BS1 0020000U
#define VTDLY 0040000U
#define VT0 0000000U
#define VT1 0040000U
#define FFDLY 0100000U
#define FF0 0000000U
#define FF1 0100000U

/* c_cflag bit meaning */
#define CBAUD 0010017U
#define B0 0000000U /* hang up */
#define B50 0000001U
#define B75 0000002U
#define B110 0000003U
#define B134 0000004U
#define B150 0000005U
#define B200 0000006U
#define B300 0000007U
#define B600 0000010U
#define B1200 0000011U
#define B1800 0000012U
#define B2400 0000013U
#define B4800 0000014U
#define B9600 0000015U
#define B19200 0000016U
#define B38400 0000017U
#define CSIZE 0000060U
#define CS5 0000000U
#define CS6 0000020U
#define CS7 0000040U
#define CS8 0000060U
#define CSTOPB 0000100U
#define CREAD 0000200U
#define PARENB 0000400U
#define PARODD 0001000U
#define HUPCL 0002000U
#define CLOCAL 0004000U
#define CBAUDEX 0010000U
#define BOTHER 0010000U
#define B57600 0010001U
#define B115200 0010002U
#define B230400 0010003U
#define B460800 0010004U
#define B500000 0010005U
#define B576000 0010006U
#define B921600 0010007U
#define B1000000 0010010U
#define B1152000 0010011U
#define B1500000 0010012U
#define B2000000 0010013U
#define B2500000 0010014U
#define B3000000 0010015U
#define B3500000 0010016U
#define B4000000 0010017U
#define CIBAUD 002003600000U /* input baud rate */
#define CMSPAR 010000000000U /* mark or space (stick) parity */
#define CRTSCTS 020000000000U /* flow control */

#define IBSHIFT 16 /* Shift from CBAUD to CIBAUD */

/* c_lflag bits */
#define ISIG 0000001U
#define ICANON 0000002U
#define XCASE 0000004U
#define ECHO 0000010U
#define ECHOE 0000020U
#define ECHOK 0000040U
#define ECHONL 0000100U
#define NOFLSH 0000200U
#define TOSTOP 0000400U
#define ECHOCTL 0001000U
#define ECHOPRT 0002000U
#define ECHOKE 0004000U
#define FLUSHO 0010000U
#define PENDIN 0040000U
#define IEXTEN 0100000U
#define EXTPROC 0200000U

/* tcflow() and TCXONC use these */
#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

/* tcflush() and TCFLSH use these */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/* tcsetattr uses these */
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

// ioctls.h
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCSETSW 0x5403
#define TCSETSF 0x5404
#define TCGETA 0x5405
#define TCSETA 0x5406
#define TCSETAW 0x5407
#define TCSETAF 0x5408
#define TCSBRK 0x5409
#define TCXONC 0x540A
#define TCFLSH 0x540B
#define TIOCEXCL 0x540C
#define TIOCNXCL 0x540D
#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCOUTQ 0x5411
#define TIOCSTI 0x5412
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCMGET 0x5415
#define TIOCMBIS 0x5416
#define TIOCMBIC 0x5417
#define TIOCMSET 0x5418
#define TIOCGSOFTCAR 0x5419
#define TIOCSSOFTCAR 0x541A
#define FIONREAD 0x541B
#define TIOCINQ FIONREAD
#define TIOCLINUX 0x541C
#define TIOCCONS 0x541D
#define TIOCGSERIAL 0x541E
#define TIOCSSERIAL 0x541F
#define TIOCPKT 0x5420
#define FIONBIO 0x5421
#define TIOCNOTTY 0x5422
#define TIOCSETD 0x5423
#define TIOCGETD 0x5424
#define TCSBRKP 0x5425 // Needed for POSIX tcsendbreak()
#define TIOCSBRK 0x5427 // BSD compatibility
#define TIOCCBRK 0x5428 // BSD compatibility
#define TIOCGSID 0x5429 // Return the session ID of FD

#define TIOCSPTLCK _IOW('T', 0x31, int32_t)  // Lock/unlock Pty
#define TIOCGPTPEER _IO('T', 0x41) // Safely open the slave

// termios.h
struct winsize {
    uint16_t ws_row;
    uint16_t ws_col;
    uint16_t ws_xpixel;
    uint16_t ws_ypixel;
};

// vt.h
#define MIN_NR_CONSOLES 1
#define MAX_NR_CONSOLES 63 // serial lines start at 64
#define VT_OPENQRY 0x5600 // find available vt

struct vt_mode {
    uint8_t mode; // vt mode
    uint8_t waitv; // if set, hang on writes if not active
    int16_t relsig; // signal to raise on release req
    int16_t acqsig; // signal to raise on acquisition
    int16_t frsig; // unused (set to 0)
};
#define VT_GETMODE 0x5601 // get mode of active vt
#define VT_SETMODE 0x5602 // set mode of active vt
#define VT_AUTO 0x00 // auto vt switching
#define VT_PROCESS 0x01 // process controls switching
#define VT_ACKACQ 0x02 // acknowledge switch

struct vt_stat {
    uint16_t v_active; // active vt
    uint16_t v_signal; // signal to send
    uint16_t v_state; // vt bitmask
};
#define VT_GETSTATE 0x5603 // get global vt state info
#define VT_SENDSIG 0x5604 // signal to send to bitmask of vts

#define VT_RELDISP 0x5605 // release display

#define VT_ACTIVATE 0x5606 // make vt active
#define VT_WAITACTIVE 0x5607 // wait for vt active
#define VT_DISALLOCATE 0x5608 // free memory associated to vt

struct vt_sizes {
    uint16_t v_rows; // number of rows
    uint16_t v_cols; // number of columns
    uint16_t v_scrollsize; // number of lines of scrollback
};
#define VT_RESIZE 0x5609 // set kernel's idea of screensize

struct vt_consize {
    uint16_t v_rows; // number of rows
    uint16_t v_cols; // number of columns
    uint16_t v_vlin; // number of pixel rows on screen
    uint16_t v_clin; // number of pixel rows per character
    uint16_t v_vcol; // number of pixel columns on screen
    uint16_t v_ccol; // number of pixel columns per character
};
#define VT_RESIZEX 0x560A // set kernel's idea of screensize + more
#define VT_LOCKSWITCH 0x560B // disallow vt switching
#define VT_UNLOCKSWITCH 0x560C // allow vt switching
#define VT_GETHIFONTMASK 0x560D // return hi font mask

#define VT_EVENT_SWITCH 0x0001 // Console switch
#define VT_EVENT_BLANK 0x0002 // Screen blank
#define VT_EVENT_UNBLANK 0x0004 // Screen unblank
#define VT_EVENT_RESIZE 0x0008 // Resize display
#define VT_MAX_EVENT 0x000F
struct vt_event {
    uint32_t event;
    uint32_t oldev; // Old console
    uint32_t newev; // New console (if changing)
    uint32_t __pad[4]; // Padding for expansion
};

#define VT_WAITEVENT 0x560E // Wait for an event

struct vt_setactivate {
    uint32_t console;
    struct vt_mode mode;
};

#define VT_SETACTIVATE 0x560F // Activate and set the mode of a console

// kd.h
#define KDSETMODE 0x4B3A // set text/graphics mode
#define KD_TEXT 0x00
#define KD_GRAPHICS 0x01
#define KD_TEXT0 0x02 // obsolete
#define KD_TEXT1 0x03 // obsolete
#define KDGETMODE 0x4B3B // get current mode

#define K_RAW 0x00
#define K_XLATE 0x01
#define K_MEDIUMRAW 0x02
#define K_UNICODE 0x03
#define K_OFF 0x04
#define KDGKBMODE 0x4B44 // gets current keyboard mode
#define KDSKBMODE 0x4B45 // sets current keyboard mode

#define K_METABIT 0x03
#define K_ESCPREFIX 0x04
#define KDGKBMETA 0x4B62 // gets meta key handling mode
#define KDSKBMETA 0x4B63 // sets meta key handling mode

struct kbentry {
    uint8_t kb_table;
    uint8_t kb_index;
    uint16_t kb_value;
};
#define K_NORMTAB 0x00
#define K_SHIFTTAB 0x01
#define K_ALTTAB 0x02
#define K_ALTSHIFTTAB 0x03

#define KDGKBENT 0x4B46 // gets one entry in translation table
#define KDSKBENT 0x4B47 // sets one entry in translation table

// signalfd.h
#define SFD_CLOEXEC 02000000
#define SFD_NONBLOCK 00004000

struct signalfd_siginfo {
    uint32_t ssi_signo;
    int32_t ssi_errno;
    int32_t ssi_code;
    uint32_t ssi_pid;
    uint32_t ssi_uid;
    int32_t ssi_fd;
    uint32_t ssi_tid;
    uint32_t ssi_band;
    uint32_t ssi_overrun;
    uint32_t ssi_trapno;
    int32_t ssi_status;
    int32_t ssi_int;
    uint64_t ssi_ptr;
    uint64_t ssi_utime;
    uint64_t ssi_stime;
    uint64_t ssi_addr;
    uint16_t ssi_addr_lsb;
    uint16_t __pad2;
    int32_t ssi_syscall;
    uint64_t ssi_call_addr;
    uint32_t ssi_arch;
    uint8_t __pad[28];
};

// prctl.h
#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004
#define ARCH_GET_CPUID 0x1011
#define ARCH_SET_CPUID 0x1012

// stat.h
#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

struct statx_timestamp {
    int64_t tv_sec;
    uint32_t tv_nsec;
    int32_t __reserved;
};

struct statx {
    uint32_t stx_mask; /* What results were written [uncond] */
    uint32_t stx_blksize; /* Preferred general I/O size [uncond] */
    uint64_t stx_attributes; /* Flags conveying information about the file [uncond] */
    uint32_t stx_nlink; /* Number of hard links */
    uint32_t stx_uid; /* User ID of owner */
    uint32_t stx_gid; /* Group ID of owner */
    uint16_t stx_mode; /* File mode */
    uint16_t __spare0[1];
    uint64_t stx_ino; /* Inode number */
    int64_t stx_size; /* File size */
    uint64_t stx_blocks; /* Number of 512-byte blocks allocated */
    uint64_t stx_attributes_mask; /* Mask to show what's supported in stx_attributes */
    struct statx_timestamp stx_atime; /* Last access time */
    struct statx_timestamp stx_btime; /* File creation time */
    struct statx_timestamp stx_ctime; /* Last attribute change time */
    struct statx_timestamp stx_mtime; /* Last data modification time */
    uint32_t stx_rdev_major; /* Device ID of special file [if bdev/cdev] */
    uint32_t stx_rdev_minor;
    uint32_t stx_dev_major; /* ID of device containing file [uncond] */
    uint32_t stx_dev_minor;
    uint64_t stx_mnt_id;
    uint64_t __spare2;
    uint64_t __spare3[12]; /* Spare space for future expansion */
};

// Bits for mask argument and `stx_mask`.
#define STATX_TYPE 0x00000001U /* Want/got stx_mode & S_IFMT */
#define STATX_MODE 0x00000002U /* Want/got stx_mode & ~S_IFMT */
#define STATX_NLINK 0x00000004U /* Want/got stx_nlink */
#define STATX_UID 0x00000008U /* Want/got stx_uid */
#define STATX_GID 0x00000010U /* Want/got stx_gid */
#define STATX_ATIME 0x00000020U /* Want/got stx_atime */
#define STATX_MTIME 0x00000040U /* Want/got stx_mtime */
#define STATX_CTIME 0x00000080U /* Want/got stx_ctime */
#define STATX_INO 0x00000100U /* Want/got stx_ino */
#define STATX_SIZE 0x00000200U /* Want/got stx_size */
#define STATX_BLOCKS 0x00000400U /* Want/got stx_blocks */
#define STATX_BASIC_STATS 0x000007ffU /* The stuff in the normal stat struct */
#define STATX_BTIME 0x00000800U /* Want/got stx_btime */
#define STATX_MNT_ID 0x00001000U /* Got stx_mnt_id */

// Bits in `stx_attributes` and `stx_attributes_mask`.
#define STATX_ATTR_COMPRESSED 0x00000004 /* [I] File is compressed by the fs */
#define STATX_ATTR_IMMUTABLE 0x00000010 /* [I] File is marked immutable */
#define STATX_ATTR_APPEND 0x00000020 /* [I] File is append-only */
#define STATX_ATTR_NODUMP 0x00000040 /* [I] File is not to be dumped */
#define STATX_ATTR_ENCRYPTED 0x00000800 /* [I] File requires key to decrypt in fs */
#define STATX_ATTR_AUTOMOUNT 0x00001000 /* Dir: Automount trigger */
#define STATX_ATTR_MOUNT_ROOT 0x00002000 /* Root of a mount */
#define STATX_ATTR_VERITY 0x00100000 /* [I] Verity protected file */
#define STATX_ATTR_DAX 0x00200000 /* File is currently in DAX state */

#if hc_X86_64
struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    uint64_t st_nlink;
    uint32_t st_mode;
    uint32_t st_uid;
    uint32_t st_gid;
    uint32_t __pad;
    uint64_t st_rdev;
    int64_t st_size;
    int64_t st_blksize;
    int64_t st_blocks;
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    int64_t unused[3];
};
#else
struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    uint64_t __pad;
    int64_t st_size;
    int32_t st_blksize;
    int32_t __pad2;
    int64_t st_blocks;
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    uint32_t unused[2];
};
#endif

// reboot.h
#define LINUX_REBOOT_MAGIC1 0xfee1dead
#define LINUX_REBOOT_MAGIC2 672274793
#define LINUX_REBOOT_MAGIC2A 85072278
#define LINUX_REBOOT_MAGIC2B 369367448
#define LINUX_REBOOT_MAGIC2C 537993216

#define LINUX_REBOOT_CMD_RESTART 0x01234567
#define LINUX_REBOOT_CMD_HALT 0xCDEF0123
#define LINUX_REBOOT_CMD_CAD_ON 0x89ABCDEF
#define LINUX_REBOOT_CMD_CAD_OFF 0x00000000
#define LINUX_REBOOT_CMD_POWER_OFF 0x4321FEDC
#define LINUX_REBOOT_CMD_RESTART2 0xA1B2C3D4
#define LINUX_REBOOT_CMD_SW_SUSPEND 0xD000FCE2
#define LINUX_REBOOT_CMD_KEXEC 0x45584543

// resource.h
struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
    int64_t ru_maxrss;
    int64_t ru_ixrss;
    int64_t ru_idrss;
    int64_t ru_isrss;
    int64_t ru_minflt;
    int64_t ru_majflt;
    int64_t ru_nswap;
    int64_t ru_inblock;
    int64_t ru_oublock;
    int64_t ru_msgsnd;
    int64_t ru_msgrcv;
    int64_t ru_nsignals;
    int64_t ru_nvcsw;
    int64_t ru_nivcsw;
};

// wait.h
#define WNOHANG 0x00000001
#define WUNTRACED 0x00000002
#define WSTOPPED WUNTRACED
#define WEXITED 0x00000004
#define WCONTINUED 0x00000008
#define WNOWAIT 0x01000000 /* Don't reap, just poll status. */

#define __WNOTHREAD 0x20000000 /* Don't wait on children of other threads in this group */
#define __WALL 0x40000000 /* Wait on all children, regardless of type */
#define __WCLONE 0x80000000 /* Wait only on non-SIGCHLD children */

#define P_ALL 0
#define P_PID 1
#define P_PGID 2
#define P_PIDFD 3

// netlink.h
#define NETLINK_ROUTE 0 /* Routing/device hook */
#define NETLINK_UNUSED 1 /* Unused number */
#define NETLINK_USERSOCK 2 /* Reserved for user mode socket protocols */
#define NETLINK_FIREWALL 3 /* Unused number, formerly ip_queue */
#define NETLINK_SOCK_DIAG 4 /* socket monitoring */
#define NETLINK_NFLOG 5 /* netfilter/iptables ULOG */
#define NETLINK_XFRM 6 /* ipsec */
#define NETLINK_SELINUX 7 /* SELinux event notifications */
#define NETLINK_ISCSI 8 /* Open-iSCSI */
#define NETLINK_AUDIT 9 /* auditing */
#define NETLINK_FIB_LOOKUP 10
#define NETLINK_CONNECTOR 11
#define NETLINK_NETFILTER 12 /* netfilter subsystem */
#define NETLINK_IP6_FW 13
#define NETLINK_DNRTMSG 14 /* DECnet routing messages */
#define NETLINK_KOBJECT_UEVENT 15 /* Kernel messages to userspace */
#define NETLINK_GENERIC 16

struct sockaddr_nl {
    uint16_t nl_family;
    uint16_t nl_pad;
    uint32_t nl_pid; /* port ID */
    uint32_t nl_groups; /* multicast groups mask */
};

struct nlmsghdr {
    uint32_t nlmsg_len; /* Length of message including header */
    uint16_t nlmsg_type; /* Message content */
    uint16_t nlmsg_flags; /* Additional flags */
    uint32_t nlmsg_seq; /* Sequence number */
    uint32_t nlmsg_pid; /* Sending process port ID */
};

/* Flags values */
#define NLM_F_REQUEST 0x01 /* It is request message. */
#define NLM_F_MULTI 0x02 /* Multipart message, terminated by NLMSG_DONE */
#define NLM_F_ACK 0x04 /* Reply with ack, with zero or error code */
#define NLM_F_ECHO 0x08 /* Echo this request */
#define NLM_F_DUMP_INTR 0x10 /* Dump was inconsistent due to sequence change */
#define NLM_F_DUMP_FILTERED 0x20 /* Dump was filtered as requested */

/* Modifiers to GET request */
#define NLM_F_ROOT 0x100 /* specify tree root */
#define NLM_F_MATCH 0x200 /* return all matching */
#define NLM_F_ATOMIC 0x400 /* atomic GET */
#define NLM_F_DUMP (NLM_F_ROOT|NLM_F_MATCH)

/* Modifiers to NEW request */
#define NLM_F_REPLACE 0x100 /* Override existing */
#define NLM_F_EXCL 0x200 /* Do not touch, if it exists */
#define NLM_F_CREATE 0x400 /* Create, if it does not exist */
#define NLM_F_APPEND 0x800 /* Add to end of list */

/* Modifiers to DELETE request */
#define NLM_F_NONREC 0x100 /* Do not delete recursively */

/* Flags for ACK message */
#define NLM_F_CAPPED 0x100 /* request was capped */
#define NLM_F_ACK_TLVS 0x200 /* extended ACK TVLs were included */

#define NLMSG_NOOP 0x1 /* Nothing. */
#define NLMSG_ERROR 0x2 /* Error */
#define NLMSG_DONE 0x3 /* End of a dump */
#define NLMSG_OVERRUN 0x4 /* Data lost */

#define NLMSG_MIN_TYPE 0x10 /* < 0x10: reserved control messages */

/*
 *  <------- NLA_HDRLEN ------> <-- NLA_ALIGN(payload)-->
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 * |        Header       | Pad |     Payload       | Pad |
 * |   (struct nlattr)   | ing |                   | ing |
 * +---------------------+- - -+- - - - - - - - - -+- - -+
 *  <-------------- nlattr->nla_len -------------->
 */
struct nlattr {
    uint16_t nla_len;
    uint16_t nla_type;
};

/*
 * nla_type (16 bits)
 * +---+---+-------------------------------+
 * | N | O | Attribute Type                |
 * +---+---+-------------------------------+
 * N := Carries nested attributes
 * O := Payload stored in network byte order
 *
 * Note: The N and O flag are mutually exclusive.
 */
#define NLA_F_NESTED (1 << 15)
#define NLA_F_NET_BYTEORDER (1 << 14)
#define NLA_TYPE_MASK ~(NLA_F_NESTED | NLA_F_NET_BYTEORDER)

// rtnetlink.h
enum {
    RTM_BASE = 16,
#define RTM_BASE RTM_BASE

    RTM_NEWLINK = 16,
#define RTM_NEWLINK RTM_NEWLINK
    RTM_DELLINK,
#define RTM_DELLINK RTM_DELLINK
    RTM_GETLINK,
#define RTM_GETLINK RTM_GETLINK
    RTM_SETLINK,
#define RTM_SETLINK RTM_SETLINK

    RTM_NEWADDR = 20,
#define RTM_NEWADDR RTM_NEWADDR
    RTM_DELADDR,
#define RTM_DELADDR RTM_DELADDR
    RTM_GETADDR,
#define RTM_GETADDR RTM_GETADDR

    RTM_NEWROUTE = 24,
#define RTM_NEWROUTE RTM_NEWROUTE
    RTM_DELROUTE,
#define RTM_DELROUTE RTM_DELROUTE
    RTM_GETROUTE,
#define RTM_GETROUTE RTM_GETROUTE

    RTM_NEWNEIGH = 28,
#define RTM_NEWNEIGH RTM_NEWNEIGH
    RTM_DELNEIGH,
#define RTM_DELNEIGH RTM_DELNEIGH
    RTM_GETNEIGH,
#define RTM_GETNEIGH RTM_GETNEIGH

    RTM_NEWRULE = 32,
#define RTM_NEWRULE RTM_NEWRULE
    RTM_DELRULE,
#define RTM_DELRULE RTM_DELRULE
    RTM_GETRULE,
#define RTM_GETRULE RTM_GETRULE

    RTM_NEWQDISC = 36,
#define RTM_NEWQDISC RTM_NEWQDISC
    RTM_DELQDISC,
#define RTM_DELQDISC RTM_DELQDISC
    RTM_GETQDISC,
#define RTM_GETQDISC RTM_GETQDISC

    RTM_NEWTCLASS = 40,
#define RTM_NEWTCLASS RTM_NEWTCLASS
    RTM_DELTCLASS,
#define RTM_DELTCLASS RTM_DELTCLASS
    RTM_GETTCLASS,
#define RTM_GETTCLASS RTM_GETTCLASS

    RTM_NEWTFILTER = 44,
#define RTM_NEWTFILTER RTM_NEWTFILTER
    RTM_DELTFILTER,
#define RTM_DELTFILTER RTM_DELTFILTER
    RTM_GETTFILTER,
#define RTM_GETTFILTER RTM_GETTFILTER

    RTM_NEWACTION = 48,
#define RTM_NEWACTION   RTM_NEWACTION
    RTM_DELACTION,
#define RTM_DELACTION   RTM_DELACTION
    RTM_GETACTION,
#define RTM_GETACTION   RTM_GETACTION

    RTM_NEWPREFIX = 52,
#define RTM_NEWPREFIX RTM_NEWPREFIX

    RTM_GETMULTICAST = 58,
#define RTM_GETMULTICAST RTM_GETMULTICAST

    RTM_GETANYCAST = 62,
#define RTM_GETANYCAST RTM_GETANYCAST

    RTM_NEWNEIGHTBL = 64,
#define RTM_NEWNEIGHTBL RTM_NEWNEIGHTBL
    RTM_GETNEIGHTBL = 66,
#define RTM_GETNEIGHTBL RTM_GETNEIGHTBL
    RTM_SETNEIGHTBL,
#define RTM_SETNEIGHTBL RTM_SETNEIGHTBL

    RTM_NEWNDUSEROPT = 68,
#define RTM_NEWNDUSEROPT RTM_NEWNDUSEROPT

    RTM_NEWADDRLABEL = 72,
#define RTM_NEWADDRLABEL RTM_NEWADDRLABEL
    RTM_DELADDRLABEL,
#define RTM_DELADDRLABEL RTM_DELADDRLABEL
    RTM_GETADDRLABEL,
#define RTM_GETADDRLABEL RTM_GETADDRLABEL

    RTM_GETDCB = 78,
#define RTM_GETDCB RTM_GETDCB
    RTM_SETDCB,
#define RTM_SETDCB RTM_SETDCB

    RTM_NEWNETCONF = 80,
#define RTM_NEWNETCONF RTM_NEWNETCONF
    RTM_DELNETCONF,
#define RTM_DELNETCONF RTM_DELNETCONF
    RTM_GETNETCONF = 82,
#define RTM_GETNETCONF RTM_GETNETCONF

    RTM_NEWMDB = 84,
#define RTM_NEWMDB RTM_NEWMDB
    RTM_DELMDB = 85,
#define RTM_DELMDB RTM_DELMDB
    RTM_GETMDB = 86,
#define RTM_GETMDB RTM_GETMDB

    RTM_NEWNSID = 88,
#define RTM_NEWNSID RTM_NEWNSID
    RTM_DELNSID = 89,
#define RTM_DELNSID RTM_DELNSID
    RTM_GETNSID = 90,
#define RTM_GETNSID RTM_GETNSID

    RTM_NEWSTATS = 92,
#define RTM_NEWSTATS RTM_NEWSTATS
    RTM_GETSTATS = 94,
#define RTM_GETSTATS RTM_GETSTATS
    RTM_SETSTATS,
#define RTM_SETSTATS RTM_SETSTATS

    RTM_NEWCACHEREPORT = 96,
#define RTM_NEWCACHEREPORT RTM_NEWCACHEREPORT

    RTM_NEWCHAIN = 100,
#define RTM_NEWCHAIN RTM_NEWCHAIN
    RTM_DELCHAIN,
#define RTM_DELCHAIN RTM_DELCHAIN
    RTM_GETCHAIN,
#define RTM_GETCHAIN RTM_GETCHAIN

    RTM_NEWNEXTHOP = 104,
#define RTM_NEWNEXTHOP RTM_NEWNEXTHOP
    RTM_DELNEXTHOP,
#define RTM_DELNEXTHOP RTM_DELNEXTHOP
    RTM_GETNEXTHOP,
#define RTM_GETNEXTHOP RTM_GETNEXTHOP

    RTM_NEWLINKPROP = 108,
#define RTM_NEWLINKPROP RTM_NEWLINKPROP
    RTM_DELLINKPROP,
#define RTM_DELLINKPROP RTM_DELLINKPROP
    RTM_GETLINKPROP,
#define RTM_GETLINKPROP RTM_GETLINKPROP

    RTM_NEWVLAN = 112,
#define RTM_NEWNVLAN RTM_NEWVLAN
    RTM_DELVLAN,
#define RTM_DELVLAN RTM_DELVLAN
    RTM_GETVLAN,
#define RTM_GETVLAN RTM_GETVLAN

    RTM_NEWNEXTHOPBUCKET = 116,
#define RTM_NEWNEXTHOPBUCKET RTM_NEWNEXTHOPBUCKET
    RTM_DELNEXTHOPBUCKET,
#define RTM_DELNEXTHOPBUCKET RTM_DELNEXTHOPBUCKET
    RTM_GETNEXTHOPBUCKET,
#define RTM_GETNEXTHOPBUCKET RTM_GETNEXTHOPBUCKET

    RTM_NEWTUNNEL = 120,
#define RTM_NEWTUNNEL RTM_NEWTUNNEL
    RTM_DELTUNNEL,
#define RTM_DELTUNNEL RTM_DELTUNNEL
    RTM_GETTUNNEL,
#define RTM_GETTUNNEL RTM_GETTUNNEL

    __RTM_MAX,
#define RTM_MAX (((__RTM_MAX + 3) & ~3) - 1)
};

#define RTM_NR_MSGTYPES (RTM_MAX + 1 - RTM_BASE)
#define RTM_NR_FAMILIES (RTM_NR_MSGTYPES >> 2)
#define RTM_FAM(cmd) (((cmd) - RTM_BASE) >> 2)

struct rtmsg {
    uint8_t rtm_family;
    uint8_t rtm_dst_len;
    uint8_t rtm_src_len;
    uint8_t rtm_tos;
    uint8_t rtm_table; /* Routing table id */
    uint8_t rtm_protocol; /* Routing protocol; see below */
    uint8_t rtm_scope; /* See below */
    uint8_t rtm_type; /* See below */
    uint32_t rtm_flags;
};

/* rtm_type */
enum {
    RTN_UNSPEC,
    RTN_UNICAST, /* Gateway or direct route */
    RTN_LOCAL, /* Accept locally */
    RTN_BROADCAST, /* Accept locally as broadcast, send as broadcast */
    RTN_ANYCAST, /* Accept locally as broadcast, but send as unicast */
    RTN_MULTICAST, /* Multicast route */
    RTN_BLACKHOLE, /* Drop */
    RTN_UNREACHABLE, /* Destination is unreachable */
    RTN_PROHIBIT, /* Administratively prohibited */
    RTN_THROW, /* Not in this table */
    RTN_NAT, /* Translate this address */
    RTN_XRESOLVE, /* Use external resolver */
    __RTN_MAX
};
#define RTN_MAX (__RTN_MAX - 1)

/* rtm_protocol */
#define RTPROT_UNSPEC 0
#define RTPROT_REDIRECT 1 /* Route installed by ICMP redirects; not used by current IPv4 */
#define RTPROT_KERNEL 2 /* Route installed by kernel */
#define RTPROT_BOOT 3 /* Route installed during boot */
#define RTPROT_STATIC 4 /* Route installed by administrator */

/* Values of protocol >= RTPROT_STATIC are not interpreted by kernel;
   they are just passed from user and back as is.
   It will be used by hypothetical multiple routing daemons.
   Note that protocol values should be standardized in order to
   avoid conflicts.
 */
#define RTPROT_GATED 8 /* Apparently, GateD */
#define RTPROT_RA 9 /* RDISC/ND router advertisements */
#define RTPROT_MRT 10 /* Merit MRT */
#define RTPROT_ZEBRA 11 /* Zebra */
#define RTPROT_BIRD 12 /* BIRD */
#define RTPROT_DNROUTED 13 /* DECnet routing daemon */
#define RTPROT_XORP 14 /* XORP */
#define RTPROT_NTK 15 /* Netsukuku */
#define RTPROT_DHCP 16 /* DHCP client */
#define RTPROT_MROUTED 17 /* Multicast daemon */
#define RTPROT_KEEPALIVED 18 /* Keepalived daemon */
#define RTPROT_BABEL 42 /* Babel daemon */
#define RTPROT_OPENR 99 /* Open Routing (Open/R) Routes */
#define RTPROT_BGP 186 /* BGP Routes */
#define RTPROT_ISIS 187 /* ISIS Routes */
#define RTPROT_OSPF 188 /* OSPF Routes */
#define RTPROT_RIP 189 /* RIP Routes */
#define RTPROT_EIGRP 192 /* EIGRP Routes */

/* rtm_scope
   Really it is not scope, but sort of distance to the destination.
   NOWHERE are reserved for not existing destinations, HOST is our
   local addresses, LINK are destinations, located on directly attached
   link and UNIVERSE is everywhere in the Universe.

   Intermediate values are also possible f.e. interior routes
   could be assigned a value between UNIVERSE and LINK.
*/
enum rt_scope_t {
    RT_SCOPE_UNIVERSE=0,
/* User defined values */
    RT_SCOPE_SITE=200,
    RT_SCOPE_LINK=253,
    RT_SCOPE_HOST=254,
    RT_SCOPE_NOWHERE=255
};

/* rtm_flags */
#define RTM_F_NOTIFY 0x100 /* Notify user of route change */
#define RTM_F_CLONED 0x200 /* This route is cloned */
#define RTM_F_EQUALIZE 0x400 /* Multipath equalizer: NI */
#define RTM_F_PREFIX 0x800 /* Prefix addresses */
#define RTM_F_LOOKUP_TABLE 0x1000 /* set rtm_table to FIB lookup result */
#define RTM_F_FIB_MATCH 0x2000 /* return full fib lookup match */
#define RTM_F_OFFLOAD 0x4000 /* route is offloaded */
#define RTM_F_TRAP 0x8000 /* route is trapping packets */
#define RTM_F_OFFLOAD_FAILED 0x20000000 /* route offload failed */

/* Reserved table identifiers */
enum rt_class_t {
    RT_TABLE_UNSPEC=0,
/* User defined values */
    RT_TABLE_COMPAT=252,
    RT_TABLE_DEFAULT=253,
    RT_TABLE_MAIN=254,
    RT_TABLE_LOCAL=255,
    RT_TABLE_MAX=0xFFFFFFFF
};

/* Routing message attributes */
enum rtattr_type_t {
    RTA_UNSPEC,
    RTA_DST,
    RTA_SRC,
    RTA_IIF,
    RTA_OIF,
    RTA_GATEWAY,
    RTA_PRIORITY,
    RTA_PREFSRC,
    RTA_METRICS,
    RTA_MULTIPATH,
    RTA_PROTOINFO, /* no longer used */
    RTA_FLOW,
    RTA_CACHEINFO,
    RTA_SESSION, /* no longer used */
    RTA_MP_ALGO, /* no longer used */
    RTA_TABLE,
    RTA_MARK,
    RTA_MFC_STATS,
    RTA_VIA,
    RTA_NEWDST,
    RTA_PREF,
    RTA_ENCAP_TYPE,
    RTA_ENCAP,
    RTA_EXPIRES,
    RTA_PAD,
    RTA_UID,
    RTA_TTL_PROPAGATE,
    RTA_IP_PROTO,
    RTA_SPORT,
    RTA_DPORT,
    RTA_NH_ID,
    __RTA_MAX
};
#define RTA_MAX (__RTA_MAX - 1)

struct ifinfomsg {
    uint8_t ifi_family;
    uint8_t __pad;
    uint16_t ifi_type; /* ARPHRD_* */
    int32_t ifi_index; /* Link index */
    uint32_t ifi_flags; /* IFF_* flags */
    uint32_t ifi_change; /* IFF_* change mask */
};

// if_addr.h
struct ifaddrmsg {
    uint8_t ifa_family;
    uint8_t ifa_prefixlen; /* The prefix length */
    uint8_t ifa_flags; /* Flags */
    uint8_t ifa_scope; /* Address scope */
    int32_t ifa_index; /* Link index */
};

/*
 * Important comment:
 * IFA_ADDRESS is prefix address, rather than local interface address.
 * It makes no difference for normally configured broadcast interfaces,
 * but for point-to-point IFA_ADDRESS is DESTINATION address,
 * local address is supplied in IFA_LOCAL attribute.
 *
 * IFA_FLAGS is a u32 attribute that extends the u8 field ifa_flags.
 * If present, the value from struct ifaddrmsg will be ignored.
 */
enum {
    IFA_UNSPEC,
    IFA_ADDRESS,
    IFA_LOCAL,
    IFA_LABEL,
    IFA_BROADCAST,
    IFA_ANYCAST,
    IFA_CACHEINFO,
    IFA_MULTICAST,
    IFA_FLAGS,
    IFA_RT_PRIORITY, /* u32, priority/metric for prefix route */
    IFA_TARGET_NETNSID,
    IFA_PROTO, /* u8, address protocol */
    __IFA_MAX
};

#define IFA_MAX (__IFA_MAX - 1)

/* ifa_flags */
#define IFA_F_SECONDARY 0x01
#define IFA_F_TEMPORARY IFA_F_SECONDARY

#define IFA_F_NODAD 0x02
#define IFA_F_OPTIMISTIC 0x04
#define IFA_F_DADFAILED 0x08
#define IFA_F_HOMEADDRESS 0x10
#define IFA_F_DEPRECATED 0x20
#define IFA_F_TENTATIVE 0x40
#define IFA_F_PERMANENT 0x80
#define IFA_F_MANAGETEMPADDR 0x100
#define IFA_F_NOPREFIXROUTE 0x200
#define IFA_F_MCAUTOJOIN 0x400
#define IFA_F_STABLE_PRIVACY 0x800

struct ifa_cacheinfo {
    uint32_t ifa_prefered;
    uint32_t ifa_valid;
    uint32_t cstamp; /* created timestamp, hundredths of seconds */
    uint32_t tstamp; /* updated timestamp, hundredths of seconds */
};

// if.h
#define IFNAMSIZ 16

enum net_device_flags {
    IFF_UP = 1<<0, /* sysfs */
    IFF_BROADCAST = 1<<1, /* volatile */
    IFF_DEBUG = 1<<2, /* sysfs */
    IFF_LOOPBACK = 1<<3, /* volatile */
    IFF_POINTOPOINT = 1<<4, /* volatile */
    IFF_NOTRAILERS = 1<<5, /* sysfs */
    IFF_RUNNING = 1<<6, /* volatile */
    IFF_NOARP = 1<<7, /* sysfs */
    IFF_PROMISC = 1<<8, /* sysfs */
    IFF_ALLMULTI = 1<<9, /* sysfs */
    IFF_MASTER = 1<<10, /* volatile */
    IFF_SLAVE = 1<<11, /* volatile */
    IFF_MULTICAST = 1<<12, /* sysfs */
    IFF_PORTSEL = 1<<13, /* sysfs */
    IFF_AUTOMEDIA = 1<<14, /* sysfs */
    IFF_DYNAMIC = 1<<15, /* sysfs */
    IFF_LOWER_UP = 1<<16, /* volatile */
    IFF_DORMANT = 1<<17, /* volatile */
    IFF_ECHO = 1<<18, /* volatile */
};

struct ifmap {
    uint64_t mem_start;
    uint64_t mem_end;
    uint16_t base_addr;
    uint8_t irq;
    uint8_t dma;
    uint8_t port;
    char __pad[3];
};

struct ifreq {
    char ifr_name[IFNAMSIZ]; /* Interface name */
    union {
        char ifr_addr[16];
        int16_t ifr_flags;
        int32_t ifr_ifindex;
        int32_t ifr_metric;
        int32_t ifr_mtu;
        struct ifmap ifr_map;
        char ifr_slave[IFNAMSIZ];
        char ifr_newname[IFNAMSIZ];
        char *ifr_data;
    };
};

// wireguard.h
#define WG_GENL_NAME "wireguard"
#define WG_GENL_VERSION 1

#define WG_KEY_LEN 32

enum wg_cmd {
    WG_CMD_GET_DEVICE,
    WG_CMD_SET_DEVICE,
    __WG_CMD_MAX
};
#define WG_CMD_MAX (__WG_CMD_MAX - 1)

enum wgdevice_flag {
    WGDEVICE_F_REPLACE_PEERS = 1U << 0,
    __WGDEVICE_F_ALL = WGDEVICE_F_REPLACE_PEERS
};
enum wgdevice_attribute {
    WGDEVICE_A_UNSPEC,
    WGDEVICE_A_IFINDEX,
    WGDEVICE_A_IFNAME,
    WGDEVICE_A_PRIVATE_KEY,
    WGDEVICE_A_PUBLIC_KEY,
    WGDEVICE_A_FLAGS,
    WGDEVICE_A_LISTEN_PORT,
    WGDEVICE_A_FWMARK,
    WGDEVICE_A_PEERS,
    __WGDEVICE_A_LAST
};
#define WGDEVICE_A_MAX (__WGDEVICE_A_LAST - 1)

enum wgpeer_flag {
    WGPEER_F_REMOVE_ME = 1U << 0,
    WGPEER_F_REPLACE_ALLOWEDIPS = 1U << 1,
    WGPEER_F_UPDATE_ONLY = 1U << 2,
    __WGPEER_F_ALL = WGPEER_F_REMOVE_ME | WGPEER_F_REPLACE_ALLOWEDIPS | WGPEER_F_UPDATE_ONLY
};
enum wgpeer_attribute {
    WGPEER_A_UNSPEC,
    WGPEER_A_PUBLIC_KEY,
    WGPEER_A_PRESHARED_KEY,
    WGPEER_A_FLAGS,
    WGPEER_A_ENDPOINT,
    WGPEER_A_PERSISTENT_KEEPALIVE_INTERVAL,
    WGPEER_A_LAST_HANDSHAKE_TIME,
    WGPEER_A_RX_BYTES,
    WGPEER_A_TX_BYTES,
    WGPEER_A_ALLOWEDIPS,
    WGPEER_A_PROTOCOL_VERSION,
    __WGPEER_A_LAST
};
#define WGPEER_A_MAX (__WGPEER_A_LAST - 1)

enum wgallowedip_attribute {
    WGALLOWEDIP_A_UNSPEC,
    WGALLOWEDIP_A_FAMILY,
    WGALLOWEDIP_A_IPADDR,
    WGALLOWEDIP_A_CIDR_MASK,
    __WGALLOWEDIP_A_LAST
};
#define WGALLOWEDIP_A_MAX (__WGALLOWEDIP_A_LAST - 1)

// if_link.h
enum {
    IFLA_UNSPEC,
    IFLA_ADDRESS,
    IFLA_BROADCAST,
    IFLA_IFNAME,
    IFLA_MTU,
    IFLA_LINK,
    IFLA_QDISC,
    IFLA_STATS,
    IFLA_COST,
#define IFLA_COST IFLA_COST
    IFLA_PRIORITY,
#define IFLA_PRIORITY IFLA_PRIORITY
    IFLA_MASTER,
#define IFLA_MASTER IFLA_MASTER
    IFLA_WIRELESS, /* Wireless Extension event - see wireless.h */
#define IFLA_WIRELESS IFLA_WIRELESS
    IFLA_PROTINFO, /* Protocol specific information for a link */
#define IFLA_PROTINFO IFLA_PROTINFO
    IFLA_TXQLEN,
#define IFLA_TXQLEN IFLA_TXQLEN
    IFLA_MAP,
#define IFLA_MAP IFLA_MAP
    IFLA_WEIGHT,
#define IFLA_WEIGHT IFLA_WEIGHT
    IFLA_OPERSTATE,
    IFLA_LINKMODE,
    IFLA_LINKINFO,
#define IFLA_LINKINFO IFLA_LINKINFO
    IFLA_NET_NS_PID,
    IFLA_IFALIAS,
    IFLA_NUM_VF, /* Number of VFs if device is SR-IOV PF */
    IFLA_VFINFO_LIST,
    IFLA_STATS64,
    IFLA_VF_PORTS,
    IFLA_PORT_SELF,
    IFLA_AF_SPEC,
    IFLA_GROUP, /* Group the device belongs to */
    IFLA_NET_NS_FD,
    IFLA_EXT_MASK, /* Extended info mask, VFs, etc */
    IFLA_PROMISCUITY, /* Promiscuity count: > 0 means acts PROMISC */
#define IFLA_PROMISCUITY IFLA_PROMISCUITY
    IFLA_NUM_TX_QUEUES,
    IFLA_NUM_RX_QUEUES,
    IFLA_CARRIER,
    IFLA_PHYS_PORT_ID,
    IFLA_CARRIER_CHANGES,
    IFLA_PHYS_SWITCH_ID,
    IFLA_LINK_NETNSID,
    IFLA_PHYS_PORT_NAME,
    IFLA_PROTO_DOWN,
    IFLA_GSO_MAX_SEGS,
    IFLA_GSO_MAX_SIZE,
    IFLA_PAD,
    IFLA_XDP,
    IFLA_EVENT,
    IFLA_NEW_NETNSID,
    IFLA_IF_NETNSID,
    IFLA_TARGET_NETNSID = IFLA_IF_NETNSID, /* new alias */
    IFLA_CARRIER_UP_COUNT,
    IFLA_CARRIER_DOWN_COUNT,
    IFLA_NEW_IFINDEX,
    IFLA_MIN_MTU,
    IFLA_MAX_MTU,
    IFLA_PROP_LIST,
    IFLA_ALT_IFNAME, /* Alternative ifname */
    IFLA_PERM_ADDRESS,
    IFLA_PROTO_DOWN_REASON,

    /* device (sysfs) name as parent, used instead
     * of IFLA_LINK where there's no parent netdev
     */
    IFLA_PARENT_DEV_NAME,
    IFLA_PARENT_DEV_BUS_NAME,
    IFLA_GRO_MAX_SIZE,

    __IFLA_MAX
};
#define IFLA_MAX (__IFLA_MAX - 1)

enum {
    IFLA_INFO_UNSPEC,
    IFLA_INFO_KIND,
    IFLA_INFO_DATA,
    IFLA_INFO_XSTATS,
    IFLA_INFO_SLAVE_KIND,
    IFLA_INFO_SLAVE_DATA,
    __IFLA_INFO_MAX,
};
#define IFLA_INFO_MAX (__IFLA_INFO_MAX - 1)

// gennetlink.h
#define GENL_NAMSIZ 16 /* length of family name */
#define GENL_MIN_ID NLMSG_MIN_TYPE
#define GENL_MAX_ID 1023

struct genlmsghdr {
    uint8_t cmd;
    uint8_t version;
    uint16_t reserved;
};

#define GENL_ADMIN_PERM 0x01
#define GENL_CMD_CAP_DO 0x02
#define GENL_CMD_CAP_DUMP 0x04
#define GENL_CMD_CAP_HASPOL 0x08
#define GENL_UNS_ADMIN_PERM 0x10

/*
 * List of reserved static generic netlink identifiers:
 */
#define GENL_ID_CTRL NLMSG_MIN_TYPE
#define GENL_ID_VFS_DQUOT (NLMSG_MIN_TYPE + 1)
#define GENL_ID_PMCRAID (NLMSG_MIN_TYPE + 2)
/* must be last reserved + 1 */
#define GENL_START_ALLOC (NLMSG_MIN_TYPE + 3)

enum {
    CTRL_CMD_UNSPEC,
    CTRL_CMD_NEWFAMILY,
    CTRL_CMD_DELFAMILY,
    CTRL_CMD_GETFAMILY,
    CTRL_CMD_NEWOPS,
    CTRL_CMD_DELOPS,
    CTRL_CMD_GETOPS,
    CTRL_CMD_NEWMCAST_GRP,
    CTRL_CMD_DELMCAST_GRP,
    CTRL_CMD_GETMCAST_GRP, /* unused */
    CTRL_CMD_GETPOLICY,
    __CTRL_CMD_MAX,
};
#define CTRL_CMD_MAX (__CTRL_CMD_MAX - 1)

enum {
    CTRL_ATTR_UNSPEC,
    CTRL_ATTR_FAMILY_ID,
    CTRL_ATTR_FAMILY_NAME,
    CTRL_ATTR_VERSION,
    CTRL_ATTR_HDRSIZE,
    CTRL_ATTR_MAXATTR,
    CTRL_ATTR_OPS,
    CTRL_ATTR_MCAST_GROUPS,
    CTRL_ATTR_POLICY,
    CTRL_ATTR_OP_POLICY,
    CTRL_ATTR_OP,
    __CTRL_ATTR_MAX,
};
#define CTRL_ATTR_MAX (__CTRL_ATTR_MAX - 1)

enum {
    CTRL_ATTR_OP_UNSPEC,
    CTRL_ATTR_OP_ID,
    CTRL_ATTR_OP_FLAGS,
    __CTRL_ATTR_OP_MAX,
};
#define CTRL_ATTR_OP_MAX (__CTRL_ATTR_OP_MAX - 1)

enum {
    CTRL_ATTR_MCAST_GRP_UNSPEC,
    CTRL_ATTR_MCAST_GRP_NAME,
    CTRL_ATTR_MCAST_GRP_ID,
    __CTRL_ATTR_MCAST_GRP_MAX,
};

enum {
    CTRL_ATTR_POLICY_UNSPEC,
    CTRL_ATTR_POLICY_DO,
    CTRL_ATTR_POLICY_DUMP,

    __CTRL_ATTR_POLICY_DUMP_MAX,
    CTRL_ATTR_POLICY_DUMP_MAX = __CTRL_ATTR_POLICY_DUMP_MAX - 1
};
#define CTRL_ATTR_MCAST_GRP_MAX (__CTRL_ATTR_MCAST_GRP_MAX - 1)

// acpi netlink
struct acpi_genl_event {
    char device_class[20];
    char bus_id[15];
    uint32_t type;
    uint32_t data;
};

enum {
    ACPI_GENL_ATTR_UNSPEC,
    ACPI_GENL_ATTR_EVENT,
    __ACPI_GENL_ATTR_MAX,
};
#define ACPI_GENL_ATTR_MAX (__ACPI_GENL_ATTR_MAX - 1)

enum {
    ACPI_GENL_CMD_UNSPEC,
    ACPI_GENL_CMD_EVENT,
    __ACPI_GENL_CMD_MAX,
};
#define ACPI_GENL_CMD_MAX (__ACPI_GENL_CMD_MAX - 1)
#define GENL_MAX_FAM_OPS 256
#define GENL_MAX_FAM_GRPS 256

#define ACPI_EVENT_FAMILY_NAME "acpi_event"
#define ACPI_EVENT_MCAST_GROUP_NAME "acpi_mc_group"

// mount.h
#define MS_RDONLY 1 /* Mount read-only */
#define MS_NOSUID 2 /* Ignore suid and sgid bits */
#define MS_NODEV 4 /* Disallow access to device special files */
#define MS_NOEXEC 8 /* Disallow program execution */
#define MS_SYNCHRONOUS 16 /* Writes are synced at once */
#define MS_REMOUNT 32 /* Alter flags of a mounted FS */
#define MS_MANDLOCK 64 /* Allow mandatory locks on an FS */
#define MS_DIRSYNC 128 /* Directory modifications are synchronous */
#define MS_NOSYMFOLLOW 256 /* Do not follow symlinks */
#define MS_NOATIME 1024 /* Do not update access times. */
#define MS_NODIRATIME 2048 /* Do not update directory access times */
#define MS_BIND 4096
#define MS_MOVE 8192
#define MS_REC 16384
#define MS_VERBOSE 32768 /* War is peace. Verbosity is silence. MS_VERBOSE is deprecated. */
#define MS_SILENT 32768
#define MS_POSIXACL (1<<16) /* VFS does not apply the umask */
#define MS_UNBINDABLE (1<<17) /* change to unbindable */
#define MS_PRIVATE (1<<18) /* change to private */
#define MS_SLAVE (1<<19) /* change to slave */
#define MS_SHARED (1<<20) /* change to shared */
#define MS_RELATIME (1<<21) /* Update atime relative to mtime/ctime. */
#define MS_KERNMOUNT (1<<22) /* this is a kern_mount call */
#define MS_I_VERSION (1<<23) /* Update inode I_version field */
#define MS_STRICTATIME (1<<24) /* Always perform atime updates */
#define MS_LAZYTIME (1<<25) /* Update the on-disk [acm]times lazily */

// dirent.h
struct linux_dirent64 {
    uint64_t d_ino; /* 64-bit inode number */
    int64_t d_off; /* 64-bit offset to next structure */
    uint16_t d_reclen; /* Size of this dirent */
    uint8_t d_type; /* File type */
    char d_name[5]; /* Filename (null-terminated) */
};

// fs.h
#define SEEK_SET 0 /* seek relative to beginning of file */
#define SEEK_CUR 1 /* seek relative to current file position */
#define SEEK_END 2 /* seek relative to end of file */
#define SEEK_DATA 3 /* seek to the next data */
#define SEEK_HOLE 4 /* seek to the next hole */
#define SEEK_MAX SEEK_HOLE

#define RENAME_NOREPLACE (1 << 0) /* Don't overwrite target */
#define RENAME_EXCHANGE (1 << 1) /* Exchange source and dest */
#define RENAME_WHITEOUT (1 << 2) /* Whiteout source */

#define BLKROSET _IO(0x12,93) /* set device read-only (0 = read-write) */
#define BLKROGET _IO(0x12,94) /* get read-only status (0 = read_write) */
#define BLKRRPART _IO(0x12,95) /* re-read partition table */
#define BLKGETSIZE _IO(0x12,96) /* return device size /512 (long *arg) */
#define BLKFLSBUF _IO(0x12,97) /* flush buffer cache */
#define BLKRASET _IO(0x12,98) /* set read ahead for block device */
#define BLKRAGET _IO(0x12,99) /* get current read ahead setting */
#define BLKFRASET _IO(0x12,100) /* set filesystem (mm/filemap.c) read-ahead */
#define BLKFRAGET _IO(0x12,101) /* get filesystem (mm/filemap.c) read-ahead */
#define BLKSECTSET _IO(0x12,102) /* set max sectors per request (ll_rw_blk.c) */
#define BLKSECTGET _IO(0x12,103) /* get max sectors per request (ll_rw_blk.c) */
#define BLKSSZGET _IO(0x12,104) /* get block device sector size */
#define BLKBSZGET _IOR(0x12,112,size_t)
#define BLKBSZSET _IOW(0x12,113,size_t)
#define BLKGETSIZE64 _IOR(0x12,114,size_t) /* return device size in bytes (u64 *arg) */
#define BLKTRACESETUP _IOWR(0x12,115,struct blk_user_trace_setup)
#define BLKTRACESTART _IO(0x12,116)
#define BLKTRACESTOP _IO(0x12,117)
#define BLKTRACETEARDOWN _IO(0x12,118)
#define BLKDISCARD _IO(0x12,119)
#define BLKIOMIN _IO(0x12,120)
#define BLKIOOPT _IO(0x12,121)
#define BLKALIGNOFF _IO(0x12,122)
#define BLKPBSZGET _IO(0x12,123)
#define BLKDISCARDZEROES _IO(0x12,124)
#define BLKSECDISCARD _IO(0x12,125)
#define BLKROTATIONAL _IO(0x12,126)
#define BLKZEROOUT _IO(0x12,127)
#define BLKGETDISKSEQ _IOR(0x12,128,uint64_t)

// sockios.h
#define SIOCINQ FIONREAD
#define SIOCOUTQ TIOCOUTQ /* output queue size (not sent + not acked) */

#define SOCK_IOC_TYPE 0x89

/*
 * the timeval/timespec data structure layout is defined by libc,
 * so we need to cover both possible versions on 32-bit.
 */
/* Get stamp (timeval) */
#define SIOCGSTAMP_NEW _IOR(SOCK_IOC_TYPE, 0x06, long long[2])
/* Get stamp (timespec) */
#define SIOCGSTAMPNS_NEW _IOR(SOCK_IOC_TYPE, 0x07, long long[2])

#define SIOCGSTAMP SIOCGSTAMP_NEW
#define SIOCGSTAMPNS SIOCGSTAMPNS_NEW

/* Routing table calls. */
#define SIOCADDRT 0x890B /* add routing table entry */
#define SIOCDELRT 0x890C /* delete routing table entry */
#define SIOCRTMSG 0x890D /* unused */

/* Socket configuration controls. */
#define SIOCGIFNAME 0x8910 /* get iface name */
#define SIOCSIFLINK 0x8911 /* set iface channel */
#define SIOCGIFCONF 0x8912 /* get iface list */
#define SIOCGIFFLAGS 0x8913 /* get flags */
#define SIOCSIFFLAGS 0x8914 /* set flags */
#define SIOCGIFADDR 0x8915 /* get PA address */
#define SIOCSIFADDR 0x8916 /* set PA address */
#define SIOCGIFDSTADDR 0x8917 /* get remote PA address */
#define SIOCSIFDSTADDR 0x8918 /* set remote PA address */
#define SIOCGIFBRDADDR 0x8919 /* get broadcast PA address */
#define SIOCSIFBRDADDR 0x891a /* set broadcast PA address */
#define SIOCGIFNETMASK 0x891b /* get network PA mask */
#define SIOCSIFNETMASK 0x891c /* set network PA mask */
#define SIOCGIFMETRIC 0x891d /* get metric */
#define SIOCSIFMETRIC 0x891e /* set metric */
#define SIOCGIFMEM 0x891f /* get memory address (BSD) */
#define SIOCSIFMEM 0x8920 /* set memory address (BSD) */
#define SIOCGIFMTU 0x8921 /* get MTU size */
#define SIOCSIFMTU 0x8922 /* set MTU size */
#define SIOCSIFNAME 0x8923 /* set interface name */
#define SIOCSIFHWADDR 0x8924 /* set hardware address */
#define SIOCGIFENCAP 0x8925 /* get/set encapsulations */
#define SIOCSIFENCAP 0x8926
#define SIOCGIFHWADDR 0x8927 /* Get hardware address */
#define SIOCGIFSLAVE 0x8929 /* Driver slaving support */
#define SIOCSIFSLAVE 0x8930
#define SIOCADDMULTI 0x8931 /* Multicast address lists */
#define SIOCDELMULTI 0x8932
#define SIOCGIFINDEX 0x8933 /* name -> if_index mapping */
#define SIOGIFINDEX SIOCGIFINDEX /* misprint compatibility :-) */
#define SIOCSIFPFLAGS 0x8934 /* set/get extended flags set */
#define SIOCGIFPFLAGS 0x8935
#define SIOCDIFADDR 0x8936 /* delete PA address */
#define SIOCSIFHWBROADCAST 0x8937 /* set hardware broadcast addr */
#define SIOCGIFCOUNT 0x8938 /* get number of devices */

#define SIOCGIFBR 0x8940 /* Bridging support */
#define SIOCSIFBR 0x8941 /* Set bridging options */

#define SIOCGIFTXQLEN 0x8942 /* Get the tx queue length */
#define SIOCSIFTXQLEN 0x8943 /* Set the tx queue length */

#define SIOCETHTOOL 0x8946 /* Ethtool interface */

#define SIOCGMIIPHY 0x8947 /* Get address of MII PHY in use. */
#define SIOCGMIIREG 0x8948 /* Read MII PHY register. */
#define SIOCSMIIREG 0x8949 /* Write MII PHY register. */
#define SIOCWANDEV 0x894A /* get/set netdev parameters */
#define SIOCOUTQNSD 0x894B /* output queue size (not sent only) */
#define SIOCGSKNS 0x894C /* get socket network namespace */

/* ARP cache control calls. */
#define SIOCDARP 0x8953 /* delete ARP table entry */
#define SIOCGARP 0x8954 /* get ARP table entry */
#define SIOCSARP 0x8955 /* set ARP table entry */

/* RARP cache control calls. */
#define SIOCDRARP 0x8960 /* delete RARP table entry */
#define SIOCGRARP 0x8961 /* get RARP table entry */
#define SIOCSRARP 0x8962 /* set RARP table entry */

/* Driver configuration calls */

#define SIOCGIFMAP 0x8970 /* Get device parameters */
#define SIOCSIFMAP 0x8971 /* Set device parameters */

/* DLCI configuration calls */

#define SIOCADDDLCI 0x8980 /* Create new DLCI device */
#define SIOCDELDLCI 0x8981 /* Delete DLCI device */

#define SIOCGIFVLAN 0x8982 /* 802.1Q VLAN support */
#define SIOCSIFVLAN 0x8983 /* Set 802.1Q VLAN options */

/* bonding calls */
#define SIOCBONDENSLAVE 0x8990 /* enslave a device to the bond */
#define SIOCBONDRELEASE 0x8991 /* release a slave from the bond*/
#define SIOCBONDSETHWADDR      0x8992 /* set the hw addr of the bond */
#define SIOCBONDSLAVEINFOQUERY 0x8993   /* rtn info about slave state   */
#define SIOCBONDINFOQUERY      0x8994 /* rtn info about bond state    */
#define SIOCBONDCHANGEACTIVE   0x8995   /* update to a new active slave */

/* bridge calls */
#define SIOCBRADDBR 0x89a0 /* create new bridge device     */
#define SIOCBRDELBR 0x89a1 /* remove bridge device         */
#define SIOCBRADDIF 0x89a2 /* add interface to bridge      */
#define SIOCBRDELIF 0x89a3 /* remove interface from bridge */

/* hardware time stamping: parameters in linux/net_tstamp.h */
#define SIOCSHWTSTAMP 0x89b0 /* set and get config */
#define SIOCGHWTSTAMP 0x89b1 /* get config */

// netfilter.h
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_STOP 5 /* Deprecated, for userspace nf_queue compatibility. */
#define NF_MAX_VERDICT NF_STOP

/* we overload the higher bits for encoding auxiliary data such as the queue
 * number or errno values. Not nice, but better than additional function
 * arguments. */
#define NF_VERDICT_MASK 0x000000ff

/* extra verdict flags have mask 0x0000ff00 */
#define NF_VERDICT_FLAG_QUEUE_BYPASS 0x00008000

/* queue number (NF_QUEUE) or errno (NF_DROP) */
#define NF_VERDICT_QMASK 0xffff0000
#define NF_VERDICT_QBITS 16

#define NF_QUEUE_NR(x) ((((x) << 16) & NF_VERDICT_QMASK) | NF_QUEUE)
#define NF_DROP_ERR(x) (((-x) << 16) | NF_DROP)

enum nf_inet_hooks {
    NF_INET_PRE_ROUTING,
    NF_INET_LOCAL_IN,
    NF_INET_FORWARD,
    NF_INET_LOCAL_OUT,
    NF_INET_POST_ROUTING,
    NF_INET_NUMHOOKS,
    NF_INET_INGRESS = NF_INET_NUMHOOKS,
};

enum nf_dev_hooks {
    NF_NETDEV_INGRESS,
    NF_NETDEV_EGRESS,
    NF_NETDEV_NUMHOOKS
};

enum {
    NFPROTO_UNSPEC = 0,
    NFPROTO_INET = 1,
    NFPROTO_IPV4 = 2,
    NFPROTO_ARP = 3,
    NFPROTO_NETDEV = 5,
    NFPROTO_BRIDGE = 7,
    NFPROTO_IPV6 = 10,
    NFPROTO_DECNET = 12,
    NFPROTO_NUMPROTO,
};

// x_tables.h
#define XT_FUNCTION_MAXNAMELEN 30
#define XT_EXTENSION_MAXNAMELEN 29
#define XT_TABLE_MAXNAMELEN 32

struct xt_entry_match {
    uint16_t match_size;
    char name[XT_EXTENSION_MAXNAMELEN];
    uint8_t revision;
} hc_ALIGNED(8);

struct xt_entry_target {
    uint16_t target_size;
    char name[XT_EXTENSION_MAXNAMELEN];
    uint8_t revision;
} hc_ALIGNED(8);

struct xt_standard_target {
    struct xt_entry_target target;
    int32_t verdict;
    int32_t __pad;
};

/* CONTINUE verdict for targets */
#define XT_CONTINUE 0xFFFFFFFF

/* For standard target */
#define XT_RETURN (-NF_REPEAT - 1)

/* Standard return verdict, or do jump. */
#define XT_STANDARD_TARGET ""

struct xt_counters {
    uint64_t pcnt, bcnt; /* Packet and byte counters */
};

#define XT_INV_PROTO 0x40 /* Invert the sense of PROTO. */

// nf_nat.h
struct nf_nat_ipv4_range {
    uint32_t flags;
    uint8_t min_ip[4];
    uint8_t max_ip[4];
    uint16_t min;
    uint16_t max;
};

struct nf_nat_ipv4_multi_range_compat {
    uint32_t rangesize;
    struct nf_nat_ipv4_range range[1];
};

// ip_tables.h
struct ipt_ip {
    /* Source and destination IP addr */
    uint8_t src[4], dst[4];
    /* Mask for src and dest IP addr */
    uint8_t smsk[4], dmsk[4];
    char iniface[IFNAMSIZ], outiface[IFNAMSIZ];
    uint8_t iniface_mask[IFNAMSIZ], outiface_mask[IFNAMSIZ];

    /* Protocol, 0 = ANY */
    uint16_t proto;

    /* Flags word */
    uint8_t flags;
    /* Inverse flags */
    uint8_t invflags;
};

/* Values for "flag" field in struct ipt_ip (general ip structure). */
#define IPT_F_FRAG 0x01 /* Set if rule is a fragment rule */
#define IPT_F_GOTO 0x02 /* Set if jump is a goto */
#define IPT_F_MASK 0x03 /* All possible flag bits mask. */

/* Values for "inv" field in struct ipt_ip. */
#define IPT_INV_VIA_IN 0x01 /* Invert the sense of IN IFACE. */
#define IPT_INV_VIA_OUT 0x02 /* Invert the sense of OUT IFACE */
#define IPT_INV_TOS 0x04 /* Invert the sense of TOS. */
#define IPT_INV_SRCIP 0x08 /* Invert the sense of SRC IP. */
#define IPT_INV_DSTIP 0x10 /* Invert the sense of DST OP. */
#define IPT_INV_FRAG 0x20 /* Invert the sense of FRAG. */
#define IPT_INV_PROTO XT_INV_PROTO
#define IPT_INV_MASK 0x7F /* All possible flag bits mask. */

/* This structure defines each of the firewall rules. Consists of 3
   parts which are 1) general IP header stuff 2) match specific
   stuff 3) the target to perform if the rule matches */
struct ipt_entry {
    struct ipt_ip ip;

    /* Mark with fields that we care about. */
    uint32_t nfcache;

    /* Size of ipt_entry + matches */
    uint16_t target_offset;
    /* Size of ipt_entry + matches + target */
    uint16_t next_offset;

    /* Back pointer */
    uint32_t comefrom;

    /* Packet and byte counters. */
    struct xt_counters counters;

    /* The matches (if any), then the target. */
    // uint8_t elems[];
};

#define IPT_BASE_CTL 64

#define IPT_SO_SET_REPLACE (IPT_BASE_CTL)
#define IPT_SO_SET_ADD_COUNTERS (IPT_BASE_CTL + 1)

#define IPT_SO_GET_INFO (IPT_BASE_CTL)
#define IPT_SO_GET_ENTRIES (IPT_BASE_CTL + 1)
#define IPT_SO_GET_REVISION_MATCH (IPT_BASE_CTL + 2)
#define IPT_SO_GET_REVISION_TARGET (IPT_BASE_CTL + 3)

/* ICMP matching stuff */
struct ipt_icmp {
    uint8_t type; /* type to match */
    uint8_t code[2]; /* range of code */
    uint8_t invflags; /* Inverse flags */
};

/* Values for "inv" field for struct ipt_icmp. */
#define IPT_ICMP_INV 0x01 /* Invert the sense of type/code test */

/* The argument to IPT_SO_GET_INFO */
struct ipt_getinfo {
    /* Which table: caller fills this in. */
    char name[XT_TABLE_MAXNAMELEN];

    /* Kernel fills these in. */
    /* Which hook entry points are valid: bitmask */
    uint32_t valid_hooks;

    /* Hook entry points: one per netfilter hook. */
    uint32_t hook_entry[NF_INET_NUMHOOKS];

    /* Underflow points. */
    uint32_t underflow[NF_INET_NUMHOOKS];

    /* Number of entries */
    uint32_t num_entries;

    /* Size of entries. */
    uint32_t size;
};

/* The argument to IPT_SO_SET_REPLACE. */
struct ipt_replace {
    /* Which table. */
    char name[XT_TABLE_MAXNAMELEN];

    /* Which hook entry points are valid: bitmask. You can't change this. */
    uint32_t valid_hooks;

    /* Number of entries */
    uint32_t num_entries;

    /* Total size of new entries */
    uint32_t size;

    /* Hook entry points. */
    uint32_t hook_entry[NF_INET_NUMHOOKS];

    /* Underflow points. */
    uint32_t underflow[NF_INET_NUMHOOKS];

    /* Information about old entries: */
    /* Number of counters (must be equal to current number of entries). */
    uint32_t num_counters;
    /* The old entries' counters. */
    struct xt_counters *counters;

    /* The entries (hang off end: not really an array). */
    // uint8_t entries[];
};

/* The argument to IPT_SO_GET_ENTRIES. */
struct ipt_get_entries {
    /* Which table: user fills this in. */
    char name[XT_TABLE_MAXNAMELEN];

    /* User fills this in: total entry size. */
    uint32_t size;

    /* The entries. */
    // uint8_t entries[];
};

// if_ether.h
#define ETH_P_ALL 0x0003
#define ETH_P_IP 0x0800

// if_packet.h
struct sockaddr_ll {
    uint16_t sll_family;
    uint16_t sll_protocol;
    int32_t sll_ifindex;
    uint16_t sll_hatype;
    uint8_t sll_pkttype;
    uint8_t sll_halen;
    uint8_t sll_addr[8];
};

/* Packet types */
#define PACKET_HOST 0 /* To us */
#define PACKET_BROADCAST 1 /* To all */
#define PACKET_MULTICAST 2 /* To group */
#define PACKET_OTHERHOST 3 /* To someone else */
#define PACKET_OUTGOING 4 /* Outgoing of any type */
#define PACKET_LOOPBACK 5 /* MC/BRD frame looped back */
#define PACKET_USER 6 /* To user space */
#define PACKET_KERNEL 7 /* To kernel space */

// inotify.h
/*
 * struct inotify_event - structure read from the inotify device for each event
 *
 * When you are watching a directory, you will receive the filename for events
 * such as IN_CREATE, IN_DELETE, IN_OPEN, IN_CLOSE, ..., relative to the wd.
 */
struct inotify_event {
    int32_t wd; /* watch descriptor */
    uint32_t mask; /* watch mask */
    uint32_t cookie; /* cookie to synchronize two events */
    uint32_t len; /* length (including nulls) of name */
    // char name[]; /* stub for possible name */
};

/* the following are legal, implemented events that user-space can watch for */
#define IN_ACCESS 0x00000001 /* File was accessed */
#define IN_MODIFY 0x00000002 /* File was modified */
#define IN_ATTRIB 0x00000004 /* Metadata changed */
#define IN_CLOSE_WRITE 0x00000008 /* Writtable file was closed */
#define IN_CLOSE_NOWRITE 0x00000010 /* Unwrittable file closed */
#define IN_OPEN 0x00000020 /* File was opened */
#define IN_MOVED_FROM 0x00000040 /* File was moved from X */
#define IN_MOVED_TO 0x00000080 /* File was moved to Y */
#define IN_CREATE 0x00000100 /* Subfile was created */
#define IN_DELETE 0x00000200 /* Subfile was deleted */
#define IN_DELETE_SELF 0x00000400 /* Self was deleted */
#define IN_MOVE_SELF 0x00000800 /* Self was moved */

/* the following are legal events. they are sent as needed to any watch */
#define IN_UNMOUNT 0x00002000 /* Backing fs was unmounted */
#define IN_Q_OVERFLOW 0x00004000 /* Event queued overflowed */
#define IN_IGNORED 0x00008000 /* File was ignored */

/* helper events */
#define IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* close */
#define IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO) /* moves */

/* special flags */
#define IN_ONLYDIR 0x01000000 /* only watch the path if it is a directory */
#define IN_DONT_FOLLOW 0x02000000 /* don't follow a sym link */
#define IN_EXCL_UNLINK 0x04000000 /* exclude events on unlinked objects */
#define IN_MASK_CREATE 0x10000000 /* only create watches */
#define IN_MASK_ADD 0x20000000 /* add to the mask of an already existing watch */
#define IN_ISDIR 0x40000000 /* event occurred against dir */
#define IN_ONESHOT 0x80000000 /* only send event once */

#define IN_ALL_EVENTS (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE | IN_CREATE | IN_DELETE_SELF | IN_MOVE_SELF)

/* Flags for sys_inotify_init1. */
#define IN_CLOEXEC O_CLOEXEC
#define IN_NONBLOCK O_NONBLOCK

// ksmbd_netlink.h
#define KSMBD_GENL_NAME "SMBD_GENL"
#define KSMBD_GENL_VERSION 0x01

#define KSMBD_REQ_MAX_ACCOUNT_NAME_SZ 48
#define KSMBD_REQ_MAX_HASH_SZ 18
#define KSMBD_REQ_MAX_SHARE_NAME 64

/*
 * IPC heartbeat frame to check whether user IPC daemon is alive.
 */
struct ksmbd_heartbeat {
    uint32_t handle;
};

/*
 * Global config flags.
 */
#define KSMBD_GLOBAL_FLAG_INVALID (0)
#define KSMBD_GLOBAL_FLAG_SMB2_LEASES (1u << 0)
#define KSMBD_GLOBAL_FLAG_SMB2_ENCRYPTION (1u << 1)
#define KSMBD_GLOBAL_FLAG_SMB3_MULTICHANNEL (1u << 2)
#define KSMBD_GLOBAL_FLAG_SMB2_ENCRYPTION_OFF (1u << 3)

/*
 * IPC request for ksmbd server startup
 */
struct ksmbd_startup_request {
    uint32_t flags; /* Flags for global config */
    int32_t signing; /* Signing enabled */
    char min_prot[16]; /* The minimum SMB protocol version */
    char max_prot[16]; /* The maximum SMB protocol version */
    char netbios_name[16];
    char work_group[64]; /* Workgroup */
    char server_string[64]; /* Server string */
    uint16_t tcp_port; /* tcp port */
    uint16_t ipc_timeout; /*
                           * specifies the number of seconds
                           * server will wait for the userspace to
                           * reply to heartbeat frames.
                           */
    uint32_t deadtime; /* Number of minutes of inactivity */
    uint32_t file_max; /* Limits the maximum number of open files */
    uint32_t smb2_max_write; /* MAX write size */
    uint32_t smb2_max_read; /* MAX read size */
    uint32_t smb2_max_trans; /* MAX trans size */
    uint32_t share_fake_fscaps; /*
                                 * Support some special application that
                                 * makes QFSINFO calls to check whether
                                 * we set the SPARSE_FILES bit (0x40).
                                 */
    uint32_t sub_auth[3]; /* Subauth value for Security ID */
    uint32_t smb2_max_credits; /* MAX credits */
    uint32_t smbd_max_io_size; /* smbd read write size */
    uint32_t max_connections; /* Number of maximum simultaneous connections */
    uint32_t reserved[126]; /* Reserved room */
    uint32_t ifc_list_sz; /* interfaces list size */
    // char payload[];
};

/*
 * IPC request to shutdown ksmbd server.
 */
struct ksmbd_shutdown_request {
    int32_t reserved[16];
};

/*
 * IPC user login request.
 */
struct ksmbd_login_request {
    uint32_t handle;
    char account[KSMBD_REQ_MAX_ACCOUNT_NAME_SZ]; /* user account name */
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC user login response.
 */
struct ksmbd_login_response {
    uint32_t handle;
    uint32_t gid; /* group id */
    uint32_t uid; /* user id */
    char account[KSMBD_REQ_MAX_ACCOUNT_NAME_SZ]; /* user account name */
    uint16_t status;
    uint16_t hash_sz; /* hash size */
    uint8_t hash[KSMBD_REQ_MAX_HASH_SZ]; /* password hash */
    int16_t __pad;
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC request to fetch net share config.
 */
struct ksmbd_share_config_request {
    uint32_t handle;
    char share_name[KSMBD_REQ_MAX_SHARE_NAME]; /* share name */
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC response to the net share config request.
 */
struct ksmbd_share_config_response {
    uint32_t handle;
    uint32_t flags;
    uint16_t create_mask;
    uint16_t directory_mask;
    uint16_t force_create_mode;
    uint16_t force_directory_mode;
    uint16_t force_uid;
    uint16_t force_gid;
    char share_name[KSMBD_REQ_MAX_SHARE_NAME];
    uint32_t reserved[112]; /* Reserved room */
    uint32_t veto_list_sz;
    // char payload[];
};

/*
 * IPC request for tree connection. This request include session and tree
 * connect info from client.
 */
struct ksmbd_tree_connect_request {
    uint32_t handle;
    uint16_t account_flags;
    uint16_t flags;
    uint64_t session_id;
    uint64_t connect_id;
    char account[KSMBD_REQ_MAX_ACCOUNT_NAME_SZ];
    char share[KSMBD_REQ_MAX_SHARE_NAME];
    char peer_addr[64];
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC Response structure for tree connection.
 */
struct ksmbd_tree_connect_response {
    uint32_t handle;
    uint16_t status;
    uint16_t connection_flags;
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC Request struture to disconnect tree connection.
 */
struct ksmbd_tree_disconnect_request {
    uint64_t session_id; /* session id */
    uint64_t connect_id; /* tree connection id */
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * IPC Response structure to logout user account.
 */
struct ksmbd_logout_request {
    char account[KSMBD_REQ_MAX_ACCOUNT_NAME_SZ]; /* user account name */
    uint32_t account_flags;
    uint32_t reserved[16]; /* Reserved room */
};

/*
 * This also used as NETLINK attribute type value.
 *
 * NOTE:
 * Response message type value should be equal to
 * request message type value + 1.
 */
enum ksmbd_event {
    KSMBD_EVENT_UNSPEC = 0,
    KSMBD_EVENT_HEARTBEAT_REQUEST,

    KSMBD_EVENT_STARTING_UP,
    KSMBD_EVENT_SHUTTING_DOWN,

    KSMBD_EVENT_LOGIN_REQUEST,
    KSMBD_EVENT_LOGIN_RESPONSE = 5,

    KSMBD_EVENT_SHARE_CONFIG_REQUEST,
    KSMBD_EVENT_SHARE_CONFIG_RESPONSE,

    KSMBD_EVENT_TREE_CONNECT_REQUEST,
    KSMBD_EVENT_TREE_CONNECT_RESPONSE,

    KSMBD_EVENT_TREE_DISCONNECT_REQUEST = 10,

    KSMBD_EVENT_LOGOUT_REQUEST,

    KSMBD_EVENT_RPC_REQUEST,
    KSMBD_EVENT_RPC_RESPONSE,

    KSMBD_EVENT_SPNEGO_AUTHEN_REQUEST,
    KSMBD_EVENT_SPNEGO_AUTHEN_RESPONSE = 15,

    KSMBD_EVENT_MAX
};

/*
 * Enumeration for IPC tree connect status.
 */
enum KSMBD_TREE_CONN_STATUS {
    KSMBD_TREE_CONN_STATUS_OK = 0,
    KSMBD_TREE_CONN_STATUS_NOMEM,
    KSMBD_TREE_CONN_STATUS_NO_SHARE,
    KSMBD_TREE_CONN_STATUS_NO_USER,
    KSMBD_TREE_CONN_STATUS_INVALID_USER,
    KSMBD_TREE_CONN_STATUS_HOST_DENIED = 5,
    KSMBD_TREE_CONN_STATUS_CONN_EXIST,
    KSMBD_TREE_CONN_STATUS_TOO_MANY_CONNS,
    KSMBD_TREE_CONN_STATUS_TOO_MANY_SESSIONS,
    KSMBD_TREE_CONN_STATUS_ERROR,
};

/*
 * User config flags.
 */
#define KSMBD_USER_FLAG_INVALID (0)
#define KSMBD_USER_FLAG_OK (1u << 0)
#define KSMBD_USER_FLAG_BAD_PASSWORD (1u << 1)
#define KSMBD_USER_FLAG_BAD_UID (1u << 2)
#define KSMBD_USER_FLAG_BAD_USER (1u << 3)
#define KSMBD_USER_FLAG_GUEST_ACCOUNT (1u << 4)
#define KSMBD_USER_FLAG_DELAY_SESSION (1u << 5)

/*
 * Share config flags.
 */
#define KSMBD_SHARE_FLAG_INVALID (0)
#define KSMBD_SHARE_FLAG_AVAILABLE (1u << 0)
#define KSMBD_SHARE_FLAG_BROWSEABLE (1u << 1)
#define KSMBD_SHARE_FLAG_WRITEABLE (1u << 2)
#define KSMBD_SHARE_FLAG_READONLY (1u << 3)
#define KSMBD_SHARE_FLAG_GUEST_OK (1u << 4)
#define KSMBD_SHARE_FLAG_GUEST_ONLY (1u << 5)
#define KSMBD_SHARE_FLAG_STORE_DOS_ATTRS (1u << 6)
#define KSMBD_SHARE_FLAG_OPLOCKS (1u << 7)
#define KSMBD_SHARE_FLAG_PIPE (1u << 8)
#define KSMBD_SHARE_FLAG_HIDE_DOT_FILES (1u << 9)
#define KSMBD_SHARE_FLAG_INHERIT_OWNER (1u << 10)
#define KSMBD_SHARE_FLAG_STREAMS (1u << 11)
#define KSMBD_SHARE_FLAG_FOLLOW_SYMLINKS (1u << 12)
#define KSMBD_SHARE_FLAG_ACL_XATTR (1u << 13)
#define KSMBD_SHARE_FLAG_UPDATE (1u << 14)
#define KSMBD_SHARE_FLAG_CROSSMNT (1u << 15)

/*
 * Tree connect request flags.
 */
#define KSMBD_TREE_CONN_FLAG_REQUEST_SMB1 (0)
#define KSMBD_TREE_CONN_FLAG_REQUEST_IPV6 (1u << 0)
#define KSMBD_TREE_CONN_FLAG_REQUEST_SMB2 (1u << 1)

/*
 * Tree connect flags.
 */
#define KSMBD_TREE_CONN_FLAG_GUEST_ACCOUNT (1u << 0)
#define KSMBD_TREE_CONN_FLAG_READ_ONLY (1u << 1)
#define KSMBD_TREE_CONN_FLAG_WRITABLE (1u << 2)
#define KSMBD_TREE_CONN_FLAG_ADMIN_ACCOUNT (1u << 3)
#define KSMBD_TREE_CONN_FLAG_UPDATE (1u << 4)

/*
 * RPC over IPC.
 */
#define KSMBD_RPC_METHOD_RETURN (1u << 0)
#define KSMBD_RPC_SRVSVC_METHOD_INVOKE (1u << 1)
#define KSMBD_RPC_SRVSVC_METHOD_RETURN (KSMBD_RPC_SRVSVC_METHOD_INVOKE | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_WKSSVC_METHOD_INVOKE (1u << 2)
#define KSMBD_RPC_WKSSVC_METHOD_RETURN (KSMBD_RPC_WKSSVC_METHOD_INVOKE | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_IOCTL_METHOD ((1u << 3) | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_OPEN_METHOD (1u << 4)
#define KSMBD_RPC_WRITE_METHOD (1u << 5)
#define KSMBD_RPC_READ_METHOD ((1u << 6) | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_CLOSE_METHOD (1u << 7)
#define KSMBD_RPC_RAP_METHOD ((1u << 8) | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_RESTRICTED_CONTEXT (1u << 9)
#define KSMBD_RPC_SAMR_METHOD_INVOKE (1u << 10)
#define KSMBD_RPC_SAMR_METHOD_RETURN (KSMBD_RPC_SAMR_METHOD_INVOKE | KSMBD_RPC_METHOD_RETURN)
#define KSMBD_RPC_LSARPC_METHOD_INVOKE (1u << 11)
#define KSMBD_RPC_LSARPC_METHOD_RETURN (KSMBD_RPC_LSARPC_METHOD_INVOKE | KSMBD_RPC_METHOD_RETURN)

/*
 * RPC status definitions.
 */
#define KSMBD_RPC_OK 0
#define KSMBD_RPC_EBAD_FUNC 0x00000001
#define KSMBD_RPC_EACCESS_DENIED 0x00000005
#define KSMBD_RPC_EBAD_FID 0x00000006
#define KSMBD_RPC_ENOMEM 0x00000008
#define KSMBD_RPC_EBAD_DATA 0x0000000D
#define KSMBD_RPC_ENOTIMPLEMENTED 0x00000040
#define KSMBD_RPC_EINVALID_PARAMETER 0x00000057
#define KSMBD_RPC_EMORE_DATA 0x000000EA
#define KSMBD_RPC_EINVALID_LEVEL 0x0000007C
#define KSMBD_RPC_SOME_NOT_MAPPED 0x00000107

#define KSMBD_CONFIG_OPT_DISABLED 0
#define KSMBD_CONFIG_OPT_ENABLED 1
#define KSMBD_CONFIG_OPT_AUTO 2
#define KSMBD_CONFIG_OPT_MANDATORY 3

// memfd.h
#define MFD_CLOEXEC 0x0001U
#define MFD_ALLOW_SEALING 0x0002U
#define MFD_HUGETLB 0x0004U

// close_range.h
/* Unshare the file descriptor table before closing file descriptors. */
#define CLOSE_RANGE_UNSHARE (1U << 1)

/* Set the FD_CLOEXEC bit instead of closing the file descriptor. */
#define CLOSE_RANGE_CLOEXEC (1U << 2)

// filter.h
struct sock_filter {
    uint16_t code;
    uint8_t jt;
    uint8_t jf;
    uint32_t k;
};

struct sock_fprog {
    uint16_t len;
    char __pad[6];
    struct sock_filter *filter;
};
