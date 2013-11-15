## Introduction

The goal of that project is to provide a research on network server programming approaches to solve the c10k problem (c100k...cNk as well). The programming approaches we will inspect:

* Native thread per socket 
  (synchronous handling)
* State machine based on epoll 
  (native thread per core, asynchronous handling)
* Green thread per socket (codenames: Coroutine/Fiber) 
  (native thread per core, synchronous handling (asynchronous under the hood))

## Dependencies

* gcc 4.7 ++ (or anything else that support rudimentary C++11)
* boost 1.49
* autoconf and automake

## Building

1. Run `autoreconf -i` to create the configure script.
2. Optional: Create a directory for the specific variant, e.g.: `mkdir build_noaffinity`. Change into it.
3. Run configure from the source dir. For the example in 2 above, run `../configure --disable-affinity`.
4. `make`.
    
## Environment

### Sysctl:

    net.ipv4.tcp_tw_recycle = 1
    net.ipv4.tcp_tw_reuse = 1
    net.ipv4.tcp_fin_timeout = 1
    net.ipv4.tcp_timestamps = 1
    net.ipv4.tcp_syncookies = 0
    net.ipv4.ip_local_port_range = 1024 65535
    kernel.pid_max = 4194303
    kernel.threads-max = 4194303
    vm.max_map_count = 4194303

### change the maximum of open files:

    $ ulimit -n 200000


## License

Licensed under the terms and conditions of the GNU General Public License (GPL) version 2, or, at your discretion, any later version.

## Authors

Uri Shamay (shamayuri@gmail.com)

Shachar Shemesh (shachar@shemesh.biz)
