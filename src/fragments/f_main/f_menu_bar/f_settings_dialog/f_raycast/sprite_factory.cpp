#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/sprite_factory.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/animation_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/inventory_system.hpp"
#include "exception.hpp"
#include "utils.hpp"


//===========================================
// constructSprite
//===========================================
void constructSprite(EntityManager& em, const parser::Object& obj, double frameRate, CZone& zone,
  CRegion& region, const Matrix& parentTransform) {

  SpatialSystem& spatialSystem = em.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  RenderSystem& renderSystem = em.system<RenderSystem>(ComponentKind::C_RENDER);
  AnimationSystem& animationSystem = em.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  InventorySystem& inventorySystem = em.system<InventorySystem>(ComponentKind::C_INVENTORY);

  DBG_PRINT("Constructing Sprite\n");

  if (getValue(obj.dict, "subtype") == "bad_guy") {
    entityId_t id = Component::getNextId();

    CVRect* vRect = new CVRect(id, zone.entityId(), Size(70, 70));
    Matrix m = transformFromTriangle(obj.path);
    vRect->setTransform(parentTransform * obj.transform * m);
    vRect->zone = &zone;

    spatialSystem.addComponent(pComponent_t(vRect));

    CSprite* sprite = new CSprite(id, zone.entityId(), "bad_guy");
    sprite->region = &region;
    sprite->texViews = {
      QRectF(0, 0, 0.125, 0.125),
      QRectF(0.125, 0, 0.125, 0.125),
      QRectF(0.25, 0, 0.125, 0.125),
      QRectF(0.375, 0, 0.125, 0.125),
      QRectF(0.5, 0, 0.125, 0.125),
      QRectF(0.625, 0, 0.125, 0.125),
      QRectF(0.750, 0, 0.125, 0.125),
      QRectF(0.875, 0, 0.125, 0.125)
    };

    renderSystem.addComponent(pComponent_t(sprite));

    pCAnimation_t anim(new CAnimation(id));
    anim->animations.insert(std::make_pair("idle", Animation(frameRate, 1.0, {
      AnimationFrame{{
        QRectF(0, 0, 0.125, 0.125),
        QRectF(0.125, 0, 0.125, 0.125),
        QRectF(0.25, 0, 0.125, 0.125),
        QRectF(0.375, 0, 0.125, 0.125),
        QRectF(0.5, 0, 0.125, 0.125),
        QRectF(0.625, 0, 0.125, 0.125),
        QRectF(0.750, 0, 0.125, 0.125),
        QRectF(0.875, 0, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.125, 0.125, 0.125),
        QRectF(0.125, 0.125, 0.125, 0.125),
        QRectF(0.25, 0.125, 0.125, 0.125),
        QRectF(0.375, 0.125, 0.125, 0.125),
        QRectF(0.5, 0.125, 0.125, 0.125),
        QRectF(0.625, 0.125, 0.125, 0.125),
        QRectF(0.750, 0.125, 0.125, 0.125),
        QRectF(0.875, 0.125, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.25, 0.125, 0.125),
        QRectF(0.125, 0.25, 0.125, 0.125),
        QRectF(0.25, 0.25, 0.125, 0.125),
        QRectF(0.375, 0.25, 0.125, 0.125),
        QRectF(0.5, 0.25, 0.125, 0.125),
        QRectF(0.625, 0.25, 0.125, 0.125),
        QRectF(0.750, 0.25, 0.125, 0.125),
        QRectF(0.875, 0.25, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.375, 0.125, 0.125),
        QRectF(0.125, 0.375, 0.125, 0.125),
        QRectF(0.25, 0.375, 0.125, 0.125),
        QRectF(0.375, 0.375, 0.125, 0.125),
        QRectF(0.5, 0.375, 0.125, 0.125),
        QRectF(0.625, 0.375, 0.125, 0.125),
        QRectF(0.750, 0.375, 0.125, 0.125),
        QRectF(0.875, 0.375, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.5, 0.125, 0.125),
        QRectF(0.125, 0.5, 0.125, 0.125),
        QRectF(0.25, 0.5, 0.125, 0.125),
        QRectF(0.375, 0.5, 0.125, 0.125),
        QRectF(0.5, 0.5, 0.125, 0.125),
        QRectF(0.625, 0.5, 0.125, 0.125),
        QRectF(0.750, 0.5, 0.125, 0.125),
        QRectF(0.875, 0.5, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.625, 0.125, 0.125),
        QRectF(0.125, 0.625, 0.125, 0.125),
        QRectF(0.25, 0.625, 0.125, 0.125),
        QRectF(0.375, 0.625, 0.125, 0.125),
        QRectF(0.5, 0.625, 0.125, 0.125),
        QRectF(0.625, 0.625, 0.125, 0.125),
        QRectF(0.750, 0.625, 0.125, 0.125),
        QRectF(0.875, 0.625, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.75, 0.125, 0.125),
        QRectF(0.125, 0.75, 0.125, 0.125),
        QRectF(0.25, 0.75, 0.125, 0.125),
        QRectF(0.375, 0.75, 0.125, 0.125),
        QRectF(0.5, 0.75, 0.125, 0.125),
        QRectF(0.625, 0.75, 0.125, 0.125),
        QRectF(0.750, 0.75, 0.125, 0.125),
        QRectF(0.875, 0.75, 0.125, 0.125)
      }},
      AnimationFrame{{
        QRectF(0, 0.875, 0.125, 0.125),
        QRectF(0.125, 0.875, 0.125, 0.125),
        QRectF(0.25, 0.875, 0.125, 0.125),
        QRectF(0.375, 0.875, 0.125, 0.125),
        QRectF(0.5, 0.875, 0.125, 0.125),
        QRectF(0.625, 0.875, 0.125, 0.125),
        QRectF(0.750, 0.875, 0.125, 0.125),
        QRectF(0.875, 0.875, 0.125, 0.125)
      }}
    })));

    animationSystem.addComponent(std::move(anim));
    animationSystem.playAnimation(id, "idle", true);
  }
  else if (getValue(obj.dict, "subtype") == "ammo") {
    entityId_t id = Component::getNextId();

    CVRect* vRect = new CVRect(id, zone.entityId(), Size(30, 15));
    Matrix m = transformFromTriangle(obj.path);
    vRect->setTransform(parentTransform * obj.transform * m);
    vRect->zone = &zone;

    spatialSystem.addComponent(pComponent_t(vRect));

    CSprite* sprite = new CSprite(id, zone.entityId(), "ammo");
    sprite->region = &region;
    sprite->texViews = {
      QRectF(0, 0, 1, 1)
    };

    renderSystem.addComponent(pComponent_t(sprite));

    CCollectable* collectable = new CCollectable(id, "ammo", 20);
    inventorySystem.addComponent(pComponent_t(collectable));
  }
  else {
    EXCEPTION("Error constructing sprite of unknown type");
  }
}
