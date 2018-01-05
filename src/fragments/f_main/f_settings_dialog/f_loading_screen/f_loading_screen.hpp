#ifndef __PROCALC_FRAGMENTS_F_LOADING_SCREEN_HPP__
#define __PROCALC_FRAGMENTS_F_LOADING_SCREEN_HPP__


#include <memory>
#include <QLabel>
#include <QMargins>
#include "fragment.hpp"


struct FLoadingScreenData : public FragmentData {};

class FLoadingScreen : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FLoadingScreen(Fragment& parent, FragmentData& parentData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FLoadingScreen() override;

  private:
    FLoadingScreenData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
