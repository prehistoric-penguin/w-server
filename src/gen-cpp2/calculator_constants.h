/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#pragma once

#include "calculator_types.h"
#include <thrift/lib/cpp2/protocol/Protocol.h>
namespace calculator {

struct calculator_constants {};

struct __attribute__((__deprecated__("calculator_constants_codemod is a transitional class only intended for codemods from the deprecated calculatorConstants to calculator_constants. Consider switching to the latter as soon as possible."))) calculator_constants_codemod {};

class __attribute__((__deprecated__("calculatorConstants suffers from the 'static initialization order fiasco' (https://isocpp.org/wiki/faq/ctors#static-init-order) and may CRASH you program. Instead, use calculator_constants::CONSTANT_NAME()"))) calculatorConstants {
 public:
  calculatorConstants() {}
};

} // calculator