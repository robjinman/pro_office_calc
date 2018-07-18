#include <QFileInfo>
#include <QUrl>
#include "raycast/audio_service.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/time_service.hpp"
#include "exception.hpp"


using std::make_pair;
using std::string;
using std::unique_ptr;
using std::array;


static int nextId = 0;


//===========================================
// indexForId
//===========================================
static int indexForId(const array<int, CONCURRENT_SOUNDS>& ids, int id) {
  for (int i = 0; i < CONCURRENT_SOUNDS; ++i) {
    if (ids[i] == id) {
      return i;
    }
  }

  return -1;
}

//===========================================
// AudioService::AudioService
//===========================================
AudioService::AudioService(EntityManager& entityManager, TimeService& timeService)
  : m_entityManager(entityManager),
    m_timeService(timeService) {

  m_initialised = false;
}

//===========================================
// AudioService::checkInitialised
//===========================================
void AudioService::checkInitialised() const {
  if (!m_initialised) {
    EXCEPTION("Audio service not initialised");
  }
}

//===========================================
// AudioService::initialise
//
// Should be called once the Event System exists
//===========================================
void AudioService::initialise() {
  m_musicVolume = 0.5;
  setMasterVolume(0.5);

  m_mediaPlayer.setPlaylist(&m_playlist);

  m_entityId = Component::getNextId();

  auto& eventHandlerSystem =
    m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

  CEventHandler* events = new CEventHandler(m_entityId);
  events->broadcastedEventHandlers.push_back(EventHandler{"player_move",
    [this](const GameEvent&) { onPlayerMove(); }});

  eventHandlerSystem.addComponent(pComponent_t(events));

  m_initialised = true;
}

//===========================================
// AudioService::nextAvailable
//===========================================
int AudioService::nextAvailable(const array<SoundInstance, CONCURRENT_SOUNDS>& instances) {
  for (int i = 0; i < CONCURRENT_SOUNDS; ++i) {
    if (!instances[i].sound.isPlaying()) {
      return i;
    }
  }

  return -1;
}

//===========================================
// AudioService::addSound
//===========================================
void AudioService::addSound(const string& name, const string& resourcePath) {
  pSoundEffect_t sound(new SoundEffect);

  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();

  for (int i = 0; i < CONCURRENT_SOUNDS; ++i) {
    sound->instances[i].sound.setSource(QUrl::fromLocalFile(absPath));
  }

  m_sounds.insert(make_pair(name, std::move(sound)));
}

//===========================================
// AudioService::soundIsPlaying
//===========================================
bool AudioService::soundIsPlaying(const string& name, int id) const {
  checkInitialised();

  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int idx = indexForId(sound.soundIds, id);
    if (idx != -1) {
      return sound.instances[idx].sound.isPlaying();
    }
  }

  return false;
}

//===========================================
// AudioService::onPlayerMove
//===========================================
void AudioService::onPlayerMove() {
  for (auto it = m_sounds.begin(); it != m_sounds.end(); ++it) {
    auto& sound = *it->second;

    for (auto jt = sound.instances.begin(); jt != sound.instances.end(); ++jt) {
      SoundInstance& instance = *jt;

      if (instance.positional) {
        instance.sound.setVolume(calcVolume(sound, instance));
      }
    }
  }
}

//===========================================
// AudioService::addMusicTrack
//===========================================
void AudioService::addMusicTrack(const string& name, const string& resourcePath) {
  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  m_musicTracks[name] = QMediaContent(QUrl::fromLocalFile(absPath));
}

//===========================================
// AudioService::playSound
//===========================================
int AudioService::playSound(const string& name, bool loop) {
  checkInitialised();

  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int i = nextAvailable(sound.instances);

    if (i != -1) {
      sound.instances[i].positional = false;
      sound.instances[i].sound.setVolume(m_masterVolume * sound.volume);
      sound.instances[i].sound.setLoopCount(loop ? QSoundEffect::Infinite : 0);
      sound.instances[i].sound.play();

      long id = ++nextId;
      sound.soundIds[i] = id;
      return id;
    }
  }

  return -1;
}

//===========================================
// AudioService::stopSound
//===========================================
void AudioService::stopSound(const string& name, int id) {
  checkInitialised();

  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int idx = indexForId(sound.soundIds, id);
    if (idx != -1) {
      sound.instances[idx].sound.stop();
    }
  }
}

//===========================================
// AudioService::calcVolume
//===========================================
double AudioService::calcVolume(const SoundEffect& sound, const SoundInstance& instance) const {
  if (!instance.positional) {
    return m_masterVolume * sound.volume;
  }

  auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  double d = distance(spatialSystem.sg.player->pos(), instance.pos);
  double v = 1.0 - clipNumber(d, Range(0, 2000)) / 2000;

  return m_masterVolume * sound.volume * v;
}

//===========================================
// AudioService::playSoundAtPos
//===========================================
int AudioService::playSoundAtPos(const string& name, const Point& pos, bool loop) {
  checkInitialised();

  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int i = nextAvailable(sound.instances);
    if (i != -1) {
      sound.instances[i].positional = true;
      sound.instances[i].pos = pos;
      sound.instances[i].sound.setVolume(calcVolume(sound, sound.instances[i]));
      sound.instances[i].sound.setLoopCount(loop ? QSoundEffect::Infinite : 0);
      sound.instances[i].sound.play();

      long id = ++nextId;
      sound.soundIds[i] = id;
      return id;
    }
  }

  return -1;
}

//===========================================
// AudioService::playMusic
//===========================================
void AudioService::playMusic(const string& name, bool loop, double fadeDuration) {
  checkInitialised();

  // TODO: Fade

  auto it = m_musicTracks.find(name);

  if (it != m_musicTracks.end()) {
    m_playlist.clear();
    m_playlist.addMedia(it->second);
    m_playlist.setPlaybackMode(loop ? QMediaPlaylist::Loop : QMediaPlaylist::CurrentItemOnce);

    m_mediaPlayer.play();
  }
}

//===========================================
// AudioService::stopMusic
//===========================================
void AudioService::stopMusic(double fadeDuration) {
  checkInitialised();

  if (fadeDuration <= 0.0) {
    m_mediaPlayer.stop();
    return;
  }

  double delta = m_musicVolume / (fadeDuration * m_timeService.frameRate);

  Tween tween{[this, delta](long, double, double) -> bool {
    setMusicVolume(m_musicVolume - delta);
    return m_musicVolume > 0.0;
  }, [this](long, double, double) {
    m_mediaPlayer.stop();
  }};

  m_timeService.addTween(std::move(tween));
}

//===========================================
// AudioService::setMusicVolume
//===========================================
void AudioService::setMusicVolume(double volume) {
  m_musicVolume = volume;
  m_mediaPlayer.setVolume(m_masterVolume * m_musicVolume * 100);
}

//===========================================
// AudioService::setMasterVolume
//===========================================
void AudioService::setMasterVolume(double volume) {
  m_masterVolume = volume;
  setMusicVolume(m_musicVolume);
}
