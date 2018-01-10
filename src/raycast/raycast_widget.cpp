#include <list>
#include <QMessageBox>
#include <QMenuBar>
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>
#include "event_system.hpp"
#include "raycast/raycast_widget.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/map_parser.hpp"


#ifdef DEBUG
namespace chrono = std::chrono;
#endif

using std::string;
using std::list;


const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int FRAME_RATE = 60;

const double PLAYER_SPEED = 250.0;


//===========================================
// RaycastWidget::RaycastWidget
//===========================================
RaycastWidget::RaycastWidget(QWidget* parent, EventSystem& eventSystem)
  : QWidget(parent),
    m_eventSystem(eventSystem),
    m_timeService(FRAME_RATE),
    m_audioService(m_entityManager) {}

//===========================================
// RaycastWidget::loadMap
//===========================================
void RaycastWidget::loadMap(const string& mapFilePath) {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem&>(ComponentKind::C_RENDER);
  RenderGraph& rg = renderSystem.rg;

  // TODO: Read from map file
  rg.textures["default"] = Texture{QImage("data/default.png"), Size(100, 100)};
  rg.textures["light_bricks"] = Texture{QImage("data/light_bricks.png"), Size(100, 100)};
  rg.textures["dark_bricks"] = Texture{QImage("data/dark_bricks.png"), Size(100, 100)};
  rg.textures["door"] = Texture{QImage("data/door.png"), Size(100, 100)};
  rg.textures["cracked_mud"] = Texture{QImage("data/cracked_mud.png"), Size(100, 100)};
  rg.textures["dirt"] = Texture{QImage("data/dirt.png"), Size(100, 100)};
  rg.textures["crate"] = Texture{QImage("data/crate.png"), Size(30, 30)};
  rg.textures["grey_stone"] = Texture{QImage("data/grey_stone.png"), Size(100, 100)};
  rg.textures["stone_slabs"] = Texture{QImage("data/stone_slabs.png"), Size(100, 100)};
  rg.textures["ammo"] = Texture{QImage("data/ammo.png"), Size(100, 100)};
  rg.textures["bad_guy"] = Texture{QImage("data/bad_guy.png"), Size(100, 100)};
  rg.textures["sky"] = Texture{QImage("data/sky.png"), Size()};
  rg.textures["beer"] = Texture{QImage("data/beer.png"), Size()};
  rg.textures["gun"] = Texture{QImage("data/gun.png"), Size(100, 100)};
  rg.textures["crosshair"] = Texture{QImage("data/crosshair.png"), Size(32, 32)};
  rg.textures["switch"] = Texture{QImage("data/switch.png"), Size()};

  m_audioService.addSound("pistol_shoot", "data/pistol_shoot.wav");
  m_audioService.addSound("shotgun_shoot", "data/shotgun_shoot.wav");
  m_audioService.addSound("monster_hurt", "data/monster_hurt.wav");
  m_audioService.addSound("monster_death", "data/monster_death.wav");
  m_audioService.addSound("ammo_collect", "data/ammo_collect.wav");
  m_audioService.addSound("click", "data/click.wav");

  m_audioService.addMusicTrack("loop1", "data/loop1.mp3");
  m_audioService.addMusicTrack("loop2", "data/loop2.mp3");
  m_audioService.addMusicTrack("loop3", "data/loop3.mp3");

  m_audioService.setMusicVolume(0.0); // TODO
  m_audioService.playMusic("loop3");

  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  assert(objects.size() == 1);
  m_rootFactory->constructObject("region", -1, *objects.front(), -1, Matrix());
  m_rootFactory->constructObject("player_inventory", -1, parser::Object(), -1, Matrix());
}

//===========================================
// RaycastWidget::initialise
//===========================================
void RaycastWidget::initialise() {
  m_rootFactory.reset(new RootFactory(m_entityManager, m_audioService, m_timeService));

  setMouseTracking(true);

  m_defaultCursor = cursor().shape();
  m_cursorCaptured = false;
  m_mouseBtnState = false;

  m_buffer = QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_ARGB32);

  setFocus();

  BehaviourSystem* behaviourSystem = new BehaviourSystem;
  m_entityManager.addSystem(ComponentKind::C_BEHAVIOUR, pSystem_t(behaviourSystem));

  RenderSystem* renderSystem = new RenderSystem(m_entityManager, m_buffer);
  m_entityManager.addSystem(ComponentKind::C_RENDER, pSystem_t(renderSystem));

  SpatialSystem* spatialSystem = new SpatialSystem(m_entityManager, m_timeService, FRAME_RATE);
  m_entityManager.addSystem(ComponentKind::C_SPATIAL, pSystem_t(spatialSystem));

  AnimationSystem* animationSystem = new AnimationSystem(m_entityManager, FRAME_RATE);
  m_entityManager.addSystem(ComponentKind::C_ANIMATION, pSystem_t(animationSystem));

  InventorySystem* inventorySystem = new InventorySystem(m_entityManager);
  m_entityManager.addSystem(ComponentKind::C_INVENTORY, pSystem_t(inventorySystem));

  EventHandlerSystem* eventHandlerSystem = new EventHandlerSystem;
  m_entityManager.addSystem(ComponentKind::C_EVENT_HANDLER, pSystem_t(eventHandlerSystem));

  DamageSystem* damageSystem = new DamageSystem(m_entityManager);
  m_entityManager.addSystem(ComponentKind::C_DAMAGE, pSystem_t(damageSystem));

  loadMap("data/maps/house3.svg");

  m_timer.reset(new QTimer(this));
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));

  m_timer->start(1000 / FRAME_RATE);
}

