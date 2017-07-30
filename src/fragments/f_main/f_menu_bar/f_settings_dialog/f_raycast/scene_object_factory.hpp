#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_OBJECT_FACTORY_HPP__


#include <string>


class EntityManager;

void loadMap(const std::string& mapFilePath, EntityManager& entityManager, double frameRate);


#endif
