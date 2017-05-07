#include <QPixmap>
#include <QPainter>
#include "rotated_widget.hpp"


//===========================================
// RotatedWidget::RotatedWidget
//===========================================
RotatedWidget::RotatedWidget(QWidget* parent)
  : QWidget(parent),
    m_baseWidget(nullptr) {}

//===========================================
// RotatedWidget::setBaseWidget
//===========================================
void RotatedWidget::setBaseWidget(QWidget* baseWidget) {
  m_baseWidget = baseWidget;
}

//===========================================
// RotatedWidget::baseWidget
//===========================================
QWidget* RotatedWidget::baseWidget() const {
  return m_baseWidget;
}

//===========================================
// RotatedWidget::paintEvent
//===========================================
void RotatedWidget::paintEvent(QPaintEvent* event) {
  m_baseWidget->resize(size());
  m_baseWidget->hide();

  QPixmap basePixmap(m_baseWidget->size());
  m_baseWidget->render(&basePixmap);

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.setRenderHint(QPainter::HighQualityAntialiasing);
  //p.translate(m_baseWidget->width() / 2, m_baseWidget->height() / 2);
  p.rotate(-15);
  //p.translate(-m_baseWidget->width() / 2, -m_baseWidget->height() / 2);
  p.drawPixmap(0, 0, basePixmap);
  p.end();
}
