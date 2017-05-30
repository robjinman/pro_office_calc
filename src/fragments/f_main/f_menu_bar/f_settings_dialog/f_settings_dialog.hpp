#ifndef __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_HPP__
#define __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_HPP__


#include <memory>
#include <QDialog>
#include <QAction>
#include <QVBoxLayout>
#include "fragment.hpp"


struct FSettingsDialogData : public FragmentData {
  std::unique_ptr<QAction> actSettings;
  std::unique_ptr<QVBoxLayout> vbox;
};

class FSettingsDialog : public QDialog, public Fragment {
  Q_OBJECT

  public:
    FSettingsDialog(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private slots:
    void showSettingsDialog();

  private:
    FSettingsDialogData m_data;
};


#endif
