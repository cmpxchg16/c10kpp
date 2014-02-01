/*
    Fakeroot Next Generation - run command with fake root privileges
    Copyright (C) 2013-2014 Shachar Shemesh

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

#ifndef UNIQUE_FD_H
#define UNIQUE_FD_H

#include <sys/file.h>
#include <unistd.h>
#include <system_error>

struct unique_fd {
    int _fd;

    unique_fd( const unique_fd &rhs )=delete;
    unique_fd & operator=( const unique_fd &rhs )=delete;
public:
    explicit unique_fd( int fd=-1, const char *exception_message=nullptr ) : _fd( fd>=0 ? fd : -1 )
    {
        if( _fd<0 && exception_message )
            throw std::system_error( errno, std::system_category(), exception_message );
    }

    // Movers
    explicit unique_fd( unique_fd &&rhs )
    {
        _fd=rhs._fd;
        rhs._fd=-1;
    }
    unique_fd & operator=( unique_fd &&rhs )
    {
        _fd=rhs._fd;
        rhs._fd=-1;

        return *this;
    }

    // Destructor
    ~unique_fd()
    {
        if( _fd>=0 )
            if( close(_fd)<0 )
                throw std::system_error( errno, std::system_category(), "Close failed" );
    }

    int get() const { return _fd; }
    int release()
    {
        int ret=get();
        _fd=-1;
        return ret;
    }

    operator bool() const { return _fd>=0; }

    // File related operations
    bool flock( int operation )
    {
        if( ::flock( get(), operation )<0 ) {
            if( errno==EWOULDBLOCK )
                return false;

            throw std::system_error( errno, std::system_category(), "flock failed" );
        }

        return true;
    }
};

#endif // UNIQUE_FD_H
