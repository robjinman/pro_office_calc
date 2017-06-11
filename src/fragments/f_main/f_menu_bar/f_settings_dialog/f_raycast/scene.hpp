#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QPixmap>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Wall {
  LineSegment lseg;
  std::string texture;
};

struct AnimationFrame {
  std::array<QRectF, 8> parts;

  const QRectF& part(double angle) const {
    return parts[static_cast<int>(round(8.0 * angle / PI)) % 8];
  }
};

class Animation {
  public:
    int fps;
    std::vector<AnimationFrame> frames;

    void update();
    const AnimationFrame& currentFrame() const;

  private:
    double m_elapsed;
    int m_currentFrameIdx;
};

class Sprite {
  public:
    Sprite(const std::string& texture)
      : texture(texture) {}

    std::string texture;
    Matrix transform;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~Sprite() {}
};

struct Ammo : public Sprite {
  Ammo() : Sprite("ammo") {
    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~Ammo() override {}
};

struct BadGuy : public Sprite {
  BadGuy() : Sprite("bad_guy") {
    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~BadGuy() override {}
};

class Scene {
  public:
    Scene(const std::string& mapFilePath);

    std::unique_ptr<Camera> camera;
    std::list<std::unique_ptr<Wall>> walls;
    std::list<std::unique_ptr<Sprite>> sprites;
    Size viewport;
    double wallHeight;
    std::map<std::string, QPixmap> textures;

  private:
    void addObject(const parser::Object& obj);
};


#endif
