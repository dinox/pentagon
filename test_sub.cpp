#include "sub_domain.h"
#include<assert.h>
#include<iostream>

int main() {
    SubDomain s;
    s.lessThan(1,2);
    assert(s.isLessThan(1,2));
    s.lessThan(2,3);
    assert(!s.isLessThan(1,3));
    s.notLessThan(1,2);
    assert(!s.isLessThan(1,2));
}
