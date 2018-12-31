#include <list>
#include <regex>
#include <cassert>
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
#include "raycast/spawn_system.hpp"
#include "raycast/agent_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/map_parser.hpp"
#include "raycast/misc_factory.hpp"
#include "raycast/sprite_factory.hpp"
#include "raycast/geometry_factory.hpp"
#include "raycast/game_event.hpp"
#include "app_config.hpp"


#ifdef DEBUG
namespace chrono = std::chrono;
#endif

using std::string;
using std::list;


static const double PLAYER_SPEED = 350.0;
static const double MOUSE_LOOK_SPEED = 0.0006;
static const double KEY_LOOK_SPEED = 1.2;


//===========================================
// RaycastWidget::RaycastWidget
//===========================================
RaycastWidget::RaycastWidget(const AppConfig& appConfig, EventSystem& eventSystem, int width,
  int height, int frameRate)
  : QWidget(nullptr),
    m_appConfig(appConfig),
    m_eventSystem(eventSystem),
    m_timeService(frameRate),
    m_audioService(m_entityManager, m_timeService),
    m_width(width),
    m_height(height),
    m_frameRate(frameRate) {}

//===========================================
// RaycastWidget::loadTextures
//===========================================
void RaycastWidget::loadTextures(RenderGraph& rg, const parser::Object& obj) {
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

    rg.textures[name] = Texture{QImage(m_appConfig.dataPath(path).c_str()), sz};
  }
}

//===========================================
// RaycastWidget::loadMusicAssets
//===========================================
void RaycastWidget::loadMusicAssets(const parser::Object& obj) {
  for (auto it = obj.dict.begin(); it != obj.dict.end(); ++it) {
    m_audioService.addMusicTrack(it->first, m_appConfig.dataPath(it->second));
  }
}

//===========================================
// RaycastWidget::loadSoundAssets
//===========================================
void RaycastWidget::loadSoundAssets(const parser::Object& obj) {
  for (auto it = obj.dict.begin(); it != obj.dict.end(); ++it) {
    m_audioService.addSound(it->first, m_appConfig.dataPath(it->second));
  }
}

//===========================================
// RaycastWidget::configureAudioService
//===========================================
void RaycastWidget::configureAudioService(const parser::Object& obj) {
  parser::Object* pObj = firstObjectOfType(obj.children, "music_assets");
  if (pObj != nullptr) {
    loadMusicAssets(*pObj);
  }

  pObj = firstObjectOfType(obj.children, "sound_assets");
  if (pObj != nullptr) {
    loadSoundAssets(*pObj);
  }

  string musicTrack = getValue(obj.dict, "music_track", "");

  string strMusicVolume = getValue(obj.dict, "music_volume", "1.0");
  double musicVolume = std::stod(strMusicVolume);

  bool loop = getValue(obj.dict, "loop", "true") == "true";

  if (musicTrack.length() > 0) {
    m_audioService.playMusic(musicTrack, loop);
    m_audioService.setMusicVolume(musicVolume);
  }
}

