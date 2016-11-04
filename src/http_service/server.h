#pragma once

#include <boost/noncopyable.hpp>
#include <proxygen/httpserver/HTTPServer.h>

namespace w {
class HttpServer : private boost::noncopyable {
public:
  HttpServer(int port, int threads, int idletimeout)
      : port_(port), threads_(threads), idletimeout_(idletimeout) {}
  ~HttpServer() = default;

  void AsyncStart();

private:
  int port_;
  int threads_;
  int idletimeout_;
  std::unique_ptr<proxygen::HTTPServer> http_server_;
};
} // namespace w
