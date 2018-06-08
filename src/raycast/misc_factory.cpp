#include <algorithm>
#include <vector>
#include "raycast/misc_factory.hpp"
#include "raycast/root_factory.hpp"
#include "raycast/geometry.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/spawn_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/c_door_behaviour.hpp"
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/c_player_behaviour.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "exception.hpp"


using std::stringstream;
using std::unique_ptr;
using std::function;
using std::string;
using std::list;
using std::map;
using std::set;
using std::vector;


//===========================================
// setBoolean
//===========================================
static void setBoolean(bool& b, const std::string& s) {
  if (s == "true") {
    b = true;
  }
  else if (s == "false") {
    b = false;
  }
}

//===========================================
// MiscFactory::MiscFactory
//===========================================
MiscFactory::MiscFactory(RootFactory& rootFactory, EntityManager& entityManager,
  AudioService& audioService, TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService) {}

//===========================================
// MiscFactory::types
//===========================================
const set<string>& MiscFactory::types() const {
  static const set<string> types{
    "player",
    "player_inventory",
    "door",
    "switch",
    "elevator",
    "spawn_point",
    "collectable_item"
  };
  return types;
}

//===========================================
// MiscFactory::constructCollectableItem
//===========================================
bool MiscFactory::constructCollectableItem(entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("sprite", entityId, obj, parentId, parentTransform)) {
    InventorySystem& inventorySystem =
      m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

    CCollectable* collectable = new CCollectable(entityId, "item");

    if (!contains<string>(obj.dict, "name")) {
      EXCEPTION("collectable_item is missing 'name' property");
    }

    collectable->name = obj.dict.at("name");

    inventorySystem.addComponent(pComponent_t(collectable));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructPlayerInventory
//===========================================
bool MiscFactory::constructPlayerInventory() {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  InventorySystem& inventorySystem =
    m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
  EventHandlerSystem& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);

  const Player& player = *spatialSystem.sg.player;
  const Size& viewport = renderSystem.rg.viewport;

  CCollector* inventory = new CCollector(player.body);
  inventory->buckets["ammo"] = pBucket_t(new CounterBucket(50));
  inventory->buckets["item"] = pBucket_t(new ItemBucket(5));
  inventorySystem.addComponent(pComponent_t(inventory));

  double itemsDisplayH_wd = viewport.y * 0.1;
  double itemsDisplayW_wd = viewport.x * 0.5;
  double itemsDisplayAspectRatio = itemsDisplayW_wd / itemsDisplayH_wd;
  double itemsDisplayH_px = 50;
  double itemsDisplayW_px = itemsDisplayH_px * itemsDisplayAspectRatio;

  QImage imgItems(itemsDisplayW_px, itemsDisplayH_px, QImage::Format_ARGB32);
  imgItems.fill(Qt::GlobalColor::transparent);

  renderSystem.rg.textures["items_display"] = Texture{imgItems, Size(0, 0)};

  entityId_t ammoId = Component::getNextId();
  entityId_t healthId = Component::getNextId();
  entityId_t itemsId = Component::getNextId();

  CImageOverlay* itemsDisplay = new CImageOverlay(itemsId, "items_display",
    Point(0, 0), Size(itemsDisplayW_wd, itemsDisplayH_wd), 1);
  renderSystem.addComponent(pComponent_t(itemsDisplay));

  CTextOverlay* ammoCounter = new CTextOverlay(ammoId, "AMMO 0/50", Point(0.1, viewport.y - 0.5),
    0.5, Qt::green, 2);
  renderSystem.addComponent(pComponent_t(ammoCounter));

  CTextOverlay* healthCounter = new CTextOverlay(healthId, "HEALTH 10/10",
    Point(9.5, viewport.y - 0.5), 0.5, Qt::red, 2);
  renderSystem.addComponent(pComponent_t(healthCounter));

  CEventHandler* syncDisplay = new CEventHandler(player.body);

  syncDisplay->targetedEventHandlers.push_back(EventHandler{"bucket_count_change",
    [=](const GameEvent& e_) {

    const EBucketCountChange& e = dynamic_cast<const EBucketCountChange&>(e_);

    if (e.collectableType == "ammo") {
      stringstream ss;
      ss << "AMMO " << e.bucket.count << "/" << e.bucket.capacity;
      ammoCounter->text = ss.str();

      if (e.bucket.count > e.prevCount) {
        m_audioService.playSound("ammo_collect");
      }
    }
  }});

  syncDisplay->targetedEventHandlers.push_back(EventHandler{"bucket_items_change",
    [=, &renderSystem](const GameEvent& e_) {

    const EBucketItemsChange& e = dynamic_cast<const EBucketItemsChange&>(e_);

    if (e.collectableType == "item") {
      QImage& target = renderSystem.rg.textures["items_display"].image;
      target.fill(Qt::GlobalColor::transparent);

      QPainter painter;
      painter.begin(&target);

      int i = 0;
      for (auto it = e.bucket.items.begin(); it != e.bucket.items.end(); ++it) {
        entityId_t id = it->second;
        const CRender& c = dynamic_cast<const CRender&>(renderSystem.getComponent(id));

        if (c.kind == CRenderKind::SPRITE) {
          const CSprite& sprite = dynamic_cast<const CSprite&>(c);
          const QImage& img = renderSystem.rg.textures.at(sprite.texture).image;

          double slotH = itemsDisplayH_px;
          double slotW = itemsDisplayW_px / e.bucket.capacity;
          double slotX = slotW * i;
          double slotY = 0;
          double margin = slotH * 0.2;
          double aspectRatio = static_cast<double>(img.width()) / img.height();
          double maxH = slotH - margin * 2;
          double maxW = slotW - margin * 2;
          double h = maxH;
          double w = h * aspectRatio;
          double s = maxW / w;
          h *= s;
          w *= s;

          QRect srcRect(0, 0, img.width(), img.height());
          QRect trgRect(slotX + margin, slotY + margin, w, h);

          painter.setBrush(QColor(0, 0, 0, 100));
          painter.setPen(Qt::NoPen);
          painter.drawRect(slotX, slotY, slotW, slotH);
          painter.drawImage(trgRect, img, srcRect);
        }

        ++i;
      }

      painter.end();

      if (static_cast<int>(e.bucket.items.size()) > e.prevCount) {
        m_audioService.playSound("item_collect");
      }
    }
  }});

  syncDisplay->targetedEventHandlers.push_back(EventHandler{"entity_damaged",
    [=, &damageSystem, &player](const GameEvent& e_) {

    const EEntityDamaged& e = dynamic_cast<const EEntityDamaged&>(e_);

    if (e.entityId == player.body) {
      const CDamage& damage = dynamic_cast<const CDamage&>(damageSystem.getComponent(player.body));

      stringstream ss;
      ss << "HEALTH " << damage.health << "/" << damage.maxHealth;
      healthCounter->text = ss.str();
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(syncDisplay));

  entityId_t bgId = Component::getNextId();

  CColourOverlay* bg = new CColourOverlay(bgId, QColor(0, 0, 0, 100), Point(0, viewport.y - 0.7),
    Size(viewport.x, 0.7), 1);

  renderSystem.addComponent(pComponent_t(bg));

  return true;
}

//===========================================
// MiscFactory::constructPlayer
//===========================================
bool MiscFactory::constructPlayer(parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  AnimationSystem& animationSystem =
    m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  DamageSystem& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  BehaviourSystem& behaviourSystem =
    m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  double tallness = std::stod(getValue(obj.dict, "tallness"));

  Matrix m = parentTransform * obj.groupTransform * obj.pathTransform
    * transformFromTriangle(obj.path);

  Player* player = new Player(m_entityManager, m_audioService, tallness, zone, m);
  player->sprite = Component::getNextId();
  player->crosshair = Component::getNextId();

  CDamage* damage = new CDamage(player->body, 10, 10);
  damageSystem.addComponent(pCDamage_t(damage));

  CPlayerBehaviour* behaviour = new CPlayerBehaviour(player->body, m_entityManager, m_timeService);
  behaviourSystem.addComponent(pComponent_t(behaviour));

  const Size& viewport = renderSystem.rg.viewport;

  Size sz(0.5, 0.5);
  CImageOverlay* crosshair = new CImageOverlay(player->crosshair, "crosshair",
    viewport / 2 - sz / 2, sz);
  renderSystem.addComponent(pCRender_t(crosshair));

  CImageOverlay* sprite = new CImageOverlay(player->sprite, "gun", Point(viewport.x * 0.5, 0),
    Size(4, 4));
  sprite->texRect = QRectF(0, 0, 0.25, 1);
  renderSystem.addComponent(pCRender_t(sprite));

  CAnimation* shoot = new CAnimation(player->sprite);
  shoot->addAnimation(pAnimation_t(new Animation("shoot", m_timeService.frameRate, 0.4, {
    AnimationFrame{{
      QRectF(0.75, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0.5, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0.25, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0, 0, 0.25, 1)
    }}
  })));
  shoot->addAnimation(pAnimation_t(new Animation("shoot_no_ammo", m_timeService.frameRate, 0.2, {
    AnimationFrame{{
      QRectF(0.25, 0, 0.25, 1)
    }},
    AnimationFrame{{
      QRectF(0, 0, 0.25, 1)
    }}
  })));

  animationSystem.addComponent(pCAnimation_t(shoot));

  spatialSystem.sg.player.reset(player);

  return true;
}

//===========================================
// MiscFactory::constructSpawnPoint
//===========================================
bool MiscFactory::constructSpawnPoint(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  SpawnSystem& spawnSystem = m_entityManager.system<SpawnSystem>(ComponentKind::C_SPAWN);

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  CVRect* vRect = new CVRect(entityId, zone.entityId(), Size(1, 1));
  Matrix m = transformFromTriangle(obj.path);
  vRect->setTransform(parentTransform * obj.groupTransform * obj.pathTransform * m);
  vRect->zone = &zone;

  spatialSystem.addComponent(pComponent_t(vRect));

  CSpawnPoint* spawnPoint = new CSpawnPoint(entityId);
  spawnSystem.addComponent(pComponent_t(spawnPoint));

  return true;
}

//===========================================
// MiscFactory::constructDoor
//===========================================
bool MiscFactory::constructDoor(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    CDoorBehaviour* behaviour = new CDoorBehaviour(entityId, m_entityManager, m_timeService,
      m_audioService);

    string s = getValue(obj.dict, "player_activated", "");
    setBoolean(behaviour->isPlayerActivated, s);

    s = getValue(obj.dict, "player_activated", "");
    setBoolean(behaviour->closeAutomatically, s);

    behaviour->openOnEvent = getValue(obj.dict, "open_on_event", "");

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructSwitch
//===========================================
bool MiscFactory::constructSwitch(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("wall_decal", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);
    FocusSystem& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

    entityId_t target = Component::getIdFromString(getValue(obj.dict, "target"));
    bool toggleable = getValue(obj.dict, "toggleable", "false") == "true";
    double toggleDelay = std::stod(getValue(obj.dict, "toggle_delay", "0.0"));

    SwitchState initialState = SwitchState::OFF;
    if (getValue(obj.dict, "initial_state", "") == "on") {
      initialState = SwitchState::ON;
    }

    string message = getValue(obj.dict, "message", "");

    CSwitchBehaviour* behaviour = new CSwitchBehaviour(entityId, m_entityManager, m_timeService,
      target, message, initialState, toggleable, toggleDelay);

    behaviour->requiredItemType = getValue(obj.dict, "required_item_type", "");
    behaviour->requiredItemName = getValue(obj.dict, "required_item_name", "");

    behaviourSystem.addComponent(pComponent_t(behaviour));

    string captionText = getValue(obj.dict, "caption", "");
    if (captionText != "") {
      CFocus* focus = new CFocus(entityId);
      focus->captionText = captionText;

      focusSystem.addComponent(pComponent_t(focus));
    }

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructElevator
//===========================================
bool MiscFactory::constructElevator(entityId_t entityId, parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform) {

  if (entityId == -1) {
    entityId = makeIdForObj(obj);
  }

  if (m_rootFactory.constructObject("region", entityId, obj, parentId, parentTransform)) {
    BehaviourSystem& behaviourSystem =
      m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);

    vector<string> strLevels = splitString(getValue(obj.dict, "levels"), ',');
    vector<double> levels(strLevels.size());
    std::transform(strLevels.begin(), strLevels.end(), levels.begin(), [](const string& s) {
      return std::stod(s);
    });

    CElevatorBehaviour* behaviour = new CElevatorBehaviour(entityId, m_entityManager,
      m_timeService.frameRate, levels);

    if (contains<string>(obj.dict, "speed")) {
      double speed = std::stod(obj.dict.at("speed"));
      behaviour->setSpeed(speed);
    }

    behaviourSystem.addComponent(pComponent_t(behaviour));

    return true;
  }

  return false;
}

//===========================================
// MiscFactory::constructObject
//===========================================
bool MiscFactory::constructObject(const string& type, entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

  if (type == "player_inventory") {
    return constructPlayerInventory();
  }
  else if (type == "player") {
    return constructPlayer(obj, parentId, parentTransform);
  }
  else if (type == "door") {
    return constructDoor(entityId, obj, parentId, parentTransform);
  }
  else if (type == "switch") {
    return constructSwitch(entityId, obj, parentId, parentTransform);
  }
  else if (type == "elevator") {
    return constructElevator(entityId, obj, parentId, parentTransform);
  }
  else if (type == "spawn_point") {
    return constructSpawnPoint(entityId, obj, parentId, parentTransform);
  }
  else if (type == "collectable_item") {
    return constructCollectableItem(entityId, obj, parentId, parentTransform);
  }

  return false;
}
