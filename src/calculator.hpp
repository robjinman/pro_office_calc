#ifndef __PROCALC_CALCULATOR_HPP__
#define __PROCALC_CALCULATOR_HPP__


#include <deque>
#include <string>


enum operator_t {
  OP_NONE,
  OP_PLUS,
  OP_MINUS,
  OP_TIMES,
  OP_DIVIDE
};

class Calculator {
  public:
    void putDigit(int d);
    void putOperator(operator_t op);
    void putDecimalPoint();
    void clear();
    void evaluate();
    bool hasPending() const;
    float currentValue() const;

    struct Expr {
      float lhs;
      operator_t op;
    };

  private:
    std::string m_buffer;
    std::deque<Expr> m_stack;

    void collapseStack();
};


#endif
