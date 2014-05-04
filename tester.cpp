#include <cstdio>
#include "timer.h"

#include "pentagon_stl.h"
#include "pentagon_dense.h"

using namespace std;

PentagonSTL* stl;
PentagonDM* dense;

int nDomains, nVars, nJoins;
pair<int,int>* joins;

void read_input()
{
    scanf("%d %d %d\n", &nDomains, &nVars, &nJoins);
    
    stl = new PentagonSTL[nDomains];
    dense = new PentagonDM[nDomains];
    joins = new pair<int,int>[nJoins];
    
    int i,j,a,b,com,v;
    
    for (i=0;i<nDomains;++i) {
        
    	stl[i].allocate(nVars);
        dense[i].allocate(nVars);
        while (1) {
            scanf("%d",&com);
            if (com == 0)
                break;
            if (com == 1) {
                scanf("%d %d %d", &v, &a, &b);
                stl[i].setIntervalFor(v, Interval(a,b));
                dense[i].setIntervalFor(v, Interval(a,b));
            } else if (com == 2) {
                scanf("%d %d", &a, &b);
                stl[i].setSubFor(a,b);
                dense[i].setSubFor(a,b);
            }
        }
    }
    
    for (i=0;i<nJoins;++i)
        scanf("%d %d", &joins[i].first, &joins[i].second);
}
/*
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
*/

void printSet(set<int> s)
{
	for (set<int>::iterator ii = s.begin(); ii != s.end(); ++ii)
		printf("%d ", *ii);
	printf("\n");
}

bool verify(PentagonSTL& a, PentagonDM& b)
{
	if (a.getNumOfVars() != b.getNumOfVars()) {
		printf("Different number of variables.\n");
		return false;
	}

	for (int i=0;i<a.getNumOfVars();++i) {
		Interval x,y;
		x = a.getIntervalFor(i);
		y = b.getIntervalFor(i);
		if (!(x == y)) {
			printf("Different INTERVALs for var %d.\n", i);
			printf("STL: [%d,%d]\n", x.l_, x.h_);
			printf("DM: [%d,%d]\n", y.l_, y.h_);
			return false;
		}
	}

	for (int i=0;i<a.getNumOfVars();++i) {
		if (!(a.getSubFor(i) == b.getSubFor(i))) {
			printf("Different SUB sets for var %d.\n", i);
			printf("STL: ");
			printSet(a.getSubFor(i));
			printf("DM: ");
			printSet(b.getSubFor(i));
			return false;
		}
	}

	return true;
}

void test_()
{
	bool verified = true;

    for (int i=0;i<nJoins;++i) {
        printf("join %d %d\n", joins[i].first, joins[i].second);
        stl[joins[i].first].join(stl[joins[i].second]);
        dense[joins[i].first].join(dense[joins[i].second]);
    }

    for (int i=0;i<nDomains;++i)
    	if (!verify(stl[i], dense[i])) {
    		printf("Verification failed for domain %d\n", i);
    		verified = false;
    	}

    printf("Verification%scompleted!\n", verified ? " " : " NOT ");
}

int main(int argc, char** argv)
{
    read_input();
    test_();
    //test_base();
    //test_DM();
    return 0;
}



