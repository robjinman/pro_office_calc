#ifndef __PROCALC_FRAGMENTS_F_LOADING_SCREEN_HPP__
#define __PROCALC_FRAGMENTS_F_LOADING_SCREEN_HPP__


#include <memory>
#include <QLabel>
#include <QPixmap>
#include "fragment.hpp"


struct FLoadingScreenData : public FragmentData {
  std::unique_ptr<QPixmap> background;
};

class FLoadingScreen : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FLoadingScreen(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private:
    FLoadingScreenData m_data;
};


#endif
