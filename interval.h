#ifndef INTERVAL
#define INTERVAL

#include<limits.h>

// ---- Macros ----
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define INT_BOT (Interval(1,0))
#define INT_TOP (Interval(-INT_MAX, INT_MAX))

// ---- Classes ----

/*
 *  --- Interval class ---
 *  Top is represented by [-int_max, int_max]
 *  Bot is represented by all [a, b] such that a > b
 *  All others is straightforward [l, h]
 */
class Interval {
public:
    Interval() {
        *this = INT_TOP;
    }

    // Constructor
    Interval(int low, int high) {
        l_ = low;
        h_ = high;
    }

    // Overload equals
    inline bool operator== (const Interval &other) const {
        return l_ == other.l_ && h_ == other.h_;
    }

    // Join
    // Returns: join of this and other
    inline Interval join (Interval other) {
        if (this->is_bot())
            return other;
        if (other.is_bot())
            return *this;
        return Interval(MIN(l_, other.l_), MAX(h_, other.h_));
    }

    // Meet
    // Returns: meet of this and other
    inline Interval meet (Interval other) {
        return Interval(MAX(l_, other.l_), MIN(h_, other.h_));
    }

    // Checks if this is bot
    inline bool is_bot() {
        return l_ > h_;
    }
private:
    int l_, h_;
};

#endif // INTERVAL
