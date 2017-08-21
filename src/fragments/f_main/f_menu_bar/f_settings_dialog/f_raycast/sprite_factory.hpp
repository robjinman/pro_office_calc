#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPRITE_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPRITE_FACTORY_HPP__


class EntityManager;
class AudioService;
class TimeService;
class CZone;
class CRegion;
class Matrix;
namespace parser { class Object; }

void constructSprite(EntityManager& em, AudioService& audioService, const parser::Object& obj,
  TimeService& timeService, CZone& zone, CRegion& region, const Matrix& parentTransform);


#endif
