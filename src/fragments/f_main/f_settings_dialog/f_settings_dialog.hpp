#ifndef __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_HPP__


#include <QDialog>
#include <QAction>
#include <QVBoxLayout>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FSettingsDialogData : public FragmentData {
  QtObjPtr<QAction> actSettings;
  QtObjPtr<QVBoxLayout> vbox;
};

class FSettingsDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FSettingsDialog(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FSettingsDialog() override;

  private slots:
    void showSettingsDialog();

  private:
    FSettingsDialogData m_data;
};


#endif
