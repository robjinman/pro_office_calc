#include <vector>
#include <sstream>
#include "raycast/player.hpp"
#include "raycast/camera.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/time_service.hpp"
#include "raycast/c_player_behaviour.hpp"


using std::vector;
using std::string;
using std::stringstream;


const double FOREHEAD_SIZE = 15.0;
const double COLLISION_RADIUS = 10.0;

const double HUD_H = 1.0;
const double INVENTORY_H = 1.2;
const double V_MARGIN = 0.15;


//===========================================
// Player::Player
//===========================================
Player::Player(EntityManager& entityManager, AudioService& audioService, TimeService& timeService,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform)
  : m_entityManager(entityManager),
    m_audioService(audioService),
    m_timeService(timeService),
    m_shootTimer(0.5) {

  auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  auto& animationSystem = m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
  auto& damageSystem = m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  auto& behaviourSystem = m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);
  auto& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
  auto& inventorySystem = m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);

  constructPlayer(obj, parentId, parentTransform, spatialSystem, renderSystem, animationSystem,
    damageSystem, behaviourSystem, eventHandlerSystem);
  constructInventory(renderSystem, inventorySystem, eventHandlerSystem, damageSystem);
  setupHudShowHide(renderSystem, eventHandlerSystem);
}

//===========================================
// makeTween
//===========================================
static void makeTween(RenderSystem& renderSystem, TimeService& timeService, const string& name,
  double duration, entityId_t overlayId, double fromY, double toY) {

  if (renderSystem.hasComponent(overlayId)) {
    auto& overlay = dynamic_cast<COverlay&>(renderSystem.getComponent(overlayId));

    double s = toY - fromY;
    double ds = s / (duration * timeService.frameRate);

    Tween tween{
      [=, &overlay](long, double, double) -> bool {

      overlay.pos.y += ds;

      if (fabs(overlay.pos.y - fromY) >= fabs(s)) {
        overlay.pos.y = toY;
        return false;
      }
      return true;
    },
    [](long, double, double) {}};

    timeService.addTween(tween, name);
  }
}

//===========================================
// Player::setupHudShowHide
//===========================================
void Player::setupHudShowHide(RenderSystem& renderSystem, EventHandlerSystem& eventHandlerSystem) {
  // Transition time
  const double T = 0.3;

  const Size& viewport = renderSystem.rg.viewport;

  CEventHandler& events = eventHandlerSystem.getComponent(this->body);
  events.broadcastedEventHandlers.push_back(EventHandler{"mouse_captured",
    [=, &renderSystem, &viewport] (const GameEvent&) {

    m_timeService.removeTween("gunSlideOut");
    makeTween(renderSystem, m_timeService, "gunSlideIn", T, this->sprite, -4.0, 0.0);

    m_timeService.removeTween("ammoSlideOut");
    makeTween(renderSystem, m_timeService, "ammoSlideId", T, m_ammoId, viewport.y + V_MARGIN,
      viewport.y - HUD_H + V_MARGIN);

    m_timeService.removeTween("healthSlideOut");
    makeTween(renderSystem, m_timeService, "healthSlideIn", T, m_healthId, viewport.y + V_MARGIN,
      viewport.y - HUD_H + V_MARGIN);

    m_timeService.removeTween("itemsSlideOut");
    makeTween(renderSystem, m_timeService, "itemsSlideIn", T, m_itemsId, -INVENTORY_H, 0.0);

    m_timeService.removeTween("hudBgSlideOut");
    makeTween(renderSystem, m_timeService, "hudBgSlideIn", T, m_hudBgId, viewport.y,
      viewport.y - HUD_H);

    if (renderSystem.hasComponent(this->crosshair)) {
      auto& crosshair = dynamic_cast<CImageOverlay&>(renderSystem.getComponent(this->crosshair));
      crosshair.pos = viewport / 2 - Vec2f(0.5, 0.5) / 2;
    }
  }});
  events.broadcastedEventHandlers.push_back(EventHandler{"mouse_uncaptured",
    [=, &renderSystem, &viewport](const GameEvent&) {

    m_timeService.removeTween("gunSlideIn");
    makeTween(renderSystem, m_timeService, "gunSlideOut", T, this->sprite, 0.0, -4.0);

    m_timeService.removeTween("ammoSlideIn");
    makeTween(renderSystem, m_timeService, "ammoSlideOut", T, m_ammoId,
      viewport.y - HUD_H + V_MARGIN, viewport.y + V_MARGIN);

    m_timeService.removeTween("healthSlideIn");
    makeTween(renderSystem, m_timeService, "healthSlideOut", T, m_healthId,
      viewport.y - HUD_H + V_MARGIN, viewport.y + V_MARGIN);

    m_timeService.removeTween("itemsSlideIn");
    makeTween(renderSystem, m_timeService, "itemsSlideOut", T, m_itemsId, 0.0, -INVENTORY_H);

    m_timeService.removeTween("hudBgSlideIn");
    makeTween(renderSystem, m_timeService, "hudBgSlideOut", T, m_hudBgId, viewport.y - HUD_H,
      viewport.y);

    if (renderSystem.hasComponent(this->crosshair)) {
      auto& crosshair = dynamic_cast<CImageOverlay&>(renderSystem.getComponent(this->crosshair));
      crosshair.pos = Point(10, 10);
    }
  }});
}

