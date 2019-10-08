#include <atomic>
#include "common.h"
#include "logfile.h"
#include "log.h"
#include "logmgr.h"

__NAMESPACE_BEGIN

std::atomic<size_t> __g_memory_debug_total = 0;

__NAMESPACE_END
