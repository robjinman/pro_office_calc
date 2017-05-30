#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__


class QPaintDevice;
class QRect;
class Scene;
class Camera;

void renderScene(QPaintDevice& target, const QRect& rect, const Scene& gameMap,
  const Camera& camera);


#endif
