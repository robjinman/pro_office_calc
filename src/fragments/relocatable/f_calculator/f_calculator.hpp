#ifndef __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__
#define __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__


#include <memory>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QMargins>
#include "fragment.hpp"
#include "button_grid.hpp"
#include "calculator.hpp"


class QMainWindow;

struct FCalculatorData : public FragmentData {
  Calculator calculator;
  std::unique_ptr<QVBoxLayout> vbox;
  std::unique_ptr<QLineEdit> wgtDigitDisplay;
  std::unique_ptr<ButtonGrid> wgtButtonGrid;
};

class FCalculator : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FCalculator(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FCalculator() override;

  public slots:
    void onButtonClick(int id);

  private:
    FCalculatorData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
