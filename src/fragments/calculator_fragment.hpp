#ifndef __PROCALC_FRAGMENTS_CALCULATOR_FRAGMENT_HPP__
#define __PROCALC_FRAGMENTS_CALCULATOR_FRAGMENT_HPP__


#include <memory>
#include <QLineEdit>
#include "button_grid.hpp"
#include "fragments/normal_calculator_fragment.hpp"
#include "fragments/shuffled_keys_fragment.hpp"


class QMenuBar;
class QAction;
class EventSystem;
class UpdateLoop;
class CalculatorFragmentSpec;

class CalculatorFragment : public QWidget {
  Q_OBJECT

  public:
    CalculatorFragment(QWidget* parent, QMenuBar& menuBar, QAction& actAbout,
      EventSystem& eventSystem, UpdateLoop& updateLoop);

    void rebuild(const CalculatorFragmentSpec& spec);

  private slots:
    void onButtonClick(int id);

  private:
    QMenuBar& m_menuBar;
    QAction& m_actAbout;
    EventSystem& m_eventSystem;
    UpdateLoop& m_updateLoop;

    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;

    std::unique_ptr<NormalCalculatorFragment> m_normalCalcFragment;
    std::unique_ptr<ShuffledKeysFragment> m_shuffledKeysFragment;
};


#endif
