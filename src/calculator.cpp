#include <map>
#include <cassert>
#include "calculator.hpp"
#include "exception.hpp"


static const std::map<operator_t, int> PRECEDENCE {
  { OP_NONE, 0 },
  { OP_PLUS, 1 },
  { OP_MINUS, 1 },
  { OP_TIMES, 2 },
  { OP_DIVIDE, 3 }
};


//===========================================
// apply
//===========================================
static double apply(Calculator::Expr expr, double rhs) {
  switch (expr.op) {
    case OP_PLUS: return expr.lhs + rhs;
    case OP_MINUS: return expr.lhs - rhs;
    case OP_TIMES: return expr.lhs * rhs;
    case OP_DIVIDE: return expr.lhs / rhs;
    default: EXCEPTION("Unrecognised operator\n");
  }
}

//===========================================
// Calculator::putValue
//===========================================
void Calculator::putValue(double val) {
  if (m_stack.size() > 0 && m_stack.top().op == OP_NONE) {
    m_stack.top().lhs = val;
  }
  else {
    m_stack.push(Expr{val, OP_NONE});
  }
}

//===========================================
// Calculator::putOperator
//===========================================
void Calculator::putOperator(operator_t op) {
  if (m_stack.empty()) {
    m_stack.push(Expr{0, OP_NONE});
  }

  m_stack.top().op = op;

  collapseStack();
}

//===========================================
// Calculator::collapseStack
//===========================================
void Calculator::collapseStack() {
  while (m_stack.size() > 1) {
    Expr curr = m_stack.top();
    m_stack.pop();
    Expr prev = m_stack.top();
    m_stack.push(curr);

    if (PRECEDENCE.at(curr.op) <= PRECEDENCE.at(prev.op)) {
      m_stack.pop();
      m_stack.pop();

      double val = apply(prev, curr.lhs);

      m_stack.push(Expr{val, curr.op});
    }
    else {
      break;
    }
  }
}

//===========================================
// Calculator::evaluate
//===========================================
double Calculator::evaluate() {
  collapseStack();
  assert(m_stack.size() == 1);

  return m_stack.top().lhs;
}

//===========================================
// Calculator::clear
//===========================================
void Calculator::clear() {
  m_stack = std::stack<Expr>();
}

//===========================================
// Calculator::op
//===========================================
operator_t Calculator::op() const {
  return m_stack.size() > 0 ? m_stack.top().op : OP_NONE;
}
