#include <cstdio>
#include "timer.h"

#include "pentagon_domain.h"
#include "pentagon_dense.h"

Pentagon* base;
PentagonDM* dense;

int nDomains, nVars, nOps;
std::pair<int,int>* ops;

void read_input()
{
    scanf("%d %d %d\n", &nDomains, &nVars, &nOps);
    
    base = new Pentagon[nDomains];
    dense = new PentagonDM[nDomains];
    ops = new std::pair<int,int>[nOps];
    
    int i,j,a,b,com,v;
    
    for (i=0;i<nDomains;++i) {
        
        dense[i].allocate(nVars);
        while (1) {
            scanf("%d",&com);
            if (com == 0)
                break;
            if (com == 1) {
                scanf("%d %d %d", &v, &a, &b);
                base[i].addIntervalFor(v, Interval(a,b));
                dense[i].addIntervalFor(v, Interval(a,b));
            } else if (com == 2) {
                scanf("%d %d", &a, &b);
                base[i].addSubFor(a,b);
                dense[i].addSubFor(a,b);
            }
        }
    }
    
    for (i=0;i<nOps;++i)
        scanf("%d %d", &ops[i].first, &ops[i].second);
}

void test_base()
{
    Timer t(1);
    
    t.start();
    
    for (int i=0;i<nOps;++i) {
        printf("join %d %d\n", ops[i].first, ops[i].second);
        base[ops[i].first].join(base[ops[i].second]);
    }
    
    t.stop();
    printf("Base domain perf:\n");
    t.print_cycles();
}

void test_DM()
{
    Timer t(1);
    
    t.start();
    
    for (int i=0;i<nOps;++i) {
        dense[ops[i].first].join(dense[ops[i].second]);
    }
    
    t.stop();
    printf("DM domain perf:\n");
    t.print_cycles();
}

void verify()
{
    
}

int main(int argc, char** argv)
{
    read_input();
    test_base();
    //test_DM();
    return 0;
}
