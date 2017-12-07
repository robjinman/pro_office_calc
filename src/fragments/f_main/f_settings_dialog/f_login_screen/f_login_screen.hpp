#ifndef __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_HPP__
#define __PROCALC_FRAGMENTS_F_LOGIN_SCREEN_HPP__


#include <memory>
#include <QLabel>
#include <QMargins>
#include <QTimer>
#include <QImage>
#include "fragment.hpp"


struct FLoginScreenData : public FragmentData {
  std::unique_ptr<QPixmap> background;
};

class FLoginScreen : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FLoginScreen(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private:
    FLoginScreenData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
