#include "interval.h"
#include<assert.h>
#include<iostream>

using namespace std;

int main(void) {
    Interval    i00(0, 0),
                i12(1, 2),
                i34(3, 4),
                i14(1, 4),
                i_bot = INT_BOT,
                i_top = INT_TOP,
                tmp(0, 0);
    assert(i00 == tmp);
    assert(i12.join(i34) == i14);
    assert(i12.join(i34) == i14);
    assert(i12.meet(i34).is_bot());
}
