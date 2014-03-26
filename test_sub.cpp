#include "sub.h"
#include<assert.h>
#include<iostream>

int main() {
    Sub s;
    s.less_than(1,2);
    assert(s.is_less_than(1,2));
    s.less_than(2,3);
    assert(!s.is_less_than(1,3));
    s.not_less_than(1,2);
    assert(!s.is_less_than(1,2));
}
