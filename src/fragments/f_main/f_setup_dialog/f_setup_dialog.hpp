#ifndef __PROCALC_FRAGMENTS_F_SETUP_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_SETUP_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include "fragment.hpp"


struct FSetupDialogData : public FragmentData {

};

class FSetupDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FSetupDialog(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FSetupDialog() override;

  private:
    FSetupDialogData m_data;

    int m_eventIdx;
};


#endif
