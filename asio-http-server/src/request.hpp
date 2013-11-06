#ifndef HTTP_SERVER_REQUEST_HPP
#define HTTP_SERVER_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
namespace server {

struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_REQUEST_HPP
