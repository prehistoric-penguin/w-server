namespace cpp2 calculator

enum Operation {
  ADD = 1,
  SUBTRACT = 2,
  MULTIPLY = 3,
  DIVIDE = 4
}

struct Work {
  1: i32 num1 = 0,
  2: i32 num2,
  3: Operation op,
  4: optional string comment,
}

exception InvalidOperation {
  1: Operation op,
  2: string why
}

service CalculatorService {
   void ping(),
   i32 calculate(1:i32 logid, 2:Work w) throws (1:InvalidOperation ouch),
}
