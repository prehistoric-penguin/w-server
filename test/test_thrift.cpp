#include <atomic>
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

std::atomic<size_t> gRequestSentNum{0};
std::atomic<size_t> gResponseRecvNum{0};
std::atomic<size_t> gRequestErrNum{0};

class PingCallback : public RequestCallback {
public:
  void requestSent() override {
    gRequestSentNum.fetch_add(1, std::memory_order_release);
  }

  void replyReceived(ClientReceiveState &&) override {
    gResponseRecvNum.fetch_add(1, std::memory_order_release);
  }

  void requestError(ClientReceiveState &&) override {
    gRequestErrNum.fetch_add(1, std::memory_order_release);
  }
};

class AddCallback : public RequestCallback {
public:
  AddCallback(const calculator::Work &w,
              calculator::CalculatorServiceAsyncClient &client)
      : work_(w), client_(client) {}

  void requestSent() override {
    gRequestSentNum.fetch_add(1, std::memory_order_release);
  }

  void replyReceived(ClientReceiveState &&state) override {
    gResponseRecvNum.fetch_add(1, std::memory_order_release);

    int calc_res = client_.recv_calculate(state);
    assert(calc_res == work_.num1 + work_.num2);
  }

  void requestError(ClientReceiveState &&) override {
    gRequestErrNum.fetch_add(1, std::memory_order_release);
  }

private:
  calculator::Work work_;
  calculator::CalculatorServiceAsyncClient &client_;
};

class SubtractCallback : public RequestCallback {
public:
  void requestSent() override {
    gRequestSentNum.fetch_add(1, std::memory_order_release);
  }

  void replyReceived(ClientReceiveState &&) override {
    gResponseRecvNum.fetch_add(1, std::memory_order_release);
  }

  void requestError(ClientReceiveState &&) override {
    gRequestErrNum.fetch_add(1, std::memory_order_release);
  }
};

class MultiplyCallback : public RequestCallback {
public:
  void requestSent() override {
    gRequestSentNum.fetch_add(1, std::memory_order_release);
  }

  void replyReceived(ClientReceiveState &&) override {
    gResponseRecvNum.fetch_add(1, std::memory_order_release);
  }

  void requestError(ClientReceiveState &&) override {
    gRequestErrNum.fetch_add(1, std::memory_order_release);
  }
};

class DivideCallback : public RequestCallback {
public:
  void requestSent() override {
    gRequestSentNum.fetch_add(1, std::memory_order_release);
  }

  void replyReceived(ClientReceiveState &&) override {
    gResponseRecvNum.fetch_add(1, std::memory_order_release);
  }

  void requestError(ClientReceiveState &&) override {
    gRequestErrNum.fetch_add(1, std::memory_order_release);
  }
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
    client.calculate(folly::make_unique<AddCallback>(w, client), id, w);
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
    client.calculate(folly::make_unique<DivideCallback>(), id, w);
    break;
  default:
    LOG(FATAL) << "unreachable";
  }
}

int main(int argc, char *argv[]) {
  google::SetUsageMessage(std::string("\nUSAGE:\n") + std::string(argv[0]) +
                          " [OPTIONS]...");
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_log_dir = ".";

  google::InitGoogleLogging(argv[0]);

  folly::EventBase eb;
  calculator::CalculatorServiceAsyncClient client(
      HeaderClientChannel::newChannel(async::TAsyncSocket::newSocket(
          &eb, {FLAGS_srv_host, static_cast<uint16_t>(FLAGS_srv_port)})));

  constexpr int kMax = 1e6;

  std::thread thr([&]() { eb.loopForever(); });
  thr.detach();

  for (int i = 1; i < kMax; ++i) {
    try {
      eb.runInEventBaseThread([&, i]() { random_request(client, i); });
    } catch (TException &ex) {
      std::cout << "  Thrift error: " << ex.what() << std::endl;
    } catch (std::exception &ex) {
      std::cout << "  Unexpect error: " << ex.what() << std::endl;
    }
    if (i % static_cast<int>(1e3) == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  eb.runInEventBaseThreadAndWait([&]() { eb.terminateLoopSoon(); });

  std::cout << "sent :" << gRequestSentNum.load(std::memory_order_relaxed)
            << ", resp :" << gResponseRecvNum.load(std::memory_order_relaxed)
            << ", err  :" << gRequestErrNum.load(std::memory_order_relaxed)
            << std::endl;

  if (gRequestSentNum.load(std::memory_order_relaxed) ==
      gResponseRecvNum.load(std::memory_order_relaxed)) {
    std::cout << "TEST PASSED" << std::endl;
  }

  return 0;
}