//===========================================
// RaycastWidget::configure
//===========================================
void RaycastWidget::configure(RenderGraph& rg, const parser::Object& config) {
  parser::Object* pObj = firstObjectOfType(config.children, "texture_assets");
  if (pObj != nullptr) {
    loadTextures(rg, *pObj);
  }

  pObj = firstObjectOfType(config.children, "audio_config");
  if (pObj != nullptr) {
    configureAudioService(*pObj);
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

  configure(rg, config);

  m_rootFactory.constructObject("region", -1, rootRegion, -1, Matrix());
}

//===========================================
// RaycastWidget::setupObjectFactories
//===========================================
void RaycastWidget::setupObjectFactories() {
  m_rootFactory.addFactory(pGameObjectFactory_t(new MiscFactory(m_rootFactory, m_entityManager,
    m_audioService, m_timeService)));
  m_rootFactory.addFactory(pGameObjectFactory_t(new SpriteFactory(m_rootFactory, m_entityManager,
    m_audioService, m_timeService)));
  m_rootFactory.addFactory(pGameObjectFactory_t(new GeometryFactory(m_rootFactory,
    m_entityManager)));
}

//===========================================
// RaycastWidget::setupSystems
//===========================================
void RaycastWidget::setupSystems() {
  BehaviourSystem* behaviourSystem = new BehaviourSystem;
  m_entityManager.addSystem(ComponentKind::C_BEHAVIOUR, pSystem_t(behaviourSystem));

  SpatialSystem* spatialSystem = new SpatialSystem(m_entityManager, m_timeService, m_frameRate);
  m_entityManager.addSystem(ComponentKind::C_SPATIAL, pSystem_t(spatialSystem));

  RenderSystem* renderSystem = new RenderSystem(m_appConfig, m_entityManager, m_buffer);
  m_entityManager.addSystem(ComponentKind::C_RENDER, pSystem_t(renderSystem));

  AnimationSystem* animationSystem = new AnimationSystem(m_entityManager);
  m_entityManager.addSystem(ComponentKind::C_ANIMATION, pSystem_t(animationSystem));

  InventorySystem* inventorySystem = new InventorySystem(m_entityManager);
  m_entityManager.addSystem(ComponentKind::C_INVENTORY, pSystem_t(inventorySystem));

  EventHandlerSystem* eventHandlerSystem = new EventHandlerSystem;
  m_entityManager.addSystem(ComponentKind::C_EVENT_HANDLER, pSystem_t(eventHandlerSystem));

  DamageSystem* damageSystem = new DamageSystem(m_entityManager);
  m_entityManager.addSystem(ComponentKind::C_DAMAGE, pSystem_t(damageSystem));

  SpawnSystem* spawnSystem = new SpawnSystem(m_entityManager, m_rootFactory, m_timeService);
  m_entityManager.addSystem(ComponentKind::C_SPAWN, pSystem_t(spawnSystem));

  AgentSystem* agentSystem = new AgentSystem(m_timeService, m_audioService);
  m_entityManager.addSystem(ComponentKind::C_AGENT, pSystem_t(agentSystem));

  FocusSystem* focusSystem = new FocusSystem(m_appConfig, m_entityManager, m_timeService);
  m_entityManager.addSystem(ComponentKind::C_FOCUS, pSystem_t(focusSystem));
}

//===========================================
// RaycastWidget::drawLoadingText
//===========================================
void RaycastWidget::drawLoadingText() {
  QPainter painter;
  painter.begin(&m_buffer);

  int h = 20;

  QFont font;
  font.setPixelSize(h);

  m_buffer.fill(Qt::black);

  painter.setFont(font);
  painter.setPen(Qt::white);
  painter.drawText((m_width - 100) / 2, (m_height - h) / 2, "Loading...");

  painter.end();
}

//===========================================
// RaycastWidget::setupTimer
//===========================================
void RaycastWidget::setupTimer() {
  m_timer = makeQtObjPtr<QTimer>(this);
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));
}

//===========================================
// RaycastWidget::setupEventHandlers
//===========================================
void RaycastWidget::setupEventHandlers() {
  m_playerImmobilised = false;

  m_entityId = Component::getNextId();
  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"immobilise_player",
    [this](const GameEvent&) {

    m_playerImmobilised = true;
    uncaptureCursor();
  }});

  auto& eventHandlerSystem = m_entityManager
    .system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  eventHandlerSystem.addComponent(pComponent_t(events));
}

//===========================================
// RaycastWidget::setCameraInRenderer
//===========================================
void RaycastWidget::setCameraInRenderer() {
  auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  renderSystem.setCamera(&spatialSystem.sg.player->camera());
}

//===========================================
// RaycastWidget::initialise
//===========================================
void RaycastWidget::initialise(const string& mapFile) {
  setupObjectFactories();

  setMouseTracking(true);
  setFocus();

  m_defaultCursor = cursor().shape();
  m_cursorCaptured = false;
  m_mouseBtnState = false;

  m_buffer = QImage(m_width, m_height, QImage::Format_ARGB32);

  setupSystems();

  m_audioService.initialise();

  loadMap(mapFile);

  setCameraInRenderer();

  setupTimer();

  drawLoadingText();

  setupEventHandlers();
}

//===========================================
// RaycastWidget::start
//===========================================
void RaycastWidget::start() {
  m_eventSystem.fire(pEvent_t{new Event{"raycast/start"}});

  m_timer->start(1000 / m_frameRate);
}

