#ifndef __PROCALC_FRAGMENTS_F_DESKTOP_HPP__
#define __PROCALC_FRAGMENTS_F_DESKTOP_HPP__


#include <vector>
#include <QWidget>
#include <QGridLayout>
#include <QMargins>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/f_main/f_desktop/desktop_icon.hpp"


struct FDesktopData : public FragmentData {
  QtObjPtr<QGridLayout> grid;
  std::vector<QtObjPtr<DesktopIcon>> icons;
};

class FDesktop : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FDesktop(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FDesktop() override;

  private slots:
    void onIconActivate(const std::string& name);

  private:
    FDesktopData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
