#ifndef __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__
#define __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__


#include <string>
#include <memory>
#include <map>
#include <QWidget>
#include <QLineEdit>
#include "fragment.hpp"
#include "button_grid.hpp"
#include "calculator.hpp"


class QMainWindow;
class EventSystem;
class UpdateLoop;

struct FShuffledCalcData : public FragmentData {};

class FShuffledCalc : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FShuffledCalc(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  public slots:
    void onButtonClick(int id);

  private:
    QString translateToSymbols(const QString& str) const;

    FShuffledCalcData m_data;

    Calculator m_calculator;
    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;
    EventSystem* m_eventSystem;
    UpdateLoop* m_updateLoop;
    QMainWindow* m_window;

    std::string m_targetValue;

    std::map<QChar, QChar> m_symbols;
};


#endif
