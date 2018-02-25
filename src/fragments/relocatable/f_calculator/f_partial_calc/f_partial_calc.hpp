#ifndef __PROCALC_FRAGMENTS_F_PARTIAL_CALC_HPP__
#define __PROCALC_FRAGMENTS_F_PARTIAL_CALC_HPP__


#include <QObject>
#include "fragment.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/setup_complete_event.hpp"


struct FPartialCalcData : public FragmentData {};

class FPartialCalc : public QObject, public Fragment {
  Q_OBJECT

  public:
    FPartialCalc(Fragment& parent, FragmentData& parentData,
      const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FPartialCalc() override;

  private slots:
    void onButtonClick(int id);

  private:
    void toggleFeatures(const std::set<buttonId_t>& features);

    FPartialCalcData m_data;

    int m_eventIdx = -1;
};


#endif
