#ifndef HTTP_SERVER_CONNECTION_HPP
#define HTTP_SERVER_CONNECTION_HPP

#include <fstream>
#include <boost/asio.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  explicit connection(boost::asio::io_service& io_service, request_handler& handler);
  boost::asio::ip::tcp::socket& socket();
  void start();

private:
  void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& e);
  void start_file_read();
  void handle_write_file(const boost::system::error_code& err);
  void close();
  boost::asio::ip::tcp::socket socket_;
  request_handler& request_handler_;
  boost::array<char, 8192> buffer_;
  boost::array<char, 2048> file_buffer_;
  request request_;
  request_parser request_parser_;
  reply reply_;
  std::ifstream file_;
  std::string full_file_path_;
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_SERVER_CONNECTION_HPP
