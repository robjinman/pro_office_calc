#include <QWidget>
#include <QImage>
#include "effects.hpp"
#include "update_loop.hpp"
#include "exception.hpp"


using std::function;


//===========================================
// tweenColour
//===========================================
QColor tweenColour(const QColor& a, const QColor& b, double i) {
  if (i < 0.0 || i > 1.0) {
    EXCEPTION("Tween val of " << i << " is out of range");
  }

  return QColor(a.red() + (b.red() - a.red()) * i,
                a.green() + (b.green() - a.green()) * i,
                a.blue() + (b.blue() - a.blue()) * i);
}

//===========================================
// setColour
//===========================================
void setColour(QWidget& widget, const QColor& colour, QPalette::ColorRole colourRole) {
  QPalette palette = widget.palette();
  palette.setColor(colourRole, colour);
  widget.setPalette(palette);
}

//===========================================
// transitionColour
//===========================================
void transitionColour(UpdateLoop& updateLoop, QWidget& widget, const QColor& colour,
  QPalette::ColorRole colourRole, double duration, function<void()> fnOnFinish) {

  QColor origCol = widget.palette().color(colourRole);

  int frames = updateLoop.fps() * duration;
  int i = 0;

  updateLoop.add([=,&widget]() mutable {
    double f = static_cast<double>(i) / static_cast<double>(frames);

    QColor c = tweenColour(origCol, colour, f);
    setColour(widget, c, colourRole);

    ++i;
    return i <= frames;
  }, fnOnFinish);
}

//===========================================
// setBackgroundImage
//===========================================
void setBackgroundImage(QWidget& widget, const QString& path) {
  QPixmap bg(path);
  bg = bg.scaled(widget.size(), Qt::IgnoreAspectRatio);
  QPalette palette = widget.palette();
  palette.setBrush(QPalette::Background, bg);
  widget.setPalette(palette);
}

//===========================================
// rotateHue
//===========================================
static void rotateHue(QImage& img, int deg) {
  for (int i = 0; i < img.width(); ++i) {
    for (int j = 0; j < img.height(); ++j) {
      QColor c = img.pixel(i, j);

      c.setHsv((c.hue() + deg) % 360, c.saturation(), c.value());
      img.setPixel(i, j, c.rgb());
    }
  }
}

//===========================================
// garbleImage
//===========================================
void garbleImage(const QImage& src, QImage& dest) {
  if (src.size() != dest.size()) {
    EXCEPTION("Source and destination images must be of same size");
  }

  for (int i = 0; i < dest.width(); ++i) {
    for (int j = 0; j < dest.height(); ++j) {
      // TODO
      if (i % 10 == 0 && j % 10 == 0) {
        dest.setPixel(i, j, qRgb(255, 0, 0));
      }
      else {
        dest.setPixel(i, j, src.pixel(i, j));
      }
    }
  }

  rotateHue(dest, 100);
}
