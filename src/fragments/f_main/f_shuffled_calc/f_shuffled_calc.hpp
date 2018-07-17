#ifndef __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__
#define __PROCALC_FRAGMENTS_F_SHUFFLED_CALC_HPP__


#include <string>
#include <map>
#include <random>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QImage>
#include <QTimer>
#include <QMargins>
#include <QLabel>
#include "fragment.hpp"
#include "button_grid.hpp"
#include "calculator.hpp"
#include "qt_obj_ptr.hpp"


class QMainWindow;

struct FShuffledCalcData : public FragmentData {};

class FShuffledCalc : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FShuffledCalc(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FShuffledCalc() override;

  public slots:
    void onButtonClick(int id);

  private:
    QString translateToSymbols(const QString& str) const;

    FShuffledCalcData m_data;

    Calculator m_calculator;
    QtObjPtr<QVBoxLayout> m_vbox;
    QtObjPtr<QLineEdit> m_wgtDigitDisplay;
    QtObjPtr<QLabel> m_wgtOpDisplay;
    QtObjPtr<ButtonGrid> m_wgtButtonGrid;

    std::string m_targetValue;
    std::map<QChar, QChar> m_symbols;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
