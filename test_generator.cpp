#include <cstdio>
#include <ctime>
#include <cstdlib>

int nDomains, nVars, nOps;

int* dom[2];

int rand_between(int x, int y) // [x,y)
{
    return rand() % (y-x) + x;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        printf("Usage: %s <num_of_domains> <num_of_vars> <num_of_ops>\n", argv[0]);
        return 1;
    }
    
    srand(time(NULL));
    
    nDomains = atoi(argv[1]);
    nVars = atoi(argv[2]);
    nOps = atoi(argv[3]);
    
    printf("%d %d %d\n", nDomains, nVars, nOps);
    
    dom[0] = new int[nVars];
    dom[1] = new int[nVars];
    
    int i,j,sVars,sRels,v,a,b;
    
    for (i=0;i<nDomains;++i) {
        
        for (j=0;j<nVars;++j) {
            dom[0][j] = dom[1][j] = 0;
        }
        
        // at least 75% of the vars will be set to some values
        sVars = rand_between((int)(0.75 * nVars), nVars);
        
        for (j=0;j<sVars;++j) {
            v = rand_between(0, nVars);
            dom[0][v] = rand_between(0,1024);
            dom[1][v] = rand_between(dom[0][v]-128,dom[0][v]+1024); // 1/8 chance to be bottom
            
            printf("1 %d %d %d\n", v, dom[0][v], dom[1][v]);
        }
        
        sVars = nVars * nVars;
        sRels = rand_between((int)(0.1 * sVars), (int)(0.5 * sVars)); // density
        
        for (j=0;j<sRels;++j) {
            a = rand_between(0, nVars);
            b = rand_between(0, nVars);
            
            if (dom[0][a] > dom[1][a] || dom[0][b] > dom[1][b]) // one of them is bottom
                continue;
            if (dom[0][a] > dom[1][b]) // cannot be the case: a < b
                continue;
            
            printf("2 %d %d\n", a, b);
        }
        
        printf("0\n");
    }
    
    while (nOps) {
        a = rand_between(0, nVars);
        b = rand_between(0, nVars);
        if (a != b) {
            printf("%d %d\n", a, b);
            --nOps;
        }
    }
    
    return 0;
}
