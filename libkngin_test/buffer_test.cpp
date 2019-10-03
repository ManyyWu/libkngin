#include <iostream>
#include <vector>
#include "../libkngin/core/buffer.h"

using namespace std;
using namespace k;

void
buffer_test ()
{
    try {
        uint8_arr p;
        p.resize(100);
        buffer b1(std::move(p));
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
