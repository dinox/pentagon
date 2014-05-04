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
class Interval 
{
public:
    Interval() 
    {
        l_ = 1;
        h_ = 0;
    }

    // Constructor
    Interval(int low, int high) 
    {
        l_ = low;
        h_ = high;
    }

    // Overload equals
    inline bool operator== (const Interval &other) const 
    {
    	if (l_ == other.l_ && h_ == other.h_)
    		return true;
    	if (this->is_bot() && other.is_bot())
    		return true;
    	return false;
    }

    // Join
    inline void join(const Interval& other) 
    {
    	if (is_bot()) {
    		l_ = other.l_;
    		h_ = other.h_;
    	} else if (!other.is_bot()) {
    		l_ = MIN(l_, other.l_);
			h_ = MAX(h_, other.h_);
    	}
    }

    // Meet
    inline void meet(const Interval& other) 
    {
        l_ = MAX(l_, other.l_);
        h_ = MIN(h_, other.h_);
    }

    inline bool is_bot() const 
    {
        return l_ > h_;
    }
    
    inline bool is_top() const 
    {
        return *this == INT_TOP;
    }
    
    inline void assumeLessThan(const Interval& other)
    {
        h_ = MIN(h_, other.h_ - 1);
        // equivalent to
        // meet(Interval(l_, other.h_-1));
    }
    
    inline void assumeGreaterThan(const Interval& other)
    {
        l_ = MAX(l_, other.l_ + 1);
        // equivalent to
        // meet(Interval(other.l_+1, h_));
    }
    
    inline bool lessThan(const Interval& other) const
    {
        if (is_bot() || other.is_bot())
            return false;
        return h_ < other.l_;
    }
    
    int l_, h_;
};

#endif // INTERVAL
