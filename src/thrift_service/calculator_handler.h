#pragma once

#include <boost/noncopyable.hpp>

#include "gen-cpp2/CalculatorService.h"

namespace w {

class CalculatorHandler : public calculator::CalculatorServiceSvIf,
                          private boost::noncopyable {
public:
  void ping() override;
  void async_tm_calculate(
      std::unique_ptr<apache::thrift::HandlerCallback<int32_t>> callback,
      int32_t logid, const ::calculator::Work &work) override;
};

} // namespace w
