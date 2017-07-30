#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPRITE_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPRITE_FACTORY_HPP__


class EntityManager;
class CZone;
class CRegion;
class Matrix;
namespace parser { class Object; }

void constructSprite(EntityManager& em, const parser::Object& obj, double frameRate, CZone& zone,
  CRegion& region, const Matrix& parentTransform);


#endif
