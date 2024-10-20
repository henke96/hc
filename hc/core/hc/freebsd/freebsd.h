// errno.h
#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EDEADLK 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34
#define EAGAIN 35
#define EWOULDBLOCK EAGAIN
#define EINPROGRESS 36
#define EALREADY 37
#define ENOTSOCK 38
#define EDESTADDRREQ 39
#define EMSGSIZE 40
#define EPROTOTYPE 41
#define ENOPROTOOPT 42
#define EPROTONOSUPPORT 43
#define ESOCKTNOSUPPORT 44
#define EOPNOTSUPP 45
#define ENOTSUP EOPNOTSUPP
#define EPFNOSUPPORT 46
#define EAFNOSUPPORT 47
#define EADDRINUSE 48
#define EADDRNOTAVAIL 49
#define ENETDOWN 50
#define ENETUNREACH 51
#define ENETRESET 52
#define ECONNABORTED 53
#define ECONNRESET 54
#define ENOBUFS 55
#define EISCONN 56
#define ENOTCONN 57
#define ESHUTDOWN 58
#define ETOOMANYREFS 59
#define ETIMEDOUT 60
#define ECONNREFUSED 61
#define ELOOP 62
#define ENAMETOOLONG 63
#define EHOSTDOWN 64
#define EHOSTUNREACH 65
#define ENOTEMPTY 66
#define EPROCLIM 67
#define EUSERS 68
#define EDQUOT 69
#define ESTALE 70
#define EREMOTE 71
#define EBADRPC 72
#define ERPCMISMATCH 73
#define EPROGUNAVAIL 74
#define EPROGMISMATCH 75
#define EPROCUNAVAIL 76
#define ENOLCK 77
#define ENOSYS 78
#define EFTYPE 79
#define EAUTH 80
#define ENEEDAUTH 81
#define EIDRM 82
#define ENOMSG 83
#define EOVERFLOW 84
#define ECANCELED 85
#define EILSEQ 86
#define ENOATTR 87
#define EDOOFUS 88
#define EBADMSG 89
#define EMULTIHOP 90
#define ENOLINK 91
#define EPROTO 92
#define ENOTCAPABLE 93
#define ECAPMODE 94
#define ENOTRECOVERABLE 95
#define EOWNERDEAD 96
#define EINTEGRITY 97

// poll.h
typedef int32_t nfds_t;

struct pollfd {
    int32_t fd;
    uint16_t events;
    uint16_t revents;
};

#define POLLIN 0x0001
#define POLLPRI 0x0002
#define POLLOUT 0x0004
#define POLLRDNORM 0x0040
#define POLLWRNORM POLLOUT
#define POLLRDBAND 0x0080
#define POLLWRBAND 0x0100
#define POLLRDHUP 0x4000
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020

// socket.h
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 28

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
    int32_t __pad;
    struct iovec *msg_iov;
    int32_t msg_iovlen;
    int32_t __pad2;
    void *msg_control;
    int32_t msg_controllen;
    uint32_t msg_flags;
};

struct msghdr_const {
    void *msg_name;
    int32_t msg_namelen;
    int32_t __pad;
    struct iovec_const *msg_iov;
    int32_t msg_iovlen;
    int32_t __pad2;
    void *msg_control;
    int32_t msg_controllen;
    uint32_t msg_flags;
};

#define MSG_OOB 0x00000001
#define MSG_PEEK 0x00000002
#define MSG_DONTROUTE 0x00000004
#define MSG_EOR 0x00000008
#define MSG_TRUNC 0x00000010
#define MSG_CTRUNC 0x00000020
#define MSG_WAITALL 0x00000040
#define MSG_NOSIGNAL 0x00020000

// dlfcn.h
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_GLOBAL 0x100

// ioccom.h
#define IOCPARM_SHIFT 13
#define IOCPARM_MASK ((1 << IOCPARM_SHIFT) - 1)
#define IOCPARM_LEN(x) (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x) ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x) (((x) >> 8) & 0xff)

#define IOCPARM_MAX (1 << IOCPARM_SHIFT)

#define IOC_VOID 0x20000000UL
#define IOC_OUT 0x40000000UL
#define IOC_IN 0x80000000UL
#define IOC_INOUT (IOC_IN | IOC_OUT)
#define IOC_DIRMASK (IOC_VOID | IOC_OUT | IOC_IN)

#define _IOC(inout, group, num, len) ((uint64_t)((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define _IO(g, n) _IOC(IOC_VOID, (g), (n), 0)
#define _IOR(g, n, t) _IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g, n, t) _IOC(IOC_IN, (g), (n), sizeof(t))
#define _IOWR(g, n, t) _IOC(IOC_INOUT, (g), (n), sizeof(t))

