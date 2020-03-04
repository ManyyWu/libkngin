#ifndef KNGIN_STRING_H
#define KNGIN_STRING_H

#include "kngin/core/define.h"
#include <string>

KNGIN_NAMESPACE_K_BEGIN

inline
void
clear_string (std::string &str) {
  std::string tmp;
  std::swap(str, tmp);
}

std::string &
format_string (std::string &result, const char *fmt, va_list vl);

std::string &
format_string (std::string &result, const char *fmt, ...);

std::string
format_string (const char *fmt, ...);

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_STRING_H */