#include "hc/hc.h"
#include "hc/efi.h"

int64_t _start(hc_UNUSED void *imageHandle, struct efi_systemTable *systemTable) {
    int64_t status = systemTable->consoleOut->outputString(systemTable->consoleOut, u"Hello!\r\n");
    if (status < 0) return 1;
    return 0;
}
