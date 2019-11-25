#include <iostream>
#include <vector>
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/exception.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/buffer_test.cpp"

using namespace std;
using namespace k;

void
buffer_test ()
{

    try {
        buffer b1(10);
        b1.write_bytes((uint8_t *)"0123456789", 10);
        cerr << b1.dump() << endl;
        cerr << b1.dump() << endl;

        buffer b2(10);
        b2.write_bytes((uint8_t *)"9876543210", 10);
        b1.append(b2);
        cerr << b1.dump() << endl;
    } catch (k::exception &e) {
        cerr << e.what() << endl;
    } catch (...) {
        throw;
    }
}
