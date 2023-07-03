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
