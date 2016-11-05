#include <random>
#include <stdexcept>

#include <gflags/gflags.h>
#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>

#include "gen-cpp2/CalculatorService.h"
using namespace apache::thrift;

DEFINE_int32(srv_port, 12307, "thrift service listen port");
DEFINE_string(srv_host, "127.0.0.1", "thrift service host");

using random_t = std::mt19937;

enum operation_t {
  PING = 0,
  ADD = 1,
  SUBTRACT = 2,
  MULTIPLY = 3,
  DIVIDE = 4,
  OPERATION_NUM
};

size_t gRequestSentNum = 0;
size_t gResponseRecvNum = 0;
size_t gRequestErrNum = 0;

class PingCallback : public RequestCallback {
public:
  void requestSent() override { ++gRequestSentNum; }

  void replyReceived(ClientReceiveState &&) override { ++gResponseRecvNum; }

  void requestError(ClientReceiveState &&) override { ++gRequestErrNum; }
};

class AddCallback : public RequestCallback {
public:
  void requestSent() override { ++gRequestSentNum; }

  void replyReceived(ClientReceiveState &&) override { ++gResponseRecvNum; }

  void requestError(ClientReceiveState &&) override { ++gRequestErrNum; }
};

class SubtractCallback : public RequestCallback {
public:
  void requestSent() override { ++gRequestSentNum; }

  void replyReceived(ClientReceiveState &&) override { ++gResponseRecvNum; }

  void requestError(ClientReceiveState &&) override { ++gRequestErrNum; }
};

class MultiplyCallback : public RequestCallback {
public:
  void requestSent() override { ++gRequestSentNum; }

  void replyReceived(ClientReceiveState &&) override { ++gResponseRecvNum; }

  void requestError(ClientReceiveState &&) override { ++gRequestErrNum; }
};

class DiviceCallback : public RequestCallback {
public:
  void requestSent() override { ++gRequestSentNum; }

  void replyReceived(ClientReceiveState &&) override { ++gResponseRecvNum; }

  void requestError(ClientReceiveState &&) override { ++gRequestErrNum; }
};

random_t gRng(5);
void random_request(calculator::CalculatorServiceAsyncClient &client, int id) {
  auto rand_op = gRng() % OPERATION_NUM;
  calculator::Work w;
  w.num1 = id;
  w.num2 = id;

  switch (rand_op) {
  case PING:
    client.ping(folly::make_unique<PingCallback>());
    break;
  case ADD:
    w.op = calculator::Operation::ADD;
    client.calculate(folly::make_unique<AddCallback>(), id, w);
    break;
  case SUBTRACT:
    w.op = calculator::Operation::SUBTRACT;
    client.calculate(folly::make_unique<SubtractCallback>(), id, w);
    break;
  case MULTIPLY:
    w.op = calculator::Operation::MULTIPLY;
    client.calculate(folly::make_unique<MultiplyCallback>(), id, w);
    break;
  case DIVIDE:
    w.op = calculator::Operation::DIVIDE;
    client.calculate(folly::make_unique<DiviceCallback>(), id, w);
    break;
  default:
    LOG(FATAL) << "unreachable";
  }
}

int main(int argc, char *argv[]) {
  google::SetUsageMessage(std::string("\nUSAGE:\n") + std::string(argv[0]) +
                          " [OPTIONS]...");
  google::ParseCommandLineFlags(&argc, &argv, true);

  folly::EventBase eb;
  calculator::CalculatorServiceAsyncClient client(
      HeaderClientChannel::newChannel(async::TAsyncSocket::newSocket(
          &eb, {FLAGS_srv_host, static_cast<uint16_t>(FLAGS_srv_port)})));

  constexpr int kMax = 1e8;

  std::thread thr([&]() { eb.loopForever(); });
  thr.detach();

  for (int i = 0; i < kMax; ++i) {
    try {
      eb.runInEventBaseThread([&](){  random_request(client, i); });
    } catch (TException &ex) {
      std::cout << "  Thrift error: " << ex.what() << std::endl;
    } catch (std::exception &ex) {
      std::cout << "  Unexpect error: " << ex.what() << std::endl;
    }
    if (i % static_cast<int>(1e3) == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }

  eb.runInEventBaseThreadAndWait([&]() { eb.terminateLoopSoon(); });

  std::cout << "sent :" << gRequestSentNum
      << ", resp :" << gResponseRecvNum
      << ", err  :" << gRequestErrNum << std::endl;

  return 0;
}
