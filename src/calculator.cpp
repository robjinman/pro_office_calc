#include <map>
#include <sstream>
#include <iomanip>
#include <cassert>
#include "calculator.hpp"
#include "exception.hpp"


using std::string;
using std::stringstream;


const std::map<Calculator::OpStack::operator_t, int> Calculator::OpStack::PRECEDENCE {
  { Calculator::OpStack::OP_NONE, 0 },
  { Calculator::OpStack::OP_PLUS, 1 },
  { Calculator::OpStack::OP_MINUS, 1 },
  { Calculator::OpStack::OP_TIMES, 2 },
  { Calculator::OpStack::OP_DIVIDE, 3 }
};


//===========================================
// formatNumber
//===========================================
static string formatNumber(double num) {
  stringstream ss;
  ss << std::fixed << std::setprecision(8) << num;
  string str = ss.str();

  if (str.find('.') != string::npos) {
    while (str.back() == '0') {
      str.pop_back();
    }

    if (str.back() == '.') {
      str.pop_back();
    }
  }

  return str;
}

//===========================================
// Calculator::OpStack::dbg_print
//===========================================
#ifdef DEBUG
void Calculator::OpStack::dbg_print(std::ostream& os) const {
  std::stack<Expr> stack = m_stack;

  os << "OpStack\n";
  while (!stack.empty()) {
    Expr e = stack.top();
    stack.pop();
    os << e.lhs << ", " << e.op << "\n";
  }
}
#endif

//===========================================
// Calculator::OpStack::apply
//===========================================
double Calculator::OpStack::apply(Expr expr, double rhs) const {
  switch (expr.op) {
    case OP_PLUS: return expr.lhs + rhs;
    case OP_MINUS: return expr.lhs - rhs;
    case OP_TIMES: return expr.lhs * rhs;
    case OP_DIVIDE: return expr.lhs / rhs;
    default: EXCEPTION("Unrecognised operator");
  }
}

//===========================================
// Calculator::OpStack::putValue
//===========================================
void Calculator::OpStack::putValue(double val) {
  if (m_stack.size() > 0 && m_stack.top().op == OP_NONE) {
    m_stack.top().lhs = val;
  }
  else {
    m_stack.push(Expr{val, OP_NONE});
  }
}

//===========================================
// Calculator::OpStack::putOperator
//===========================================
void Calculator::OpStack::putOperator(operator_t op) {
  if (m_stack.empty()) {
    m_stack.push(Expr{0, OP_NONE});
  }

  m_stack.top().op = op;

  collapseStack();
}

//===========================================
// Calculator::OpStack::collapseStack
//===========================================
void Calculator::OpStack::collapseStack() {
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
// Calculator::OpStack::evaluate
//===========================================
double Calculator::OpStack::evaluate() {
  if (m_stack.empty()) {
    return 0;
  }

  m_stack.top().op = OP_NONE;

  collapseStack();
  assert(m_stack.size() == 1);

  return m_stack.top().lhs;
}

//===========================================
// Calculator::OpStack::clear
//===========================================
void Calculator::OpStack::clear() {
  m_stack = std::stack<Expr>();
}

//===========================================
// Calculator::OpStack::op
//===========================================
Calculator::OpStack::operator_t Calculator::OpStack::op() const {
  return m_stack.size() > 0 ? m_stack.top().op : OP_NONE;
}

//===========================================
// Calculator::number
//===========================================
void Calculator::number(int n) {
  if (n < 0 || n > 9) {
    EXCEPTION("Calculator expects single digit");
  }

  if (m_reset || m_OpStack.op() != OpStack::OP_NONE) {
    m_display = "";
    m_reset = false;
  }

  m_display.append(std::to_string(n).c_str());

  double value = std::strtod(m_display.c_str(), nullptr);
  m_OpStack.putValue(value);
}

//===========================================
// Calculator::point
//===========================================
void Calculator::point() {
  if (m_display.find('.') == string::npos) {
    m_display.append(".");
  }
}

//===========================================
// Calculator::plus
//===========================================
void Calculator::plus() {
  m_OpStack.putOperator(OpStack::OP_PLUS);
}

//===========================================
// Calculator::times
//===========================================
void Calculator::times() {
  m_OpStack.putOperator(OpStack::OP_TIMES);
}

//===========================================
// Calculator::divide
//===========================================
void Calculator::divide() {
  m_OpStack.putOperator(OpStack::OP_DIVIDE);
}

//===========================================
// Calculator::minus
//===========================================
void Calculator::minus() {
  m_OpStack.putOperator(OpStack::OP_MINUS);
}

//===========================================
// Calculator::equals
//===========================================
double Calculator::equals() {
  double result = m_OpStack.evaluate();
  m_display = formatNumber(result).c_str();
  m_reset = true;

  return result;
}

//===========================================
// Calculator::clear
//===========================================
void Calculator::clear() {
  m_display = "";
}

//===========================================
// Calculator::display
//===========================================
const std::string& Calculator::display() const {
  return m_display;
}
