#ifndef __PROCALC_FRAGMENTS_SCENE_OBJECTS_HPP__
#define __PROCALC_FRAGMENTS_SCENE_OBJECTS_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


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

struct Texture {
  QImage image;
  Size size_wd;
};

class Region;

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

    Region* region;
    std::string texture;
    Vec2f pos;
    double angle;
    Size size;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~Sprite() {}
};

typedef std::unique_ptr<Sprite> pSprite_t;

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

  Edge(const Edge& cpy) {
    kind = cpy.kind;
    lseg = cpy.lseg;
  }

  EdgeKind kind;
  LineSegment lseg;

  virtual ~Edge() {}
};

typedef std::unique_ptr<Edge> pEdge_t;

class Region;
typedef std::unique_ptr<Region> pRegion_t;

struct Region {
  bool hasCeiling = true;
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::string floorTexture;
  std::string ceilingTexture;
  std::list<pRegion_t> children;
  std::list<Edge*> edges;
  std::list<pSprite_t> sprites;
};

void forEachConstRegion(const Region& region, std::function<void(const Region&)> fn);
void forEachRegion(Region& region, std::function<void(Region&)> fn);

struct Wall : public Edge {
  Wall() : Edge(EdgeKind::WALL) {}

  std::string texture;
  Region* region;

  double height() const {
    return region->ceilingHeight - region->floorHeight;
  }

  virtual ~Wall() {}
};

struct JoiningEdge : public Edge {
  JoiningEdge() : Edge(EdgeKind::JOINING_EDGE) {}
  JoiningEdge(const JoiningEdge& cpy) : Edge(cpy) {
    topTexture = cpy.topTexture;
    bottomTexture = cpy.bottomTexture;
    regionA = cpy.regionA;
    regionB = cpy.regionB;
  }

  void mergeIn(const JoiningEdge& other) {
    if (other.topTexture != "default") {
      topTexture = other.topTexture;
    }
    if (other.bottomTexture != "default") {
      bottomTexture = other.bottomTexture;
    }
  }

  std::string topTexture = "default";
  std::string bottomTexture = "default";

  Region* regionA = nullptr;
  Region* regionB = nullptr;

  virtual ~JoiningEdge() {}
};


#endif
