#ifndef __PROCALC_FRAGMENTS_SCENE_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_SCENE_OBJECT_FACTORY_HPP__


#include <list>
#include <map>


class SceneGraph;
class Camera;
class Sprite;
class Region;
class Wall;
class JoiningEdge;
class Point;
class Matrix;
namespace parser { class Object; }


std::list<Wall*> constructWalls(const parser::Object& obj, Region* region,
  const Matrix& parentTransform);

Camera* constructCamera(const parser::Object& obj, const Region& region,
  const Matrix& parentTransform);

Sprite* constructSprite(const parser::Object& obj, Region& region, const Matrix& parentTransform);

std::list<JoiningEdge*> constructJoiningEdges(const parser::Object& obj, Region* region,
  const Matrix& parentTransform);

Region* constructRegion_r(SceneGraph& sg, const parser::Object& obj,
  const Matrix& parentTransform, std::map<Point, bool>& endpoints);

void constructRootRegion(SceneGraph& sg, const parser::Object& obj);


#endif
