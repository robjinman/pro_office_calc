#ifndef __PROCALC_FRAGMENTS_F_DESKTOP_HPP__
#define __PROCALC_FRAGMENTS_F_DESKTOP_HPP__


#include <memory>
#include <QWidget>
#include "fragment.hpp"


struct FDesktopData : public FragmentData {

};

class FDesktop : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FDesktop(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FDesktop() override;

  private:
    FDesktopData m_data;
};


#endif
