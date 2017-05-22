#ifndef __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_HPP__
#define __PROCALC_FRAGMENTS_F_NORMAL_CALC_TRIGGER_HPP__


#include <QObject>
#include "fragment.hpp"


struct FNormalCalcTriggerData : public FragmentData {};

class FNormalCalcTrigger : public QObject, public Fragment {
  Q_OBJECT

  public:
    FNormalCalcTrigger(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private slots:
    void onButtonClick(int id);

  private:
    FNormalCalcTriggerData m_data;

    QColor m_targetWindowColour;
    QColor m_targetDisplayColour;
    QString m_symbols;
};


#endif
