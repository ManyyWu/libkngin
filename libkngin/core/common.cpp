#include <cstdarg>
#include <cstring>
#include "common.h"
#include "logfile.h"
#include "log.h"
#include "logmgr.h"

__NAMESPACE_BEGIN

size_t     __memory_debug_total = 0;
void *     __memory_debug_addr  = NULL;
std::mutex __memory_debug_mutex;

__NAMESPACE_END
