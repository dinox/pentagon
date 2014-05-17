#include "benchmark.h"
#include "timer.h"

#include "pentagon_stl.h"
#include "pentagon_dense.h"
#include "pentagon_bp.h"
#include "pentagon_fwt.h"
#include "pentagon_simd.h"


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#ifdef ENABLE_CSV

#ifndef CSV_FILE
#define CSV_FILE "output.csv"
#endif

#ifndef CSV_DELIMITER
#define CSV_DELIMITER ','
#endif

FILE* csv = fopen(CSV_FILE, "a");
#endif

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
		if (!(a.getSubFor(i) == b.getSubFor(i))) {
			printf("Different SUB sets for var %d.\n", i);
			/*
			printf("Current: ");
			printSet(a.getSubFor(i));
			printf("DM: ");
			printSet(b.getSubFor(i));
			*/
			return false;
		}
	}

	for (int i=0;i<nVars;++i) {
		Interval x,y;
		x = a.getIntervalFor(i);
		y = b.getIntervalFor(i);
		if (!(x == y)) {
			printf("Different INTERVALs for var %d.\n", i);
			/*
			printf("Current: [%d,%d]\n", x.l_, x.h_);
			printf("DM: [%d,%d]\n", y.l_, y.h_);
			*/
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
		exit(1);
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
			if (rand_between(0,100) <= 80) {
				intervals[i][v].first = rand_between(0,1024);
				intervals[i][v].second = rand_between(intervals[i][v].first, intervals[i][v].first+1024);
			} else {
				// bottom
				intervals[i][v].first = 1;
				intervals[i][v].second = 0;
			}

		sRels = nVars * nVars;
		sRels = rand_between((int)(0.1 * (double)sRels), (int)(0.25 * (double)sRels)); // density
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

void Benchmark::benchDM()
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

	double perf = (double)OP_COUNT((unsigned long long) nVars)/(double)t.get_cycles();
	printf("%8s%12.3lf%10s%10.2lf%8s\n", "DM", perf, "--", (double)t.get_cycles() / (double)CPU_FREQ, "--");
#ifdef ENABLE_CSV
	fprintf(csv, "%lf%c", perf, CSV_DELIMITER);
#endif
#ifndef COMPARE_AGAINST_BP
	dmCycles = t.get_cycles();
	dmDomain = pent;
#endif
}

void Benchmark::benchSTL()
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

	bool verified = false;
	if (dmDomain != NULL) {
		verified = true;
		for (int i=0;i<nDoms;++i)
			if (!verify(pent[i], dmDomain[i])) {
				verified = false;
				break;
			}
	}

	double perf = (double)OP_COUNT((unsigned long long) nVars)/(double)t.get_cycles();
	printf("%8s%12.3lf%9.3lfx%10.2lf%8s\n", "STL", perf, dmCycles / (double)t.get_cycles(), (double)t.get_cycles() / (double)CPU_FREQ, verified ? "OK" : "FAIL");
#ifdef ENABLE_CSV
	fprintf(csv, "%lf%c", perf, CSV_DELIMITER);
#endif
}

void Benchmark::benchFWT()
{
	int i,j;
	PentagonFWT* pent = new PentagonFWT[nDoms];
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

	bool verified = false;
	if (dmDomain != NULL) {
		verified = true;
		for (int i=0;i<nDoms;++i)
			if (!verify(pent[i], dmDomain[i])) {
				verified = false;
				break;
			}
	}

	double perf = (double)OP_COUNT((unsigned long long) nVars)/(double)t.get_cycles();
	printf("%8s%12.3lf%9.3lfx%10.2lf%8s\n", "FWT", perf, dmCycles / (double)t.get_cycles(), (double)t.get_cycles() / (double)CPU_FREQ, verified ? "OK" : "FAIL");
#ifdef ENABLE_CSV
	fprintf(csv, "%lf%c", perf, CSV_DELIMITER);
#endif
}

void Benchmark::benchBP()
{
	int i,j;
	PentagonBP* pent = new PentagonBP[nDoms];
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

	bool verified = false;

#ifdef COMPARE_AGAINST_BP
	dmCycles = t.get_cycles();
	dmDomain = pent;
	verified = true;
#else
	if (dmDomain != NULL) {
		verified = true;
		for (int i=0;i<nDoms;++i)
			if (!verify(pent[i], dmDomain[i])) {
				verified = false;
				break;
			}
	}
#endif

	double perf = (double)OP_COUNT((unsigned long long) nVars)/(double)t.get_cycles();
	printf("%8s%12.3lf%9.3lfx%10.2lf%8s\n", "BP", perf, dmCycles / (double)t.get_cycles(), (double)t.get_cycles() / (double)CPU_FREQ, verified ? "OK" : "FAIL");
#ifdef ENABLE_CSV
	fprintf(csv, "%lf%c", perf, CSV_DELIMITER);
#endif
}

void Benchmark::benchSIMD()
{
#ifdef ENABLE_SIMD

#ifdef AVX
	printf("SIMD is using AVX\n");
#elif defined SSE
	printf("SIMD is using SSE\n");
#else
#error "Probably you don't want to test SIMD on scalar unit :)"
    printf("SIMD is using scalar unit\n");
#endif

#endif
	int i,j;
	PentagonSIMD* pent = new PentagonSIMD[nDoms];
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

	bool verified = false;
	if (dmDomain != NULL) {
		verified = true;
		for (int i=0;i<nDoms;++i)
			if (!verify(pent[i], dmDomain[i])) {
				verified = false;
				break;
			}
	}

	double perf = (double)OP_COUNT((unsigned long long) nVars)/(double)t.get_cycles();
	printf("%8s%12.3lf%9.3lfx%10.2lf%8s\n", "SIMD", perf, dmCycles / (double)t.get_cycles(), (double)t.get_cycles() / (double)CPU_FREQ, verified ? "OK" : "FAIL");
#ifdef ENABLE_CSV
	fprintf(csv, "%lf%c", perf, CSV_DELIMITER);
#endif
}

void print_usage()
{
	printf("Usage:\n");
	printf("benchmark rand <Vars> <Doms> <Joins>\n");
	printf("benchmark genfile <filename> <Vars> <Doms> <Joins>\n");
	printf("benchmark benchfile <filename>\n");
	exit(1);
}

void Benchmark::BenchAll()
{
	printf("%8s%12s%10s%10s%8s\n", "Domain", "Ops/cycle", "Perf", "Time", "Verify");
#ifdef ENABLE_DM
	benchDM();
#endif
#ifdef ENABLE_STL
	benchSTL();
#endif
#ifdef ENABLE_FWT
	benchFWT();
#endif
#ifdef ENABLE_BP
	benchBP();
#endif
#ifdef ENABLE_SIMD
	benchSIMD();
#endif
#ifdef ENABLE_CSV
	fprintf(csv, "\n");
#endif
}

int main(int argc, char** argv)
{
	int nDoms, nVars, nJoins;
	Benchmark b;

	if (argc < 3)
		print_usage();
	else {
		if (strcmp(argv[1], "rand") == 0 && argc == 5) {
			b.generate(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
			b.BenchAll();
		} else if (strcmp(argv[1], "genfile") == 0 && argc == 6) {
			b.generate(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
			b.saveToFile(argv[2]);
		} else if (strcmp(argv[1], "benchfile") == 0 && argc == 3) {
			b.loadFromFile(argv[2]);
			b.BenchAll();
		} else {
			print_usage();
		}
	}

#ifdef ENABLE_CSV
	fclose(csv);
#endif

	return 0;
}
