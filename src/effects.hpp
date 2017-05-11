#ifndef __PROCALC_EFFECTS_HPP__
#define __PROCALC_EFFECTS_HPP__


#include <QColor>
#include <QPalette>


class UpdateLoop;
class QWidget;
class QString;


QColor tweenColour(const QColor& a, const QColor& b, double i);

void transitionColour(UpdateLoop& updateLoop, QWidget& widget, const QColor& colour,
  QPalette::ColorRole colourRole, double duration);

void setBackgroundImage(QWidget& widget, const QString& path);


#endif
