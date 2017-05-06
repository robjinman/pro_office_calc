#include <map>
#include <cassert>
#include "calculator.hpp"
#include "exception.hpp"


static const std::map<operator_t, int> PRECEDENCE {
  { OP_NONE, 0 },
  { OP_PLUS, 1 },
  { OP_MINUS, 1 },
  { OP_TIMES, 2 },
  { OP_DIVIDE, 2 }
};


//===========================================
// apply
//===========================================
static float apply(Calculator::Expr expr, float rhs) {
  switch (expr.op) {
    case OP_PLUS: return expr.lhs + rhs;
    case OP_MINUS: return expr.lhs - rhs;
    case OP_TIMES: return expr.lhs * rhs;
    case OP_DIVIDE: return expr.lhs / rhs;
    default: EXCEPTION("Unrecognised operator\n");
  }
}

//===========================================
// Calculator::putDigit
//===========================================
void Calculator::putDigit(int d) {
  m_buffer.append(std::to_string(d));
}

//===========================================
// Calculator::putDecimalPoint
//===========================================
void Calculator::putDecimalPoint() {
  m_buffer.append(".");
}

//===========================================
// Calculator::putOperator
//===========================================
void Calculator::putOperator(operator_t op) {
  float number = strtof(m_buffer.c_str(), nullptr);

  m_stack.push_back(Expr{number, op});
  collapseStack();

  m_buffer = "";
}

//===========================================
// Calculator::collapseStack
//===========================================
void Calculator::collapseStack() {
  while (m_stack.size() > 1) {
    Expr curr = m_stack.back();
    Expr prev = m_stack[m_stack.size() - 2];

    if (PRECEDENCE.at(curr.op) <= PRECEDENCE.at(prev.op)) {
      m_stack.pop_back();
      m_stack.pop_back();

      float val = apply(prev, curr.lhs);

      m_stack.push_back(Expr{val, curr.op});
    }
    else {
      break;
    }
  }
}

//===========================================
// Calculator::evaluate
//===========================================
void Calculator::evaluate() {
  if (m_buffer.length() > 0) {
    float number = strtof(m_buffer.c_str(), nullptr);
    m_stack.push_back(Expr{number, OP_NONE});
  }

  collapseStack();

  if (m_stack.size() > 0) {
    m_buffer = std::to_string(m_stack.back().lhs);
    m_stack.pop_back();
  }

  assert(m_stack.empty());
}

//===========================================
// Calculator::clear
//===========================================
void Calculator::clear() {
  m_stack.clear();
  m_buffer = "";
}

//===========================================
// Calculator::currentValue
//===========================================
float Calculator::currentValue() const {
  if (m_buffer.length() > 0) {
    return strtof(m_buffer.c_str(), nullptr);
  }
  else if (m_stack.size() > 0) {
    return m_stack.front().lhs;
  }

  return 0;
}
