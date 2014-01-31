#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include <iostream>

class Time {
    struct timeval m_time;

public:
    Time();

    static Time now();

    Time &operator-=( const Time &rhs );
    Time operator-( const Time &rhs );
    Time &operator+=( const Time &rhs );
    Time operator+( const Time &rhs );

    const struct timeval &get() const
    {
        return m_time;
    }
};

std::ostream &operator<< ( std::ostream &stream, const Time &time );

#endif // TIME_H
