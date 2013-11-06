#ifndef HTTP_SERVER_HEADER_HPP
#define HTTP_SERVER_HEADER_HPP

#include <string>

namespace http {
namespace server {

struct header
{
  std::string name;
  std::string value;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER2_HEADER_HPP
