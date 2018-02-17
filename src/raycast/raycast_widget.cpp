#include <list>
#include <regex>
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
// loadTextures
//===========================================
static void loadTextures(RenderGraph& rg, const parser::Object& obj) {
  for (auto it = obj.dict.begin(); it != obj.dict.end(); ++it) {
    string name = it->first;
    string details = it->second;

    Size sz(100, 100);

    std::regex rx("([a-zA-Z0-9_\\.\\/]+)(?:,(\\d+),(\\d+))?");
    std::smatch m;

    std::regex_match(details, m, rx);
    if (m.size() == 0) {
      EXCEPTION("Error parsing texture description for texture with name '" << name << "'");
    }

    string path = m.str(1);

    if (!m.str(2).empty()) {
      sz.x = std::stod(m.str(2));
    }
    if (!m.str(3).empty()) {
      sz.y = std::stod(m.str(3));
    }

    rg.textures[name] = Texture{QImage(path.c_str()), sz};
  }
}

//===========================================
// loadMusicAssets
//===========================================
static void loadMusicAssets(AudioService& audioService, const parser::Object& obj) {
  for (auto it = obj.dict.begin(); it != obj.dict.end(); ++it) {
    audioService.addMusicTrack(it->first, it->second);
  }
}

//===========================================
// loadSoundAssets
//===========================================
static void loadSoundAssets(AudioService& audioService, const parser::Object& obj) {
  for (auto it = obj.dict.begin(); it != obj.dict.end(); ++it) {
    audioService.addSound(it->first, it->second);
  }
}

//===========================================
// configureAudioService
//===========================================
static void configureAudioService(AudioService& audioService, const parser::Object& obj) {
  parser::Object* pObj = firstObjectOfType(obj.children, "music_assets");
  if (pObj != nullptr) {
    loadMusicAssets(audioService, *pObj);
  }

  pObj = firstObjectOfType(obj.children, "sound_assets");
  if (pObj != nullptr) {
    loadSoundAssets(audioService, *pObj);
  }

  string musicTrack = getValue(obj.dict, "music_track", "");

  string strMusicVolume = getValue(obj.dict, "music_volume", "1.0");
  double musicVolume = std::stod(strMusicVolume);

  if (musicTrack.length() > 0) {
    std::cout << musicTrack << ", " << musicVolume << "\n";

    audioService.playMusic(musicTrack);
    audioService.setMusicVolume(musicVolume);
  }
}

//===========================================
// configure
//===========================================
static void configure(RenderGraph& rg, AudioService& audioService, const parser::Object& config) {
  parser::Object* pObj = firstObjectOfType(config.children, "texture_assets");
  if (pObj != nullptr) {
    loadTextures(rg, *pObj);
  }

  pObj = firstObjectOfType(config.children, "audio_config");
  if (pObj != nullptr) {
    configureAudioService(audioService, *pObj);
  }
}

//===========================================
// RaycastWidget::loadMap
//===========================================
void RaycastWidget::loadMap(const string& mapFilePath) {
  RenderSystem& renderSystem = m_entityManager.system<RenderSystem&>(ComponentKind::C_RENDER);
  RenderGraph& rg = renderSystem.rg;

  list<parser::pObject_t> objects;
  parser::parse(mapFilePath, objects);

  // A config object and the root region
  assert(objects.size() == 2);

  parser::Object& config = *firstObjectOfType(objects, "config");
  parser::Object& rootRegion = *firstObjectOfType(objects, "region");

  configure(rg, m_audioService, config);

  m_rootFactory->constructObject("region", -1, rootRegion, -1, Matrix());
  m_rootFactory->constructObject("player_inventory", -1, parser::Object(), -1, Matrix());
}

//===========================================
// RaycastWidget::initialise
//===========================================
void RaycastWidget::initialise(const string& mapFile) {
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

  loadMap(mapFile);

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
    setFocus();

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
