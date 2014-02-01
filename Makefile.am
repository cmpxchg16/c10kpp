noinst_PROGRAMS = asio-http-server threads-http-server loader/loader

AM_CXXFLAGS = -pthread

asio_http_server_SOURCES = asio/connection.cpp asio/io_service_pool.cpp asio/main.cpp asio/mime_types.cpp \
                           asio/reply.cpp asio/request_handler.cpp asio/request_parser.cpp asio/server.cpp

threads_http_server_SOURCES = threads/server.cpp

loader_loader_SOURCES = loader/time.cpp loader/main.cpp loader/StatsSocket.cpp
