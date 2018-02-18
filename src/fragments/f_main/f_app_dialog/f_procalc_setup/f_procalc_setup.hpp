#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_HPP__


#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMargins>
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "utils.hpp"


struct FProcalcSetupData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QListWidget> wgtList;
  QtObjPtr<QPushButton> wgtNext;
};

class FProcalcSetup : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FProcalcSetup(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FProcalcSetup() override;

  private:
    FProcalcSetupData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
