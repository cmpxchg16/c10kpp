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

#include "time.h"

#include <unistd.h>

main()
{
    Time start = Time::now();
    usleep(1500000);
    Time end = Time::now();

    std::cout<<"From "<<start<<" to "<<end<<" elapsed "<<end-start<<std::endl;
}
