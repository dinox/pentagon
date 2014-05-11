#ifndef __BENCHMARK__
#define __BENCHMARK__

#include "pentagon_dense.h"
#include <vector>
#include <stddef.h>
using namespace std;

class Benchmark
{
public:
	int nVars, nDoms, nJoins;
	Benchmark() {
		dmCycles = 0;
		dmDomain = NULL;
	}
	void generate(int nVars, int nDoms, int nJoins);

	void BenchAll();
	void benchDM();
	void benchSTL();
	void benchFWT();
	void benchBP();
	void benchSIMD();
	// bench whatever

	void loadFromFile(const char* path);
	void saveToFile(const char* path);
private:
	vector < vector< pair<int,int> > > intervals;
	vector < vector< pair<int,int> > > relations;
	vector < pair<int,int> > joins;

	unsigned long long dmCycles;
	PentagonDM* dmDomain;
};

#endif