//===========================================
// Player::constructPlayer
//===========================================
void Player::constructPlayer(const parser::Object& obj, entityId_t parentId,
  const Matrix& parentTransform, SpatialSystem& spatialSystem, RenderSystem& renderSystem,
  AnimationSystem& animationSystem, DamageSystem& damageSystem, BehaviourSystem& behaviourSystem,
  EventHandlerSystem& eventHandlerSystem) {

  double tallness = std::stod(getValue(obj.dict, "tallness"));

  this->body = Component::getIdFromString("player");

  CZone& zone = dynamic_cast<CZone&>(spatialSystem.getComponent(parentId));

  Matrix m = parentTransform * obj.groupTransform * obj.pathTransform
    * transformFromTriangle(obj.path);

  CVRect* b = new CVRect(body, zone.entityId(), Size(0, 0));
  b->setTransform(m);
  b->zone = &zone;
  b->size.x = 60.0;
  b->size.y = tallness + FOREHEAD_SIZE;

  spatialSystem.addComponent(pCSpatial_t(b));

  m_camera.reset(new Camera(renderSystem.rg.viewport.x, DEG_TO_RAD(60), DEG_TO_RAD(50), *b,
    tallness - FOREHEAD_SIZE + zone.floorHeight));

  this->sprite = Component::getNextId();
  this->crosshair = Component::getNextId();

  CSprite* bodySprite = new CSprite(this->body, parentId, "player");
  renderSystem.addComponent(pComponent_t(bodySprite));

  // Number of frames in sprite sheet
  const int W = 8;
  const int H = 14;

  CAnimation* bodyAnims = new CAnimation(this->body);

  vector<AnimationFrame> runFrames = constructFrames(W, H,
    { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 });
  bodyAnims->addAnimation(pAnimation_t(new Animation("run", m_timeService.frameRate, 1.0,
    runFrames)));

  vector<AnimationFrame> shootFrames = constructFrames(W, H, { 1, 0 });
  bodyAnims->addAnimation(pAnimation_t(new Animation("shoot", m_timeService.frameRate, 1.0,
    shootFrames)));

  vector<AnimationFrame> idleFrames = constructFrames(W, H, { 0 });
  bodyAnims->addAnimation(pAnimation_t(new Animation("idle", m_timeService.frameRate, 1.0,
    idleFrames)));

  animationSystem.addComponent(pComponent_t(bodyAnims));
  animationSystem.playAnimation(this->body, "idle", true);

  CDamage* damage = new CDamage(this->body, 10, 10);
  damageSystem.addComponent(pCDamage_t(damage));

  CPlayerBehaviour* behaviour = new CPlayerBehaviour(this->body, m_entityManager, m_timeService);
  behaviourSystem.addComponent(pComponent_t(behaviour));

  const Size& viewport = renderSystem.rg.viewport;

  Size sz(0.5, 0.5);
  CImageOverlay* crosshair = new CImageOverlay(this->crosshair, "crosshair", Point(10, 10), sz);
  renderSystem.addComponent(pCRender_t(crosshair));

  CImageOverlay* gunSprite = new CImageOverlay(this->sprite, "gun", Point(viewport.x * 0.5, -4),
    Size(4, 4));
  gunSprite->texRect = QRectF(0, 0, 0.25, 1);
  renderSystem.addComponent(pCRender_t(gunSprite));

  CAnimation* shoot = new CAnimation(this->sprite);
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

  CEventHandler* events = new CEventHandler(this->body);
  events->targetedEventHandlers.push_back(EventHandler{"player_move",
    [=, &animationSystem](const GameEvent&) {

    if (animationSystem.animationState(this->body, "run") == AnimState::STOPPED) {
      animationSystem.playAnimation(this->body, "run", false);
    }
  }});
  events->targetedEventHandlers.push_back(EventHandler{"animation_finished",
    [=, &animationSystem](const GameEvent& e_) {

    const EAnimationFinished& e = dynamic_cast<const EAnimationFinished&>(e_);
    if (e.animName == "run") {
      animationSystem.playAnimation(this->body, "idle", true);
    }
  }});

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// Player::constructInventory
//===========================================
void Player::constructInventory(RenderSystem& renderSystem, InventorySystem& inventorySystem,
  EventHandlerSystem& eventHandlerSystem, DamageSystem& damageSystem) {

  const RenderGraph& rg = renderSystem.rg;
  const Size& viewport = rg.viewport;

  CCollector* inventory = new CCollector(this->body);
  inventory->buckets["ammo"] = pBucket_t(new CounterBucket(50));
  inventory->buckets["item"] = pBucket_t(new ItemBucket(5));
  inventorySystem.addComponent(pComponent_t(inventory));

  const double INVENTORY_W = 6.0;
  double itemsDisplayAspectRatio = INVENTORY_W / INVENTORY_H;
  double itemsDisplayH_px = INVENTORY_H * rg.hWorldUnit_px;
  double itemsDisplayW_px = itemsDisplayH_px * itemsDisplayAspectRatio;

  QImage imgItems(itemsDisplayW_px, itemsDisplayH_px, QImage::Format_ARGB32);
  imgItems.fill(Qt::GlobalColor::transparent);

  renderSystem.rg.textures["items_display"] = Texture{imgItems, Size(0, 0)};

  m_ammoId = Component::getNextId();
  m_healthId = Component::getNextId();
  m_itemsId = Component::getNextId();

  // Start everything off-screen
  //

  CImageOverlay* itemsDisplay = new CImageOverlay(m_itemsId, "items_display",
    Point(0, -INVENTORY_H), Size(INVENTORY_W, INVENTORY_H), 1);
  renderSystem.addComponent(pComponent_t(itemsDisplay));

  double hMargin = 0.15;

  CTextOverlay* ammoCounter = new CTextOverlay(m_ammoId, "AMMO 0/50",
    Point(hMargin, viewport.y + V_MARGIN), HUD_H - 2.0 * V_MARGIN, Qt::green, 2);
  renderSystem.addComponent(pComponent_t(ammoCounter));

  CTextOverlay* healthCounter = new CTextOverlay(m_healthId, "HEALTH 10/10",
    Point(0, viewport.y + V_MARGIN), HUD_H - 2.0 * V_MARGIN, Qt::red, 2);
  healthCounter->pos.x = viewport.x - renderSystem.textOverlayWidth(*healthCounter) - hMargin;
  renderSystem.addComponent(pComponent_t(healthCounter));

  CEventHandler& events = eventHandlerSystem.getComponent(this->body);
  events.targetedEventHandlers.push_back(EventHandler{"bucket_count_change",
    [=, &damageSystem](const GameEvent& e_) {

    auto& e = dynamic_cast<const EBucketCountChange&>(e_);

    if (e.collectableType == "ammo") {
      stringstream ss;
      ss << "AMMO " << e.bucket.count << "/" << e.bucket.capacity;
      ammoCounter->text = ss.str();

      if (e.bucket.count > e.prevCount) {
        m_audioService.playSound("ammo_collect");
      }
    }
  }});
  events.targetedEventHandlers.push_back(EventHandler{"collectable_encountered",
    [this, &damageSystem](const GameEvent& e_) {

    auto& e = dynamic_cast<const ECollectableEncountered&>(e_);

    if (e.item.collectableType == "health_pack") {
      if (damageSystem.getHealth(this->body) < damageSystem.getMaxHealth(this->body)) {
        DBG_PRINT("Health pack collected\n");

        m_audioService.playSound("health_pack_collect");
        damageSystem.damageEntity(this->body, -e.item.value);

        m_entityManager.deleteEntity(e.item.entityId());
      }
    }
  }});
  events.targetedEventHandlers.push_back(EventHandler{"bucket_items_change",
    [=, &renderSystem, &rg](const GameEvent& e_) {

    const EBucketItemsChange& e = dynamic_cast<const EBucketItemsChange&>(e_);

    if (e.collectableType == "item") {
      QImage& target = renderSystem.rg.textures["items_display"].image;
      target.fill(Qt::GlobalColor::transparent);

      QPainter painter;
      painter.begin(&target);

      int i = 0;
      for (auto it = e.bucket.items.rbegin(); it != e.bucket.items.rend(); ++it) {
        entityId_t id = it->second;
        const CRender& c = dynamic_cast<const CRender&>(renderSystem.getComponent(id));

        if (c.kind == CRenderKind::SPRITE) {
          const CSprite& sprite = dynamic_cast<const CSprite&>(c);
          const QImage& img = renderSystem.rg.textures.at(sprite.texture).image;

          double slotH = itemsDisplayW_px;
          double slotW = itemsDisplayW_px / e.bucket.capacity;
          double slotX = slotW * i;
          double slotY = 0;
          double vMargin = V_MARGIN * rg.hWorldUnit_px;
          double hMargin = 0.1 * rg.hWorldUnit_px;
          double aspectRatio = static_cast<double>(img.width()) / img.height();
          double maxH = slotH - vMargin * 2.0;
          double maxW = slotW - hMargin * 2.0;
          double h = maxH;
          double w = h * aspectRatio;
          double s = smallest(maxH / h, maxW / w);
          h *= s;
          w *= s;

          QRect srcRect(0, 0, img.width(), img.height());
          QRect trgRect(slotX + hMargin, slotY + vMargin, w, h);

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
  events.targetedEventHandlers.push_back(EventHandler{"entity_damaged",
    [=, &damageSystem](const GameEvent&) {

    const CDamage& damage = dynamic_cast<const CDamage&>(damageSystem.getComponent(this->body));

    stringstream ss;
    ss << "HEALTH " << damage.health << "/" << damage.maxHealth;
    healthCounter->text = ss.str();
  }});

  m_hudBgId = Component::getNextId();

  CColourOverlay* bg = new CColourOverlay(m_hudBgId, QColor(0, 0, 0, 100), Point(0, viewport.y),
    Size(viewport.x, HUD_H), 1);

  renderSystem.addComponent(pComponent_t(bg));
}

//===========================================
// Player::region
//===========================================
entityId_t Player::region() const {
  return getBody().zone->entityId();
}

//===========================================
// Player::aboveGround
//===========================================
bool Player::aboveGround() const {
  return feetHeight() - 0.1 > getBody().zone->floorHeight;
}

//===========================================
// Player::belowGround
//===========================================
bool Player::belowGround() const {
  return feetHeight() + 0.1 < getBody().zone->floorHeight;
}

//===========================================
// Player::feetHeight
//===========================================
double Player::feetHeight() const {
  return eyeHeight() + FOREHEAD_SIZE - getTallness();
}

//===========================================
// Player::headHeight
//===========================================
double Player::headHeight() const {
  return eyeHeight() + FOREHEAD_SIZE;
}

//===========================================
// Player::eyeHeight
//===========================================
double Player::eyeHeight() const {
  return m_camera->height;
}

//===========================================
// Player::getTallness
//===========================================
double Player::getTallness() const {
  return getBody().size.y;
}

//===========================================
// Player::changeTallness
//===========================================
void Player::changeTallness(double delta) {
  getBody().size.y += delta;
  m_camera->height += delta;
}

//===========================================
// Player::setFeetHeight
//===========================================
void Player::setFeetHeight(double h) {
  m_camera->height = h + getTallness() - FOREHEAD_SIZE;
}

//===========================================
// Player::setEyeHeight
//===========================================
void Player::setEyeHeight(double h) {
  m_camera->height = h;
}

//===========================================
// Player::changeHeight
//===========================================
void Player::changeHeight(const CZone& zone, double deltaH) {
  // If applying this delta puts the player's feet through the floor
  if (feetHeight() + deltaH < zone.floorHeight) {
    // Only permit positive delta
    if (deltaH <= 0) {
      // Reset to floor height
      setFeetHeight(zone.floorHeight);
      return;
    }
  }
  // If applying this delta puts the player's head through the ceiling
  else if (zone.hasCeiling && (headHeight() + deltaH > zone.ceilingHeight)) {
    // Only permit negative delta
    if (deltaH >= 0) {
      vVelocity = 0;
      return;
    }
  }
  m_camera->height += deltaH;
}

//===========================================
// Player::pos
//===========================================
const Point& Player::pos() const {
  return getBody().pos;
}

//===========================================
// Player::dir
//===========================================
Vec2f Player::dir() const {
  return Vec2f(cos(getBody().angle), sin(getBody().angle));
}

//===========================================
// Player::getBody
//===========================================
CVRect& Player::getBody() const {
  return m_entityManager.getComponent<CVRect>(body, ComponentKind::C_SPATIAL);
}

//===========================================
// Player::setPosition
//===========================================
void Player::setPosition(const Point& pos) {
  getBody().pos = pos;
}

//===========================================
// Player::hRotate
//===========================================
void Player::hRotate(double da) {
  getBody().angle += da;
}

//===========================================
// Player::vRotate
//===========================================
void Player::vRotate(double da) {
  if (fabs(m_camera->vAngle + da) <= DEG_TO_RAD(20)) {
    m_camera->vAngle += da;
  }
}

//===========================================
// Player::shoot
//===========================================
void Player::shoot() {
  if (m_shootTimer.ready()) {
    InventorySystem& inventorySystem = m_entityManager
      .system<InventorySystem>(ComponentKind::C_INVENTORY);
    AnimationSystem& animationSystem = m_entityManager
      .system<AnimationSystem>(ComponentKind::C_ANIMATION);

    if (inventorySystem.getBucketValue(body, "ammo") > 0) {
      DamageSystem& damageSystem = m_entityManager
        .system<DamageSystem>(ComponentKind::C_DAMAGE);

      animationSystem.playAnimation(sprite, "shoot", false);
      animationSystem.playAnimation(body, "shoot", false);
      m_audioService.playSound("pistol_shoot");
      inventorySystem.subtractFromBucket(body, "ammo", 1);
      damageSystem.damageAtIntersection(Vec2f(1, 0), 0, 1);
    }
    else {
      animationSystem.playAnimation(sprite, "shoot_no_ammo", false);
      m_audioService.playSound("click");
    }
  }
}

//===========================================
// Player::camera
//===========================================
const Camera& Player::camera() const {
  return *m_camera;
}
