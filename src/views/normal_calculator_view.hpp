#ifndef __PROCALC_NORMAL_CALCULATOR_VIEW_HPP__
#define __PROCALC_NORMAL_CALCULATOR_VIEW_HPP__


class NormalCalculatorView : public View {
  public:
    NormalCalculatorView(EventSystem& eventSystem);

    virtual void setup(int rootState) override;
};


#endif
