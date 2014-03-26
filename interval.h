#ifndef INTERVAL
#define INTERVAL

#include<limits.h>

// ---- Macros ----
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define INT_BOT (Interval(1,0))
#define INT_TOP (Interval(-INT_MAX, INT_MAX))

// ---- Classes ----
struct Interval {
    int l, h;

    // Constructor
    Interval(int low, int high) {
        l = low;
        h = high;
    }

    // Copy constructor
    Interval(const Interval &src) {
        l = src.l;
        h = src.h;
    }

    // Overload assignment operator
    Interval& operator= (const Interval &src) {
        l = src.l;
        h = src.h;
        return *this;
    }

    // Overload equals
    bool operator== (const Interval &other) const {
        return l == other.l && h == other.h;
    }

    Interval *join (Interval other, Interval *dest) {
        dest->l = MIN(l, other.l);
        dest->h = MAX(h, other.h);
        return dest;
    }

    Interval *meet (Interval other, Interval *dest) {
        int low = MAX(l, other.l);
        int high = MIN(h, other.h);
        // Bot
        if (low > high) {
            low = INT_BOT.l;
            high = INT_BOT.h;
        }
        *dest = Interval(low, high);
        return dest;
    }
};

#endif // INTERVAL
