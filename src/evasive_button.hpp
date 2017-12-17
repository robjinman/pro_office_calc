#ifndef __PROCALC_EVASIVE_BUTTON_HPP__
#define __PROCALC_EVASIVE_BUTTON_HPP__


#include <memory>
#include <QPushButton>
#include <QTimer>


class EvasiveButton : public QPushButton {
  Q_OBJECT

  public:
    EvasiveButton(const QString& caption);

    // Must be called after the button's position has been set, e.g. by being added to a layout
    void reset();

    void onMouseMove();

    ~EvasiveButton() override;

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;

  private slots:
    void tick();

  private:
    bool cursorInRange(QPoint cursor) const;

    bool m_active;
    QPoint m_originalPos;
    std::unique_ptr<QTimer> m_timer;
};


#endif
