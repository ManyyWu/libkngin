#include <iostream>
#include "../libkngin/core/buffer.h"

using namespace std;
using namespace k;

void
buffer_test ()
{
    try {
        uint8_uarr p = make_uint8_uarray(100);
        buffer b1(std::move(p), 100);
        cerr << b1.size() << endl;
        for (int i = 0; i < 100; ++i)
            b1.write_uint8(i);
        std::string s;
        b1.dump(s);
        cerr << s.c_str() << endl;
        b1.reset(0);
        b1.write_bytes((uint8_t *)"0123456789", 12);
        b1.dump(s);
        cerr << s.c_str() << endl;
    } catch (logic_exception &e) {
        cerr << e.what() << endl;
    } catch (...) {
        throw;
    }
}
