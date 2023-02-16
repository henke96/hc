#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_CREATE_JOINABLE 0

struct pthread_attr_t {
    uint32_t flags;
    int32_t __pad;
    void *stackBase;
    uint64_t stackSize;
    uint64_t guardSize;
    int32_t schedPolicy;
    int32_t schedPriority;
    char __pad2[16];
};

int32_t pthread_create(int64_t *pthread, const struct pthread_attr_t *attr, void *(*func)(void *), void *arg);
int32_t pthread_join(int64_t pthread, void **returnValue);
int32_t pthread_attr_init(struct pthread_attr_t *attr);
int32_t pthread_attr_destroy(struct pthread_attr_t *attr);
int32_t pthread_attr_setdetachstate(struct pthread_attr_t *attr, int32_t state);

void *malloc(uint64_t size);

int32_t clock_gettime(int32_t clock, struct timespec *time);
