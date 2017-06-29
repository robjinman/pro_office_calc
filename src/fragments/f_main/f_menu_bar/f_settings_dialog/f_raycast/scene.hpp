#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct AnimationFrame {
  std::array<QRectF, 8> parts;

  const QRectF& part(double angle) const {
    double da = PI * 0.25;
    return parts[static_cast<int>(round(normaliseAngle(angle) / da)) % 8];
  }
};

class Animation {
  public:
    int fps = 0;
    std::vector<AnimationFrame> frames;

    void update();
    const AnimationFrame& currentFrame() const {
      return frames[m_currentFrameIdx];
    }

  private:
    double m_elapsed = 0.0;
    int m_currentFrameIdx = 0;
};

class Sprite {
  public:
    Sprite(const Size& size, const std::string& texture)
      : texture(texture),
        size(size) {}

    void setTransform(const Matrix& m) {
      pos.x = m.tx();
      pos.y = m.ty();
      angle = m.a();
    }

    const QRectF& textureRegion(const Point& camPos) const {
      Vec2f v = pos - camPos;
      return animations.at("idle").currentFrame().part(PI - atan2(v.y, v.x) + angle);
    }

    std::string texture;
    Vec2f pos;
    double angle;
    Size size;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~Sprite() {}
};

struct Ammo : public Sprite {
  Ammo() : Sprite(Size(30, 15), "ammo") {
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
  BadGuy() : Sprite(Size(70, 70), "bad_guy") {
    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 0.125, 1),
      QRectF(0.125, 0, 0.125, 1),
      QRectF(0.25, 0, 0.125, 1),
      QRectF(0.375, 0, 0.125, 1),
      QRectF(0.5, 0, 0.125, 1),
      QRectF(0.625, 0, 0.125, 1),
      QRectF(0.750, 0, 0.125, 1),
      QRectF(0.875, 0, 0.125, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~BadGuy() override {}
};

enum class EdgeKind {
  JOINING_EDGE,
  WALL
};

struct Edge {
  Edge(EdgeKind kind)
    : kind(kind) {}

  EdgeKind kind;
  LineSegment lseg;

  virtual ~Edge() {}
};

struct ConvexRegion {
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::list<std::unique_ptr<ConvexRegion>> children;
  std::list<Edge*> edges;
  std::list<std::unique_ptr<Sprite>> sprites;
};

struct Wall : public Edge {
  Wall() : Edge(EdgeKind::WALL) {}

  std::string texture;
  ConvexRegion* region;

  double height() const {
    return region->ceilingHeight - region->floorHeight;
  }

  virtual ~Wall() {}
};

struct JoiningEdge : public Edge {
  JoiningEdge() : Edge(EdgeKind::JOINING_EDGE) {}

  std::string topTexture;
  std::string bottomTexture;

  ConvexRegion* regionA;
  ConvexRegion* regionB;

  virtual ~JoiningEdge() {}
};

class Scene {
  public:
    Scene(const std::string& mapFilePath);

    Size viewport;
    std::unique_ptr<Camera> camera;
    std::map<std::string, QImage> textures;

    std::unique_ptr<ConvexRegion> rootRegion;
    std::list<std::unique_ptr<Edge>> edges;
    const ConvexRegion* currentRegion;

    void rotateCamera(double da);
    void translateCamera(const Vec2f& dir);

    // TODO
    std::list<std::unique_ptr<Wall>> walls;
    std::list<std::unique_ptr<Sprite>> sprites;
    double wallHeight;

  private:
    void addObject(const parser::Object& obj);
};


#endif
