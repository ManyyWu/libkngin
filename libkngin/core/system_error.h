#ifdef _WIN32
#include <Windows.h>
#else
#include <cerrno>
#endif
#include <system_error>
#include "exception.h"

KNGIN_NAMESPACE_K_BEGIN

inline std::error_code
int2ec(int32_t _code);

inline std::error_code
last_error    ();

class system_error : public k::exception {

public:
    system_error  () = delete;

    explicit
    system_error  (const char *_what, std::error_code _sc = k::last_error())
        : k::exception(_what), m_sc(_sc) {}

    ~system_error () = default;

public:
    const std::system_error &
    error         () const { return m_sc; }

protected:
    const std::error_code m_sc;
};

inline std::error_code
int2ec (int32_t _code)
{
    return std::make_error_code(static_cast<std::errc>(_code));
}

inline std::error_code
last_error ()
{
#ifdef _WIN32
    return std::make_error_code(static_cast<std::errc>(::GetLastError()));
#else
    return std::make_error_code(static_cast<std::errc>(errno));
#endif
}

KNGIN_NAMESPACE_K_END
