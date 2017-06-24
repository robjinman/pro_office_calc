#include <cmath>
#include <cassert>
#include <limits>
#include <list>
#include <QPainter>
#include <QPaintDevice>
#include <QBrush>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer2.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::array;
using std::unique_ptr;


static const double ATAN_MIN = -10.0;
static const double ATAN_MAX = 10.0;


struct WallSlice {
  int wallTop_px;
  int sliceH_px;
};

//===========================================
// fastTan_rp
//
// Retrieves the reciprocal of tan(a) from the lookup table
//===========================================
static double fastTan_rp(const tanMap_t& tanMap_rp, double a) {
  static const double x = static_cast<double>(tanMap_rp.size()) / (2.0 * PI);
  return tanMap_rp[static_cast<int>(normaliseAngle(a) * x)];
}

//===========================================
// fastATan
//
// Retrieves atan(x) from the lookup table
//===========================================
static double fastATan(const atanMap_t& atanMap, double x) {
  if (x < ATAN_MIN) {
    x = ATAN_MIN;
  }
  if (x > ATAN_MAX) {
    x = ATAN_MAX;
  }
  double dx = (ATAN_MAX - ATAN_MIN) / static_cast<double>(atanMap.size());
  return atanMap[static_cast<int>((x - ATAN_MIN) / dx)];
}

//===========================================
// worldPointToFloorTexel
//===========================================
static Point worldPointToFloorTexel(const Point& p, const Size& texSz_wd_rp, const Size& texSz_px) {
  double nx = p.x * texSz_wd_rp.x;
  double ny = p.y * texSz_wd_rp.y;
  return Point((nx - floor(nx)) * texSz_px.x, (ny - floor(ny)) * texSz_px.y);
}

//===========================================
// sampleTexture
//===========================================
static QRect sampleTexture(const QRect& rect, const WallX& X, double camHeight,
  double width_wd, double height_wd) {

  double H_px = rect.height();
  double W_px = rect.width();

  double hWorldUnit_px = W_px / width_wd;
  double texW_wd = W_px / hWorldUnit_px;

  double n = X.distanceAlongTarget / texW_wd;
  double x = (n - floor(n)) * texW_wd;

  double texBottom_px = H_px - ((camHeight + X.sliceBottom_wd) / height_wd) * H_px;
  double texTop_px = H_px - ((camHeight + X.sliceTop_wd) / height_wd) * H_px;

  int i = x * hWorldUnit_px;

  return QRect(rect.x() + i, rect.y() + texTop_px, 1, texBottom_px - texTop_px);
}

//===========================================
// drawWallSlice
//===========================================
static WallSlice drawWallSlice(QPainter& painter, const Scene& scene,
  const WallX& X, double F, int screenX_px, int screenH_px,
  double vWorldUnitsInPx) {

  const QImage& wallTex = scene.textures.at(X.wall->texture);

  int screenSliceBottom_px = screenH_px - X.projSliceBottom_wd * vWorldUnitsInPx;
  int screenSliceTop_px = screenH_px - X.projSliceTop_wd * vWorldUnitsInPx;

  QRect trgRect(screenX_px, screenSliceTop_px, 1, screenSliceBottom_px - screenSliceTop_px);
  QRect srcRect = sampleTexture(wallTex.rect(), X, scene.camera->height,
    scene.wallHeight, scene.wallHeight);

  painter.drawImage(trgRect, wallTex, srcRect);

  return WallSlice{screenSliceTop_px, screenSliceBottom_px - screenSliceTop_px};
}

//===========================================
// Renderer2::Renderer2
//===========================================
Renderer2::Renderer2() {
  for (unsigned int i = 0; i < m_tanMap_rp.size(); ++i) {
    m_tanMap_rp[i] = 1.0 / tan(2.0 * PI * static_cast<double>(i)
      / static_cast<double>(m_tanMap_rp.size()));
  }

  double dx = (ATAN_MAX - ATAN_MIN) / static_cast<double>(m_atanMap.size());
  for (unsigned int i = 0; i < m_atanMap.size(); ++i) {
    m_atanMap[i] = atan(ATAN_MIN + dx * static_cast<double>(i));
  }
}

static Intersection* constructIntersection(Edge::kind_t kind) {
  switch (kind) {
    case Edge::WALL:
      return new WallX;
      break;
    case Edge::JOINING_EDGE:
      return new JoiningEdgeX;
      break;
    default:
      EXCEPTION("Error constructing Intersection of unknown type");
  }
}

