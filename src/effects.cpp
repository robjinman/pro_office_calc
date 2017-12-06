#include <random>
#include <QWidget>
#include <QImage>
#include "effects.hpp"
#include "update_loop.hpp"
#include "exception.hpp"


using std::function;


static std::mt19937 randEngine;


//===========================================
// mod
//===========================================
inline static int mod(int n, int quot) {
  if (n < 1) {
    return mod(quot + n, quot);
  }
  else {
    return n % quot;
  }
}

//===========================================
// rotateHue
//===========================================
void rotateHue(QImage& img, int deg) {
  for (int i = 0; i < img.width(); ++i) {
    for (int j = 0; j < img.height(); ++j) {
      QColor c = img.pixel(i, j);

      c.setHsv((c.hue() + deg) % 360, c.saturation(), c.value());
      img.setPixel(i, j, c.rgb());
    }
  }
}

//===========================================
// colourize
//===========================================
void colourize(QImage& img, const QColor& c, double x) {
  int w = img.width();
  int h = img.height();

  for (int j = 0; j < h; ++j) {
    for (int i = 0; i < w; ++i) {
      img.setPixel(i, j, tweenColour(img.pixel(i, j), c, x).rgb());
    }
  }
}

//===========================================
// tweenColour
//
// Ignores alpha
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
// garbleImage
//===========================================
void garbleImage(const QImage& src, QImage& dest) {
  if (src.size() != dest.size()) {
    EXCEPTION("Source and destination images must be of same size");
  }

  int w = dest.width();
  int h = dest.height();

  const float prob = 0.03;
  std::uniform_int_distribution<int> rollDie(0, 1.0 / prob);
  std::normal_distribution<double> randShift(0, 10);
  int shift = randShift(randEngine);

  for (int j = 0; j < h; ++j) {
    if (rollDie(randEngine) == 0) {
      shift = randShift(randEngine);
    }

    for (int i = 0; i < w; ++i) {
      dest.setPixel(i, j, src.pixel(mod(i + shift, w), j));
    }
  }

  std::uniform_int_distribution<int> randHue(0, 359);
  colourize(dest, QColor(255, 0, 0), 0.08);
  rotateHue(dest, randHue(randEngine));
}
