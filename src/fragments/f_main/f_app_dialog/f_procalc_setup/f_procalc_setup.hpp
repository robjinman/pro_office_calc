#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_HPP__


#include <map>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QStackedLayout>
#include <QMargins>
#include "fragment.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"
#include "raycast/raycast_widget.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/game_logic.hpp"
#include "qt_obj_ptr.hpp"
#include "button_grid.hpp"


struct FProcalcSetupData : public FragmentData {
  QtObjPtr<QStackedLayout> stackedLayout;

  struct {
    QtObjPtr<QWidget> widget;
    QtObjPtr<QListWidget> wgtList;
    QtObjPtr<QPushButton> wgtNext;
  } page1;

  struct {
    QtObjPtr<QWidget> widget;
    QtObjPtr<RaycastWidget> wgtRaycast;
    std::unique_ptr<making_progress::GameLogic> gameLogic;
  } page2;
};

class FProcalcSetup : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FProcalcSetup(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FProcalcSetup() override;

  private slots:
    void onNextClick();

  private:
    FProcalcSetupData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    std::map<buttonId_t, int> m_featureIndices;

    void setupPage1();
    void setupPage2();
    void populateListWidget();
    void addCheckableItem(QListWidget& wgtList, const QString& text, buttonId_t btnId = BTN_NULL);
};


#endif
