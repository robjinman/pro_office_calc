#ifndef __PROCALC_EFFECTS_HPP__
#define __PROCALC_EFFECTS_HPP__


#include <functional>
#include <QColor>
#include <QPalette>


class UpdateLoop;
class QWidget;
class QString;


QColor tweenColour(const QColor& a, const QColor& b, double i);

void setColour(QWidget& widget, const QColor& colour, QPalette::ColorRole colourRole);

void transitionColour(UpdateLoop& updateLoop, QWidget& widget, const QColor& colour,
  QPalette::ColorRole colourRole, double duration, std::function<void()> fnOnFinish = []() {});

void setBackgroundImage(QWidget& widget, const QString& path);


#endif
