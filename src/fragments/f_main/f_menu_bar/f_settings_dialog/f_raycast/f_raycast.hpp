#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_HPP__


#include <memory>
#include <QOpenGLWidget>
#include "fragment.hpp"


class QPaintEvent;


struct FRaycastData : public FragmentData {};

class FRaycast : public QOpenGLWidget, public Fragment {
  Q_OBJECT

  public:
    FRaycast(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  public slots:
    void animate();

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    FRaycastData m_data;
};


#endif
