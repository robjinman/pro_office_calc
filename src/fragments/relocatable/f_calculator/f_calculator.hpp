#ifndef __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__
#define __PROCALC_FRAGMENTS_F_CALCULATOR_HPP__


#include <QMargins>
#include "fragment.hpp"
#include "calculator_widget.hpp"
#include "qt_obj_ptr.hpp"


class QMainWindow;

struct FCalculatorData : public FragmentData {
  QtObjPtr<CalculatorWidget> wgtCalculator;
};

class FCalculator : public Fragment {
  public:
    FCalculator(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FCalculator() override;

  private:
    FCalculatorData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
