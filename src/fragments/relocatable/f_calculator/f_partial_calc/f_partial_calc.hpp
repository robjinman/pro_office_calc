#ifndef __PROCALC_FRAGMENTS_F_PARTIAL_CALC_HPP__
#define __PROCALC_FRAGMENTS_F_PARTIAL_CALC_HPP__


#include <set>
#include <QObject>
#include "fragment.hpp"
#include "event_system.hpp"
#include "button_grid.hpp"


struct FPartialCalcData : public FragmentData {};

class DialogClosedEvent;

class FPartialCalc : public QObject, public Fragment {
  Q_OBJECT

  public:
    FPartialCalc(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FPartialCalc() override;

  private slots:
    void onButtonClick(int id);
    void onButtonPress(int id);

  private:
    void toggleFeatures(const std::set<buttonId_t>& features);

    FPartialCalcData m_data;

    pEventHandle_t m_hSetupComplete;
    pEventHandle_t m_hDialogClosed;
};


#endif
