#define RTLD_LAZY 0x1
#define RTLD_NOW 0x2
#define RTLD_GLOBAL 0x100

void *dlopen(const char *filename, int32_t flags);
int32_t dlclose(void *handle);
void *dlsym(void *handle, const char *symbolname);
char *dlerror(void);