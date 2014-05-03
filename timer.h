#include<stdio.h>

class Timer {
public:
    Timer(int opcount) {
        opcount_ = opcount;
    }

    inline void start() {
        cpuid();
        start_ = rdtsc();
    }

    inline void stop() {
        stop_ = rdtsc();
        cpuid();
    }

    void print_perf() {
        printf("Performance: %.2f ops/cycle\n", 1.0 * opcount_ / (stop_-start_));
    }

    void print_cycles() {
        printf("Elapsed cycles: %.2lld cycles\n", stop_-start_);
    }
    
    unsigned long long get_cycles() {
        return stop_-start_;
    }

private:
    static inline unsigned long long rdtsc() {
        unsigned hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
    }

    static inline void cpuid() {
        __asm__ __volatile__ ("cpuid" : : "a" (0) : "bx", "cx", "dx" );
    }

    unsigned long long opcount_, start_, stop_;
};
