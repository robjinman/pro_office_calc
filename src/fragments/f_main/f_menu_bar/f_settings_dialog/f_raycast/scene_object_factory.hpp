#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECT_FACTORY_HPP__


class Scene;
class BehaviourSystem;
namespace parser { class Object; }


void constructRootRegion(Scene& scene, BehaviourSystem& behaviourSystem,
  const parser::Object& obj);


#endif
