#ifndef __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__
#define __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__


#include <string>
#include <memory>
#include <map>
#include <random>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QImage>
#include <QTimer>
#include <QLabel>
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

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FShuffledCalc() override;

  public slots:
    void onButtonClick(int id);

  private:
    QString translateToSymbols(const QString& str) const;

    FShuffledCalcData m_data;

    struct {
      QWidget* centralWidget;
    } m_origParentState;

    Calculator m_calculator;
    std::unique_ptr<QVBoxLayout> m_vbox;
    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;
    EventSystem* m_eventSystem;
    UpdateLoop* m_updateLoop;

    std::string m_targetValue;

    std::map<QChar, QChar> m_symbols;

    std::unique_ptr<QLabel> m_glitchOverlay;
    std::unique_ptr<QImage> m_glitchBuffer;
    std::unique_ptr<QTimer> m_glitchTimer;
    std::mt19937 m_randEngine;
};


#endif
