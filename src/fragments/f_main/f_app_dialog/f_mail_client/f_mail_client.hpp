#ifndef __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__
#define __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__


#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QMargins>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"


struct FMailClientData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QTabWidget> wgtTabs;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QTableWidget> wgtTable;
  } inboxTab;

  struct {
    QtObjPtr<QWidget> page;
    QtObjPtr<QGridLayout> grid;
    QtObjPtr<QTextBrowser> wgtText;
    QtObjPtr<QLabel> wgtFrom;
    QtObjPtr<QLabel> wgtSubject;
    QtObjPtr<QLabel> wgtTo;
  } emailTab;
};

class FMailClient : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMailClient(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMailClient() override;

  private slots:
    void onCellDoubleClick(int row, int col);
    void onTabClose(int idx);

  private:
    FMailClientData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    void setupInboxTab();
    void setupEmailTab();

    void enableEmail(int idx);
};


#endif
