#include "libkngin.h"
#include <iostream>
#include <memory.h>
#include <string>
#include <vector>

using namespace std;
using namespace k;

#define SIZE 50

std::string
dump (const char *data, size_t size);

void
dump_all (std::vector<uint8_t *> &buffers);

struct test {
  void
  append_buffer(const k::out_buffer &buf) {
    const size_t size = buf.size();
    const uint8_t *data = buf.begin();
    if (size) {
      while (windex_ + size > SIZE * buffers_.size())
        buffers_.push_back(new uint8_t[SIZE]);

      size_t which = (windex_ / SIZE);
      size_t new_windex = windex_ + size;
      size_t begin = 0;
      do {
        size_t index = windex_ % SIZE;
        size_t writeable = (std::min)(size - begin, SIZE - index);
        ::memcpy(buffers_[which] + index, data, writeable);
        //dump_all(buffers_);
        assert((windex_ + writeable) <= new_windex);
        if ((windex_ += writeable) >= new_windex)
          break;
        data += writeable;
        ++which;
        begin += writeable;
      } while (true);
    }
  }

  std::vector<uint8_t *> buffers_;
  size_t rindex_;
  size_t windex_;
};

std::string
dump (const char *data, size_t size) {
  std::string result;
  result.resize(size * 2 + 1);
  char tmp[3] = {0};
  for (size_t i = 0; i < size; ++i) {
    ::snprintf(tmp, sizeof(tmp), "%02x", data[i]);
    result[2 * i] = tmp[0];
    result[2 * i + 1] = tmp[1];
  }
  return result;
}

void
dump_all (std::vector<uint8_t *> &buffers) {
  for (auto &iter : buffers) {
    std::cerr << dump((const char *)iter, SIZE) << std::endl;
  }
  std::cerr << std::endl;
}

int
main () {
  test t;
  t.buffers_.push_back(new uint8_t[SIZE + 1]);
  t.rindex_ = 0;
  t.windex_ = 0;

  {
    char arr[] = "abcdefghijklmnopqrstuvwxyz";
    int len = 26;
    for (int i = 0; i < 50; ++i)
      t.append_buffer(k::out_buffer(arr, len));
  }

  {
    char arr[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
    int len = 26;
    for (int i = 0; i < 25; ++i)
      t.append_buffer(k::out_buffer(arr, len));
  }

  for (auto &iter : t.buffers_) {
    iter[SIZE] = '\0';
    std::cerr << iter << std::endl;
  }

  return 0;
}
