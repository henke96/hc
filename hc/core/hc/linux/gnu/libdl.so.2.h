void *dlopen(const char *path, int32_t flags);
int32_t dlclose(void *handle);
void *dlsym(void *handle, const char *symbol);
char *dlerror(void);
