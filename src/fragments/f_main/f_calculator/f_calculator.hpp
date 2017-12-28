#ifndef __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__
#define __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__


#include <memory>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include "fragment.hpp"
#include "button_grid.hpp"
#include "calculator.hpp"


class QMainWindow;
class EventSystem;
class UpdateLoop;

struct FCalculatorData : public FragmentData {
  Calculator calculator;
  std::unique_ptr<QVBoxLayout> vbox;
  std::unique_ptr<QLineEdit> wgtDigitDisplay;
  std::unique_ptr<ButtonGrid> wgtButtonGrid;
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;
  QMainWindow* window;
};

class FCalculator : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FCalculator(Fragment& parent, FragmentData& parentData);

    virtual void initialise(const FragmentSpec& spec) override;
    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FCalculator() override;

  public slots:
    void onButtonClick(int id);

  private:
    FCalculatorData m_data;

    struct {
      QWidget* centralWidget;
    } m_origParentState;
};


#endif
