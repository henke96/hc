// Should not return.
int32_t __libc_start_main(
    void *main,
    int32_t argc,
    char **argv,
    void (*init)(void),
    void (*fini)(void),
    void (*rtld_fini)(void)
);
int32_t __cxa_atexit(void (*func)(void *), void *arg, void *dso);
int32_t *__errno_location(void);

int32_t printf(const char *fmt, ...);
int32_t clock_gettime(int32_t clockid, struct timespec *time);
uint64_t getauxval(uint64_t type);
int32_t memfd_create(const char *name, uint32_t flags);
int32_t ftruncate(int32_t fd, int64_t size);
void *mmap(void *addr, int64_t size, int32_t prot, uint32_t flags, int32_t fd, int64_t offset);
int32_t munmap(void *addr, int64_t size);
int32_t socket(int32_t family, int32_t type, int32_t protocol);
int32_t connect(int32_t fd, const void *addr, int32_t addrlen);
int64_t sendmsg(int32_t fd, const struct msghdr_const *msg, uint32_t flags);
int64_t sendto(int32_t fd, const void *buf, int64_t size, uint32_t flags, const void *addr, int32_t addrlen);
int64_t recvfrom(int32_t fd, void *buf, int64_t size, uint32_t flags, void *addr, int32_t *addrlen);
int64_t writev(int32_t fd, const struct iovec_const *iov, int32_t iovlen);
int32_t close(int32_t fd);
int32_t openat(int32_t fd, const char *path, uint32_t flags, ...);
int32_t fstatat(int32_t fd, const char *path, struct stat *stat, uint32_t flags);
int32_t ioctl(int32_t fd, uint64_t request, ...);
int64_t read(int32_t fd, void *buffer, int64_t size);
int32_t ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const void *sigmask); // TODO sigmask
