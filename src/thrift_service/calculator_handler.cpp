#include "calculator_handler.h"

#include <glog/logging.h>

#include "monitor/stats.h"
#include "gen-cpp2/calculator_types.h"

namespace w {

using namespace calculator;

void CalculatorHandler::async_tm_calculate(
    std::unique_ptr<apache::thrift::HandlerCallback<int32_t>> callback,
    int32_t logid, const ::calculator::Work &work) {
  LOG(INFO) << "processing logid:" << logid;

  int32_t val = 0;
  switch (work.op) {
  case Operation::ADD:
    val = work.num1 + work.num2;
    Marker("op_add");
    break;
  case Operation::SUBTRACT:
    val = work.num1 - work.num2;
    Marker("op_sub");
    break;
  case Operation::MULTIPLY:
    val = work.num1 * work.num2;
    Marker("op_mul");
    break;
  case Operation::DIVIDE:
    if (work.num2 == 0) {
      InvalidOperation io;
      io.op = work.op;
      io.why = "Cannot divide by 0";
      Marker("op_div.err");
      throw io;
    }
    Marker("op_div.fin");
    val = work.num1 / work.num2;
    break;
  default:
    InvalidOperation io;
    io.op = work.op;
    io.why = "Invalid Operation";
    Marker("op_invalid");
    throw io;
  }
  callback->result(val);
}

} // namespace w
