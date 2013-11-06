## Introduction

The goal of that project is to provide a research on netowrk server programming approaches. The programming approaches we will inspect:

* Native thread per socket 
  (synchronous handling)
* State machine based on epoll 
  (native thread per core, asynchronous handling::Reactor pattern)
* Green thread per socket (codenames: Coroutine/Fiber) 
  (native thread per core, synchronous handling (asynchronous under the hood))

## Dependencies

* gcc 4.7 ++
* boost 1.49

## Building

    make

## License

Licensed under the MIT License.

## Authors

Uri Shamay (shamayuri@gmail.com)

Shachar Shemesh (shachar@shemesh.biz)
