#ifndef __BENCHMARK__
#define __BENCHMARK__

#include "pentagon_dense.h"
#include <vector>
using namespace std;

class Benchmark
{
public:
	int nVars, nDoms, nJoins;
	void generate(int nVars, int nDoms, int nJoins);

	PentagonDM* benchDM();
	void benchSTL(PentagonDM* base);
	// bench whatever

	void loadFromFile(const char* path);
	void saveToFile(const char* path);
private:
	vector < vector< pair<int,int> > > intervals;
	vector < vector< pair<int,int> > > relations;
	vector< pair<int,int> > joins;
};

#endif
