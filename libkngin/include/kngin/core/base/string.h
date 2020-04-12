#ifndef KNGIN_STRING_H
#define KNGIN_STRING_H

#include "kngin/core/define.h"
#include <string>

namespace k {

inline
void
clear_string (std::string &str) {
  std::string tmp;
  std::swap(str, tmp);
}

std::string &
format_string (std::string &result, const char *fmt, va_list vl,
               size_t offset = 0, size_t extra = 0);

std::string &
format_string (std::string &result, const char *fmt, ...);

std::string
format_string (const char *fmt, ...);

} /* namespace k */

#endif /* KNGIN_STRING_H */