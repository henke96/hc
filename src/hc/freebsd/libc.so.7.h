extern int32_t errno;

void __libc_start1(int32_t argc, char **argv, char **env, void (*cleanup)(void), void *main);
int32_t printf(const char *restrict fmt, ...);
noreturn void abort(void);
void *mmap(void *addr, int64_t size, uint32_t prot, uint32_t flags, int32_t fd, int64_t offset);
int32_t munmap(void *addr, int64_t size);
int32_t mprotect(void *addr, int64_t size, int32_t prot);
int32_t openat(int32_t fd, const char *path, uint32_t flags, ...);
int32_t fstatat(int32_t fd, const char *path, struct stat *stat, uint32_t flags);
int64_t read(int32_t fd, void *buffer, int64_t size);
int64_t write(int32_t fd, const void *buffer, int64_t size);
int32_t close(int32_t fd);
int32_t chdir(const char *path);
int32_t elf_aux_info(int32_t aux, void *buffer, int32_t size);
