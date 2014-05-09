#include "benchmark.h"
#include "timer.h"

#include "pentagon_stl.h"
#include "pentagon_dense.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

int rand_between(int x, int y) // [x,y)
{
	return rand() % (y-x) + x;
}

void printSet(set<int> s)
{
	for (set<int>::iterator ii = s.begin(); ii != s.end(); ++ii)
		printf("%d ", *ii);
	printf("\n");
}

bool verify(Pentagon& a, Pentagon& b)
{
	if (a.getNumOfVars() != b.getNumOfVars()) {
		printf("Different number of variables.\n");
		return false;
	}
	int nVars = a.getNumOfVars();

	for (int i=0;i<nVars;++i) {
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

	for (int i=0;i<nVars;++i) {
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

void Benchmark::saveToFile(const char* path)
{
	int i,j;
	FILE* f = fopen(path, "w");
	fprintf(f, "%d %d\n", nVars, nDoms);
	for (i=0;i<nDoms;++i) {
		for (j=0;j<nVars;++j)
			fprintf(f, "%d %d\n", intervals[i][j].first, intervals[i][j].second);
		fprintf(f, "%d\n", relations[i].size());
		for (j=0;j<relations[i].size();++j)
			fprintf(f, "%d %d\n", relations[i][j].first, relations[i][j].second);
	}
	fprintf(f, "%d\n", nJoins);
	for (i=0;i<nJoins;++i) {
		fprintf(f, "%d %d\n", joins[i].first, joins[i].second);
	}
	fclose(f);
}

void Benchmark::loadFromFile(const char* path)
{
	int i,j,sRels;
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		printf("Got an error while reading the file %s!", path);
		return;
	}
	fscanf(f, "%d %d\n", &nVars, &nDoms);
	intervals.resize(nDoms);
	relations.resize(nDoms);

	for (i=0;i<nDoms;++i) {
		intervals[i].resize(nVars);
		for (j=0;j<nVars;++j)
			fscanf(f, "%d %d\n", &intervals[i][j].first, &intervals[i][j].second);
		fscanf(f, "%d\n", &sRels);
		relations[i].resize(sRels);
		for (j=0;j<sRels;++j)
			fscanf(f, "%d %d\n", &relations[i][j].first, &relations[i][j].second);
	}
	fscanf(f, "%d\n", &nJoins);
	joins.resize(nJoins);
	for (i=0;i<nJoins;++i) {
		fscanf(f, "%d %d\n", &joins[i].first, &joins[i].second);
	}
	fclose(f);
}

void Benchmark::generate(int nVars, int nDoms, int nJoins)
{
	this->nVars = nVars;
	this->nDoms = nDoms;
	this->nJoins = nJoins;

	srand(time(NULL)); // new seed

	intervals.resize(nDoms);
	relations.resize(nDoms);
	joins.resize(nJoins);

	int i,j,sRels,v,a,b;

	for (i=0;i<nDoms;++i) {
		intervals[i].resize(nVars);

		for (v=0;v<nVars;++v)
			if (rand_between(0,100) <= 90) {
				intervals[i][v].first = rand_between(0,1024);
				intervals[i][v].second = rand_between(intervals[i][v].first, intervals[i][v].first+1024);
			} else {
				// bottom
				intervals[i][v].first = 1;
				intervals[i][v].second = 0;
			}

		sRels = nVars * nVars;
		sRels = rand_between((int)(0.1 * (double)sRels), (int)(0.5 * (double)sRels)); // density
		relations[i].resize(sRels);

		for (j=0;j<sRels;++j) {
			a = rand_between(0, nVars);
			b = rand_between(0, nVars);

			if (intervals[i][a].first > intervals[i][a].second || intervals[i][b].first > intervals[i][b].second) // one of them is bottom
				continue;
			if (intervals[i][a].first > intervals[i][b].second) // cannot be the case: a < b
				continue;

			relations[i][j] = pair<int,int>(a,b);
		}
	}

	j = 0;
	while (j < nJoins) {
		a = rand_between(0, nDoms);
		b = rand_between(0, nDoms);
		if (a != b) {
			joins[j] = pair<int,int>(a,b);
			++j;
		}
	}
}

PentagonDM* Benchmark::benchDM()
{
	int i,j;
	PentagonDM* pent = new PentagonDM[nDoms];
	for (i=0;i<nDoms;++i)
		pent[i].allocate(nVars);

	for (i=0;i<nDoms;++i) {
		for (j=0;j<nVars;++j)
			pent[i].setIntervalFor(j, Interval(intervals[i][j].first, intervals[i][j].second));
		for (j=0;j<relations[i].size();++j)
			pent[i].setSubFor(relations[i][j].first, relations[i][j].second);
	}

	Timer t(1);
	t.start();

	for (i=0;i<nJoins;++i)
		pent[joins[i].first].join(pent[joins[i].second]);

	t.stop();
	printf("PentagonDM domain perf:\n");
	t.print_cycles();
	return pent;
}

void Benchmark::benchSTL(PentagonDM* orig)
{
	int i,j;
	PentagonSTL* pent = new PentagonSTL[nDoms];
	for (i=0;i<nDoms;++i)
		pent[i].allocate(nVars);

	for (i=0;i<nDoms;++i) {
		for (j=0;j<nVars;++j)
			pent[i].setIntervalFor(j, Interval(intervals[i][j].first, intervals[i][j].second));
		for (j=0;j<relations[i].size();++j)
			pent[i].setSubFor(relations[i][j].first, relations[i][j].second);
	}

	Timer t(1);
	t.start();

	for (i=0;i<nJoins;++i)
		pent[joins[i].first].join(pent[joins[i].second]);

	t.stop();
	printf("PentagonSTL domain perf:\n");
	t.print_cycles();

	if (orig) { // verify
		bool verified = true;

		for (int i=0;i<nDoms;++i)
			if (!verify(pent[i], orig[i])) {
				printf("STL Verification failed for domain %d\n", i);
				verified = false;
				break;
			}

		if (verified)
			printf("Verification completed!\n");
	}

}

int main()
{
	Benchmark b;
	b.generate(10, 3, 4);
	//b.loadFromFile("test");
	PentagonDM* pentDM = b.benchDM();
	b.benchSTL(pentDM);
	return 0;
}