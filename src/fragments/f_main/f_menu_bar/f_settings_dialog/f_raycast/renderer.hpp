#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__


#include <array>


class QImage;
class Scene;

typedef std::array<double, 10000> tanMap_t;
typedef std::array<double, 10000> atanMap_t;

class Renderer {
  public:
    Renderer();
    void renderScene(QImage& target, const Scene& scene);

  private:
    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;
};


#endif
