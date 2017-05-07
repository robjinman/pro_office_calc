#ifndef __PROCALC_CALCULATOR_HPP__
#define __PROCALC_CALCULATOR_HPP__


#include <deque>


enum operator_t {
  OP_NONE,
  OP_PLUS,
  OP_MINUS,
  OP_TIMES,
  OP_DIVIDE
};

class Calculator {
  public:
    void putValue(double val);
    void putOperator(operator_t op);
    void clear();
    double evaluate();
    operator_t op() const;

    struct Expr {
      double lhs;
      operator_t op;
    };

  private:
    std::deque<Expr> m_stack;

    void collapseStack();
};


#endif
