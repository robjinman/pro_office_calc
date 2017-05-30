#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


void renderScene(QPaintDevice& target, const QRect& rect, const Scene& gameMap, const Camera& camera) {
  QPainter painter;
  painter.begin(&target);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect, QBrush(QColor(64, 32, 64)));

  painter.end();
}
