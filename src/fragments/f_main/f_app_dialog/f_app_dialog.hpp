#ifndef __PROCALC_FRAGMENTS_F_APP_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_APP_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"


struct FAppDialogData : public WidgetFragData {
  FAppDialogData()
    : WidgetFragData(new QVBoxLayout) {}
};

class FAppDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FAppDialog(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FAppDialog() override;

  private:
    FAppDialogData m_data;

    int m_eventIdx = -1;
};


#endif