void findIntersections_r(const Camera& camera, const LineSegment& ray, const ConvexRegion& region,
  const Edge* exclude, CastResult& result) {

  for (auto it = region.children.begin(); it != region.children.end(); ++it) {
    findIntersections_r(camera, ray, **it, nullptr, result);
  }

  Matrix invCamMatrix = camera.matrix().inverse();

  for (auto it = region.edges.begin(); it != region.edges.end(); ++it) {
    Edge& edge = **it;

    LineSegment lseg = transform(edge.lseg, invCamMatrix);

    Point pt;
    if (lineSegmentIntersect(ray, lseg, pt)) {
      if (exclude != &edge) {
        Intersection* X = constructIntersection(edge.kind());
        X->point_cam = pt;
        X->point_world = camera.matrix() * pt;
        X->distanceFromCamera = pt.x;
        X->distanceAlongTarget = distance(lseg.A, pt);

        result.intersections.push_back(pIntersection_t(X));

        if (edge.kind() == Edge::WALL) {
          WallX* wallX = dynamic_cast<WallX*>(X);
          Wall& wall = dynamic_cast<Wall&>(edge);

          wallX->wall = &wall;
        }
        else if (edge.kind() == Edge::JOINING_EDGE) {
          JoiningEdgeX* jeX = dynamic_cast<JoiningEdgeX*>(X);
          JoiningEdge& je = dynamic_cast<JoiningEdge&>(edge);

          jeX->joiningEdge = &je;

          const ConvexRegion& next = je.regionA == &region ? *je.regionB : *je.regionA;
          findIntersections_r(camera, ray, next, &edge, result);
        }
      }
    }
  }
}

static void castRay(Vec2f r, const Scene& scene, CastResult& result) {
  const Camera& cam = *scene.camera;
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));
  //double distanceFromCamera = std::numeric_limits<double>::infinity();
  //Matrix invCamMatrix = cam.matrix().inverse();

  const ConvexRegion& region = *scene.currentRegion;

  findIntersections_r(cam, ray, region, nullptr, result);

  result.intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  for (auto it = result.intersections.begin(); it != result.intersections.end(); ++it) {
    if ((*it)->kind == Edge::WALL) {
      WallX& X = dynamic_cast<WallX&>(**it);

      double targetHeight = scene.wallHeight; // TODO

      const Point& pt = X.point_cam;

      LineSegment projPlane(Point(cam.F, 0.00001 - scene.viewport.y / 2),
        Point(cam.F, scene.viewport.y * 0.5));

      Matrix m(cam.vAngle, Vec2f(0, 0));
      LineSegment rotProjPlane = transform(projPlane, m);

      LineSegment wallRay0(Point(0, 0), Point(pt.x, -cam.height));
      LineSegment wallRay1(Point(0, 0), Point(pt.x, targetHeight - cam.height));

      LineSegment projRay0(Point(0, 0), rotProjPlane.A * 999.9);
      LineSegment projRay1(Point(0, 0), rotProjPlane.B * 999.9);

      Point p;
      p = lineIntersect(wallRay0.line(), rotProjPlane.line());
      double proj_w0 = rotProjPlane.signedDistance(p.x);
      p = lineIntersect(wallRay1.line(), rotProjPlane.line());
      double proj_w1 = rotProjPlane.signedDistance(p.x);

      X.projSliceBottom_wd = clipNumber(proj_w0, Size(0, scene.viewport.y));
      X.projSliceTop_wd = clipNumber(proj_w1, Size(0, scene.viewport.y));

      LineSegment wall(Point(pt.x + 0.00001, -cam.height), Point(pt.x, targetHeight - cam.height));

      double wall_s0 = lineIntersect(projRay0.line(), wall.line()).y;
      double wall_s1 = lineIntersect(projRay1.line(), wall.line()).y;

      X.sliceBottom_wd = clipNumber(wall.A.y, Size(wall_s0, wall_s1));
      X.sliceTop_wd = clipNumber(wall.B.y, Size(wall_s0, wall_s1));
    }
  }
}

//===========================================
// Renderer2::renderScene
//===========================================
void Renderer2::renderScene(QImage& target, const Scene& scene) {
  QPainter painter;
  painter.begin(&target);

  const Camera& cam = *scene.camera;

  int screenW_px = target.width();
  int screenH_px = target.height();

  double hWorldUnitsInPx = screenW_px / scene.viewport.x;
  double vWorldUnitsInPx = screenH_px / scene.viewport.y;

  QRect rect(QPoint(), QSize(screenW_px, screenH_px));
  painter.fillRect(rect, QBrush(QColor(0, 0, 0)));

  for (int screenX_px = 0; screenX_px < screenW_px; ++screenX_px) {
    double projX_wd = static_cast<double>(screenX_px - screenW_px / 2) / hWorldUnitsInPx;

    CastResult result;
    castRay(Vec2f(cam.F, projX_wd), scene, result);

    for (auto it = result.intersections.begin(); it != result.intersections.end(); ++it) {
      if ((*it)->kind == Edge::WALL) {
        const WallX& wallX = dynamic_cast<const WallX&>(**it);
        WallSlice slice = drawWallSlice(painter, scene, wallX, cam.F, screenX_px, screenH_px,
          vWorldUnitsInPx);

        break;
      }
    }
  }

  painter.end();
}
