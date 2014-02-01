/*
    Loader - application for generating benchmarking load on a web server
    Copyright (C) 2014 Shachar Shemesh, Uri Shamay

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include "StatsSocket.h"
#include "time.h"

#include <assert.h>

StatsSocket::StatsSocket( TaskStats *taskStats, const sockaddr *addr, socklen_t addrlen ) :
    m_fd( socket(addr->sa_family, SOCK_STREAM, 0), "Socket creation failed" ),
    m_startTime(Time::now()), m_taskStats(taskStats),
    m_bufferFill(0), m_bufferConsumed(0)
{
    if( fcntl( m_fd, F_SETFL, O_NONBLOCK )<0 )
        throw std::system_error( errno, std::system_category(), "Setting non-block failed" );

    if( connect( m_fd, addr, addrlen )<0 )
        throw std::system_error( errno, std::system_category(), "Connection failed" );
}

#if 0
size_t StatsSocket::write( const char *buffer, size_t len );

size_t StatsSocket::read();

const char *StatsSocket::getBuffer() const;
void StatsSocket::consume( size_t bytes );
#endif
