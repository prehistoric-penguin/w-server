#include <gflags/gflags.h>
#include <glog/logging.h>

#include <unistd.h>

#include <thread>
#include <chrono>
#include <thrift/lib/cpp2/server/ThriftServer.h>

#include "http_service/server.h"
#include "thrift_service/calculator_handler.h"

DEFINE_bool(daemon, false, "Start with daemon mode");

DEFINE_int32(http_port, 12306, "Port to listen on with HTTP protocol");
DEFINE_int32(http_worker_threads, 2, "Http worker threads number");
DEFINE_int32(http_timeout, 10000, "Http request timeout");

DEFINE_int32(thrift_port, 12307, "Port to listen on with thrift service");
DEFINE_int32(thrift_worker_threads, 2, "Thrift worker threads number");
DEFINE_int32(thrift_timeout, 10000, "Thrift request timeout");

void daemon_mode() {
  if (!FLAGS_daemon)
    return;

  if (daemon(1, 1)) {
    LOG(FATAL) << "start daemon mode failed";
    exit(1);
  } else {
    LOG(INFO) << "start daemon mode";
  }
}

int main(int argc, char *argv[]) {
  google::SetUsageMessage(std::string("\nUSAGE:\n") + std::string(argv[0]) +
                          " [OPTIONS]...");
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_log_dir = "./";

  google::InitGoogleLogging(argv[0]);

  daemon_mode();

  // http server
  w::HttpServer http_srv(FLAGS_http_port, FLAGS_http_worker_threads,
                         FLAGS_http_timeout);
  http_srv.AsyncStart();

  // thrift server
  apache::thrift::ThriftServer thrift_srv;
  thrift_srv.setInterface(std::make_shared<w::CalculatorHandler>());
  thrift_srv.setPort(FLAGS_thrift_port);
  thrift_srv.setPoolThreadName("ThriftWorker");
  thrift_srv.setNWorkerThreads(FLAGS_thrift_worker_threads);

  LOG(INFO) << "Start thrift service on port:" << FLAGS_thrift_port
      << ",with worker threads num:" << FLAGS_thrift_worker_threads
      << ",idle time out:" << FLAGS_thrift_timeout;

  thrift_srv.serve();

  LOG(INFO) << "Process shutting down";

  google::ShutdownGoogleLogging();
  return 0;
}