//===========================================
// RaycastWidget::paintEvent
//===========================================
void RaycastWidget::paintEvent(QPaintEvent*) {
  if (m_timer->isActive()) {
    auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    renderSystem.render();
  }

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

  if (!m_timer->isActive()) {
    return;
  }

  m_entityManager.broadcastEvent(EKeyPressed{event->key()});

  if (event->key() == Qt::Key_F) {
    DBG_PRINT("Frame rate = " << m_measuredFrameRate << "\n");
  }
  else if (event->key() == Qt::Key_Escape) {
    uncaptureCursor();
  }
}

//===========================================
// RaycastWidget::uncaptureCursor
//===========================================
void RaycastWidget::uncaptureCursor() {
  m_cursorCaptured = false;
  setCursor(m_defaultCursor);

  m_entityManager.broadcastEvent(EMouseUncaptured{});
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

  if (!m_timer->isActive() || m_playerImmobilised) {
    return;
  }

  if (m_cursorCaptured == false) {
    Point centre(width() / 2, height() / 2);
    QCursor::setPos(mapToGlobal(QPoint(centre.x, centre.y)));

    setCursor(Qt::BlankCursor);

    m_entityManager.broadcastEvent(EMouseCaptured{});
    m_cursorCaptured = true;
  }
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
  handleCursorMovement(event->x(), event->y());
}

//===========================================
// RaycastWidget::leaveEvent
//===========================================
void RaycastWidget::leaveEvent(QEvent*) {
  auto p = mapFromGlobal(QCursor::pos());
  handleCursorMovement(p.x(), p.y());
}

//===========================================
// RaycastWidget::handleCursorMovement
//===========================================
void RaycastWidget::handleCursorMovement(int x, int y) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;

  if (!player.alive || m_playerImmobilised) {
    return;
  }

  if (m_cursorCaptured) {
    setFocus();

    Point centre(width() / 2, height() / 2);

    // Y-axis is top to bottom
    Point v(x - centre.x, centre.y - y);

    if (v.x != 0 || v.y != 0) {
      QCursor::setPos(mapToGlobal(QPoint(centre.x, centre.y)));
    }

    if (fabs(v.x) > 0) {
      double da = MOUSE_LOOK_SPEED * PI * v.x;
      spatialSystem.hRotateCamera(da);
    }

    if (fabs(v.y) > 0) {
      double da = MOUSE_LOOK_SPEED * PI * v.y;
      spatialSystem.vRotateCamera(da);
    }
  }
}

//===========================================
// RaycastWidget::handleKeyboardState
//===========================================
void RaycastWidget::handleKeyboardState() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;

  if (m_keyStates[Qt::Key_E]) {
    player.jump();
  }

  if (m_keyStates[Qt::Key_Space]) {
    GameEvent e("player_activate");
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
    double ds = PLAYER_SPEED / m_frameRate;
    spatialSystem.movePlayer(normalise(v) * ds);
  }

  if (m_keyStates[Qt::Key_Left]) {
    spatialSystem.hRotateCamera(-(KEY_LOOK_SPEED / m_frameRate) * PI);
  }
  if (m_keyStates[Qt::Key_Right]) {
    spatialSystem.hRotateCamera((KEY_LOOK_SPEED / m_frameRate) * PI);
  }
}

#ifdef DEBUG
//===========================================
// RaycastWidget::measureFrameRate
//===========================================
void RaycastWidget::measureFrameRate() {
  if (m_frame % 10 == 0) {
    chrono::high_resolution_clock::time_point t_ = chrono::high_resolution_clock::now();
    chrono::duration<double> span = chrono::duration_cast<chrono::duration<double>>(t_ - m_t);
    m_measuredFrameRate = 10.0 / span.count();
    m_t = t_;
  }
  ++m_frame;
}
#endif

//===========================================
// RaycastWidget::tick
//===========================================
void RaycastWidget::tick() {
#ifdef DEBUG
  measureFrameRate();
#endif

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;

  m_entityManager.purgeEntities();
  m_entityManager.update();

  m_timeService.update();

  if (player.alive && !m_playerImmobilised) {
    handleKeyboardState();

    if (m_mouseBtnState == true) {
      player.shoot();
      m_mouseBtnState = false;
    }
  }

  update();
}

//===========================================
// RaycastWidget::~RaycastWidget
//===========================================
RaycastWidget::~RaycastWidget() {
  DBG_PRINT("RaycastWidget::~RaycastWidget\n");
}
