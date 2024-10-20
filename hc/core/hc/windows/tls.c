// Thread local storage for Windows.
// See: http://www.nynaeve.net/?tag=tls

struct tls_imageTlsDirectory {
    uint64_t startAddressOfRawData;
    uint64_t endAddressOfRawData;
    uint64_t addressOfIndex;
    uint64_t addressOfCallBacks;
    uint32_t sizeOfZeroFill;
    uint32_t characteristics;
};

typedef void (*tls_imageTlsCallback)(void *dllHandle, uint32_t reason, void *reserved);

tls_imageTlsCallback tls_callbacks[] = { NULL };

hc_SECTION(".tls") uint64_t _tls_start;
hc_SECTION(".tls$ZZZ") uint64_t _tls_end;

// When building with `-ftls-model=local-exec` this doesn't actually get used by code generation.
// If using tls in a .dll or similar, change that option to `global-dynamic`.
uint32_t _tls_index;

const struct tls_imageTlsDirectory _tls_used = {
    .startAddressOfRawData = (uint64_t)&_tls_start,
    .endAddressOfRawData = (uint64_t)&_tls_end,
    .addressOfIndex = (uint64_t)&_tls_index,
    .addressOfCallBacks = (uint64_t)&tls_callbacks[0],
    .sizeOfZeroFill = 0,
    .characteristics = 0
};
