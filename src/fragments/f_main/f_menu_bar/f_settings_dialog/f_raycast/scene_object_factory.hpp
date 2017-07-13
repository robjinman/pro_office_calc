#ifndef __PROCALC_FRAGMENTS_SCENE_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_SCENE_OBJECT_FACTORY_HPP__


class SceneGraph;
namespace parser { class Object; }


void constructRootRegion(SceneGraph& sg, const parser::Object& obj);


#endif
