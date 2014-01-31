#include "time.h"

#include <system_error>

Time::Time()
{
    m_time.tv_sec = 0;
    m_time.tv_usec = 0;
}

Time Time::now()
{
    Time ret;
    if( gettimeofday( &ret.m_time, NULL )<0 )
        throw  std::system_error(errno, std::system_category(), "Failed to get time of day information");

    return ret;
}

Time &Time::operator-=( const Time &rhs )
{
    if( rhs.m_time.tv_usec>m_time.tv_usec ) {
        m_time.tv_usec += 1000000;
        m_time.tv_sec--;
    }

    m_time.tv_usec -= rhs.m_time.tv_usec;
    m_time.tv_sec -= rhs.m_time.tv_sec;

    return *this;
}

Time Time::operator-( const Time &rhs )
{
    Time ret(*this);
    ret-=rhs;

    return ret;
}

Time &Time::operator+=( const Time &rhs )
{
    m_time.tv_usec += rhs.m_time.tv_usec;
    m_time.tv_sec += rhs.m_time.tv_sec;

    if( m_time.tv_usec>1000000 ) {
        m_time.tv_usec -= 1000000;
        m_time.tv_sec ++;
    }
}

Time Time::operator+( const Time &rhs )
{
    Time ret(*this);
    ret+=rhs;

    return ret;
}

std::ostream &operator<< ( std::ostream &stream, const Time &time )
{
    stream<<time.get().tv_sec<<".";

    std::streamsize oldwidth = stream.width(6);
    char oldfill = stream.fill('0');

    stream<<time.get().tv_usec;

    stream.width(oldwidth);
    stream.fill(oldfill);

    return stream;
}
