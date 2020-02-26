#ifndef KNGIN_TCP_CLIENT_H
#define KNGIN_TCP_CLIENT_H

#include <functional>
#include <memory>
#include "core/base/define.h"
#include "core/base/noncopyable.h"
#include "core/event/event_loop.h"
#include "net/address.h"
#include "net/tcp/server_opts.h"
#include "net/tcp/session.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

typedef class session client;

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TCP_CLIENT_H */
