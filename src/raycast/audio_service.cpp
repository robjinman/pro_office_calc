#include <QFileInfo>
#include <QUrl>
#include "raycast/audio_service.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/time_service.hpp"


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

  m_musicVolume = 0.5;
  setMasterVolume(0.5);

  m_mediaPlayer.setPlaylist(&m_playlist);
}

//===========================================
// AudioService::nextAvailable
//===========================================
int AudioService::nextAvailable(const array<QSoundEffect, CONCURRENT_SOUNDS>& sounds) {
  for (int i = 0; i < CONCURRENT_SOUNDS; ++i) {
    if (!sounds[i].isPlaying()) {
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
    sound->sound[i].setSource(QUrl::fromLocalFile(absPath));
  }

  m_sounds.insert(make_pair(name, std::move(sound)));
}

//===========================================
// AudioService::soundIsPlaying
//===========================================
bool AudioService::soundIsPlaying(const string& name, int id) const {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int idx = indexForId(sound.soundIds, id);
    if (idx != -1) {
      return sound.sound[idx].isPlaying();
    }
  }

  return false;
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
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int i = nextAvailable(sound.sound);

    if (i != -1) {
      sound.sound[i].setVolume(m_masterVolume * sound.volume);
      sound.sound[i].setLoopCount(loop ? QSoundEffect::Infinite : 0);
      sound.sound[i].play();

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
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    int idx = indexForId(sound.soundIds, id);
    if (idx != -1) {
      sound.sound[idx].stop();
    }
  }
}

//===========================================
// AudioService::playSoundAtPos
//===========================================
int AudioService::playSoundAtPos(const string& name, const Point& pos, bool loop) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    const SpatialSystem& spatialSystem = m_entityManager
      .system<SpatialSystem>(ComponentKind::C_SPATIAL);

    double d = distance(spatialSystem.sg.player->pos(), pos);
    double v = 1.0 - clipNumber(d, Range(0, 2000)) / 2000;

    int i = nextAvailable(sound.sound);

    if (i != -1) {
      sound.sound[i].setVolume(m_masterVolume * sound.volume * v);
      sound.sound[i].setLoopCount(loop ? QSoundEffect::Infinite : 0);
      sound.sound[i].play();

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
