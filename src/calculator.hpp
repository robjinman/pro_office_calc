#ifndef __PROCALC_CALCULATOR_HPP__
#define __PROCALC_CALCULATOR_HPP__


#include <stack>
#include <string>
#ifdef DEBUG
#  include <ostream>
#endif


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

  private:
    class OpStack {
      public:
        enum operator_t {
          OP_NONE,
          OP_PLUS,
          OP_MINUS,
          OP_TIMES,
          OP_DIVIDE
        };

        void putValue(double val);
        void putOperator(operator_t op);
        void clear();
        double evaluate();
        operator_t op() const;

#ifdef DEBUG
        void dbg_print(std::ostream& os) const;
#endif

      private:
        struct Expr {
          double lhs;
          operator_t op;
        };

        double apply(Expr, double rhs) const;
        void collapseStack();

        std::stack<Expr> m_stack;

        static const std::map<operator_t, int> PRECEDENCE;
    };

    OpStack m_opStack;
    std::string m_display;
    bool m_reset;
};


#endif
