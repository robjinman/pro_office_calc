#ifndef __PROCALC_ANIMATION_HPP__
#define __PROCALC_ANIMATION_HPP__


#include <QColor>


class UpdateLoop;
class QWidget;


QColor tweenColour(const QColor& a, const QColor& b, double i);

void transitionColour(UpdateLoop& updateLoop, QWidget& widget, const QColor& colour,
  double duration);


#endif
