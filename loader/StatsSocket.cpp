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

size_t StatsSocket::write( const char *buffer, size_t len )
{
    ssize_t ret = ::write( m_fd, buffer, len );
    if( ret<0 )
        // Even if EAGAIN, due to YAGNI
        throw std::system_error( errno, std::system_category(), "Socket write failed" );

    return ret;
}

size_t StatsSocket::read()
{
    assert( m_bufferFill>=m_bufferConsumed );

    // We have unconsumed data in the buffer
    if( m_bufferFill>m_bufferConsumed )
        return m_bufferConsumed-m_bufferFill;

    assert( m_bufferFill==0 );

    ssize_t ret = ::read( m_fd, m_buffer, BUFFER_SIZE );
    if( ret<0 ) {
        if( errno!=EAGAIN )
            throw std::system_error( errno, std::system_category(), "Socket read failed" );

        // errno = EAGAIN
        return 0;
    }

    if( ret==0 )
        throw EOFException();

    m_bufferFill += ret;

    return ret;
}

const char *StatsSocket::getBuffer() const
{
    return m_buffer + m_bufferFill;
}

void StatsSocket::consume( size_t bytes )
{
    assert( bytes<=( m_bufferFill - m_bufferConsumed ) );

    m_bufferConsumed += bytes;
    
    if( m_bufferConsumed==m_bufferFill ) {
        m_bufferFill = m_bufferConsumed = 0;
    }
}
