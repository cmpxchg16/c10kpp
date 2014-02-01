#ifndef STATS_SOCKET_H
#define STATS_SOCKET_H

#include "time.h"

#include "unique_fd.h"

#include <sys/types.h>
#include <sys/socket.h>

class TaskStats;

class StatsSocket {
    unique_fd m_fd;
    Time m_startTime;

    TaskStats *m_taskStats;

    static const size_t BUFFER_SIZE = 4096;
    char m_buffer[BUFFER_SIZE];
    size_t m_bufferFill, m_bufferConsumed;
public:
    StatsSocket( TaskStats *taskStats, const sockaddr *addr, socklen_t addrlen );

    size_t write( const char *buffer, size_t len );

    size_t read();

    const char *getBuffer() const;
    void consume( size_t bytes );
};

#endif // STATS_SOCKET_H
