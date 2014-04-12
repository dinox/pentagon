#include "interval_domain.h"
#include "sub_domain.h"
#include <cassert>

using namespace std;

void test_interval()
{
    Interval    i00(0, 0),
                i12(1, 2),
                i34(3, 4),
                i14(1, 4),
                i_bot = INT_BOT,
                i_top = INT_TOP,
                tmp(0, 0);
    assert(i00 == tmp);
    Interval x = i12;
    x.join(i34);
    assert(x == i14);
    x.join(i34);
    assert(x == i14);
    i12.meet(i34);
    assert(i12.is_bot());
}

void test_interval_domain()
{
    IntervalDomain dom1, dom2;
    
    dom1.setForVar(3, Interval(2, 8));
    dom1.setForVar(5, Interval(1, 5));
    
    dom2.setForVar(5, Interval(4, 9));
    dom2.setForVar(8, Interval(3, 5));
    
    dom1.join(dom2);
    assert(dom1.getForVar(0) == INT_BOT);
    assert(dom1.getForVar(3) == Interval(2,8));
    assert(dom1.getForVar(5) == Interval(1,9));
    assert(dom1.getForVar(8) == Interval(3,5));
    
    SubDomain sub;
    sub.lessThan(3, 5);
    sub.lessThan(5, 8);
    
    dom1.inferFromSub(sub);
    
    assert(dom1.getForVar(0) == INT_BOT);
    assert(dom1.getForVar(3) == Interval(2,8));
    assert(dom1.getForVar(5) == Interval(3,4));
    assert(dom1.getForVar(8) == Interval(4,5));
}

void test_sub_domain()
{
    SubDomain s;
    s.lessThan(1,2);
    assert(s.isLessThan(1,2));
    s.lessThan(2,3);
    assert(!s.isLessThan(1,3));
    s.notLessThan(1,2);
    assert(!s.isLessThan(1,2));
    
    IntervalDomain ii;
    ii.setForVar(3, Interval(2, 4));
    ii.setForVar(5, Interval(8, 9));
    
    SubDomain ss;
    ss.lessThan(1,3);
    ss.lessThan(5,10);
    ss.lessThan(7,12);
    
    ss.inferFromInterval(ii); //should infer 3 < 5
    assert(ss.isLessThan(3,5));
    ss.closure(); // should close the chain 1->3->5->10
    
    assert(ss.isLessThan(1,3));
    assert(ss.isLessThan(1,5));
    assert(ss.isLessThan(1,10));
    assert(ss.isLessThan(3,5));
    assert(ss.isLessThan(3,10));
    assert(ss.isLessThan(7,12));
}

int main()
{
    test_interval();
    test_interval_domain();
    test_sub_domain();
    return 0;
}