//===========================================
// RaycastWidget::paintEvent
//===========================================
void RaycastWidget::paintEvent(QPaintEvent*) {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

  renderSystem.render();

  QPainter painter;
  painter.begin(this);
  painter.drawImage(rect(), m_buffer);
  painter.end();
}

//===========================================
// RaycastWidget::keyPressEvent
//===========================================
void RaycastWidget::keyPressEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = true;

  if (event->key() == Qt::Key_F) {
    DBG_PRINT("Frame rate = " << m_frameRate << "\n");
  }
  else if (event->key() == Qt::Key_Escape) {
    m_cursorCaptured = false;
    setCursor(m_defaultCursor);
  }
}

//===========================================
// RaycastWidget::keyReleaseEvent
//===========================================
void RaycastWidget::keyReleaseEvent(QKeyEvent* event) {
  m_keyStates[event->key()] = false;
}

//===========================================
// RaycastWidget::mousePressEvent
//===========================================
void RaycastWidget::mousePressEvent(QMouseEvent* event) {
  if (m_cursorCaptured && event->button() == Qt::LeftButton) {
    m_mouseBtnState = true;
  }

  m_cursorCaptured = true;
  m_cursor.x = width() / 2;
  m_cursor.y = height() / 2;

  setCursor(Qt::BlankCursor);
}

//===========================================
// RaycastWidget::mouseReleaseEvent
//===========================================
void RaycastWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_mouseBtnState = false;
  }
}

//===========================================
// RaycastWidget::mouseMoveEvent
//===========================================
void RaycastWidget::mouseMoveEvent(QMouseEvent* event) {
  m_cursor.x = event->x();
  m_cursor.y = event->y();
}

//===========================================
// RaycastWidget::tick
//===========================================
void RaycastWidget::tick() {
#ifdef DEBUG
  if (m_frame % 10 == 0) {
    chrono::high_resolution_clock::time_point t_ = chrono::high_resolution_clock::now();
    chrono::duration<double> span = chrono::duration_cast<chrono::duration<double>>(t_ - m_t);
    m_frameRate = 10.0 / span.count();
    m_t = t_;
  }
  ++m_frame;
#endif

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  m_entityManager.purgeEntities();
  m_entityManager.update();

  m_timeService.update();

  if (m_keyStates[Qt::Key_E]) {
    spatialSystem.jump();
  }

  if (m_keyStates[Qt::Key_Space]) {
    GameEvent e("playerActivate");
    spatialSystem.handleEvent(e);

    m_keyStates[Qt::Key_Space] = false;
  }

  Vec2f v; // The vector is in camera space
  if (m_keyStates[Qt::Key_A]) {
    v.y -= 1;
  }
  if (m_keyStates[Qt::Key_D]) {
    v.y += 1;
  }
  if (m_keyStates[Qt::Key_W] || m_keyStates[Qt::Key_Up]) {
    v.x += 1;
  }
  if (m_keyStates[Qt::Key_S] || m_keyStates[Qt::Key_Down]) {
    v.x -= 1;
  }

  if (v.x != 0 || v.y != 0) {
    double ds = PLAYER_SPEED / FRAME_RATE;
    spatialSystem.movePlayer(normalise(v) * ds);
  }

  if (m_keyStates[Qt::Key_Left]) {
    spatialSystem.hRotateCamera(-(1.2 / FRAME_RATE) * PI);
  }
  if (m_keyStates[Qt::Key_Right]) {
    spatialSystem.hRotateCamera((1.2 / FRAME_RATE) * PI);
  }

  if (m_cursorCaptured) {
    Point centre(width() / 2, height() / 2);

    // Y-axis is top to bottom
    Point v(m_cursor.x - centre.x, centre.y - m_cursor.y);

    if (v.x != 0 || v.y != 0) {
      QCursor::setPos(mapToGlobal(QPoint(centre.x, centre.y)));
      m_cursor = centre;
    }

    if (fabs(v.x) > 0) {
      double da = 0.0006 * PI * v.x;
      spatialSystem.hRotateCamera(da);
    }

    if (fabs(v.y) > 0) {
      double da = 0.0006 * PI * v.y;
      spatialSystem.vRotateCamera(da);
    }

    if (m_mouseBtnState == true) {
      spatialSystem.sg.player->shoot();
      m_mouseBtnState = false;
    }
  }

  update();
}