// un.h
#define SUNPATHLEN 104

struct sockaddr_un {
    uint8_t sun_len;
    uint8_t sun_family;
    char sun_path[SUNPATHLEN];
};

// fcntl.h
#define FREAD 0x1
#define FWRITE 0x2

#define O_RDONLY 0x0
#define O_WRONLY 0x1
#define O_RDWR 0x2
#define O_NONBLOCK 0x4
#define O_APPEND 0x8
#define O_SHLOCK 0x10
#define O_EXLOCK 0x20
#define O_ASYNC 0x40
#define O_FSYNC 0x80
#define O_SYNC 0x80
#define O_NOFOLLOW 0x100
#define O_CREAT 0x200
#define O_TRUNC 0x400
#define O_EXCL 0x800
#define FHASLOCK 0x4000
#define O_NOCTTY 0x8000
#define O_DIRECT 0x10000
#define O_DIRECTORY 0x20000
#define O_EXEC 0x40000
#define O_SEARCH O_EXEC
#define O_TTY_INIT 0x80000
#define O_CLOEXEC 0x100000
#define O_VERIFY 0x200000
#define O_PATH 0x400000
#define O_RESOLVE_BENEATH 0x800000
#define O_EMPTY_PATH 0x2000000

#define AT_FDCWD -100
#define AT_EACCESS 0x100
#define AT_SYMLINK_NOFOLLOW 0x200
#define AT_SYMLINK_FOLLOW 0x400
#define AT_REMOVEDIR 0x800
#define AT_RESOLVE_BENEATH 0x2000
#define AT_EMPTY_PATH 0x4000

// elf_common.h
#define AT_PAGESZ 6
#define AT_EXECPATH 15
#define AT_CANARY 16
#define AT_OSRELDATE 18
#define AT_NCPUS 19
#define AT_PAGESIZES 20
#define AT_TIMEKEEP 22
#define AT_HWCAP 25
#define AT_HWCAP2 26
#define AT_BSDFLAGS 27
#define AT_PS_STRINGS 32
#define AT_FXRNG 33
#define AT_USRSTACKBASE 35
#define AT_USRSTACKLIM 36

// mman.h
#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define MAP_SHARED 0x1
#define MAP_PRIVATE 0x2
#define MAP_COPY MAP_PRIVATE
#define MAP_FIXED 0x10
#define MAP_HASSEMAPHORE 0x200
#define MAP_STACK 0x400
#define MAP_NOSYNC 0x800
#define MAP_FILE 0x0
#define MAP_ANON 0x1000
#define MAP_ANONYMOUS MAP_ANON
#define MAP_GUARD 0x2000
#define MAP_EXCL 0x4000
#define MAP_NOCORE 0x20000
#define MAP_PREFAULT_READ 0x40000
#define MAP_32BIT 0x80000

#define MFD_CLOEXEC 0x1
#define MFD_ALLOW_SEALING 0x2
#define MFD_HUGETLB 0x4

// _timespec.h
struct timespec {
    int64_t tv_sec;
    int64_t tv_nsec;
};

// stat.h
struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    uint64_t st_nlink;
    uint16_t st_mode;
    int16_t __pad;
    uint32_t st_uid;
    uint32_t st_gid;
    int32_t __pad2;
    uint64_t st_rdev;
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    struct timespec st_birthtim;
    int64_t st_size;
    int64_t st_blocks;
    int32_t st_blksize;
    uint32_t st_flags;
    uint64_t st_gen;
    uint64_t __pad3[10];
};

#define S_ISDIR(M) (((M) & 0170000) == 0040000)
#define S_ISCHR(M) (((M) & 0170000) == 0020000)
#define S_ISBLK(M) (((M) & 0170000) == 0060000)
#define S_ISREG(M) (((M) & 0170000) == 0100000)
#define S_ISFIFO(M) (((M) & 0170000) == 0010000)
#define S_ISLNK(M) (((M) & 0170000) == 0120000)
#define S_ISSOCK(M) (((M) & 0170000) == 0140000)

// dirent.h
struct dirent {
    uint64_t d_fileno;
    int64_t d_off;
    uint16_t d_reclen;
    uint8_t d_type;
    uint8_t __pad;
    uint16_t d_namlen;
    uint16_t __pad2;
    // char d_name[];
};

// syslimits.h
#define NAME_MAX 255
#define PATH_MAX 1024

// _clock_id.h
#define CLOCK_MONOTONIC 4
