#ifndef __PROCALC_CALCULATOR_HPP__
#define __PROCALC_CALCULATOR_HPP__


#include <stack>
#include <string>


class Calculator {
  public:
    void number(int n);
    void point();
    void plus();
    void times();
    void divide();
    void minus();
    double equals();
    void clear();

    const std::string& display() const;

    enum operator_t {
      OP_NONE,
      OP_PLUS,
      OP_MINUS,
      OP_TIMES,
      OP_DIVIDE
    };

  private:
    class CalcStack {
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
        std::stack<Expr> m_stack;

        double apply(Expr, double rhs) const;
        void collapseStack();
    };

    CalcStack m_calcStack;
    std::string m_display;
    bool m_reset;
};


#endif
