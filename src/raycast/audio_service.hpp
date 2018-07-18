#ifndef __PROCALC_RAYCAST_AUDIO_MANAGER_HPP__
#define __PROCALC_RAYCAST_AUDIO_MANAGER_HPP__


#include <string>
#include <memory>
#include <array>
#include <map>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMediaContent>
#include "raycast/geometry.hpp"
#include "raycast/component.hpp"


// Number of instances of each sound effect
const int CONCURRENT_SOUNDS = 8;


struct SoundInstance {
  QSoundEffect sound;
  bool positional = false;
  Point pos;
};

struct SoundEffect {
  std::array<SoundInstance, CONCURRENT_SOUNDS> instances;
  std::array<int, CONCURRENT_SOUNDS> soundIds;

  double volume = 1.0;
};

typedef std::unique_ptr<SoundEffect> pSoundEffect_t;

class EntityManager;
class TimeService;
class Player;

class AudioService {
  public:
    AudioService(EntityManager& entityManager, TimeService& timeService);

    void initialise();

    void addSound(const std::string& name, const std::string& resourcePath);
    void addMusicTrack(const std::string& name, const std::string& resourcePath);

    int playSound(const std::string& name, bool loop = false);
    int playSoundAtPos(const std::string& name, const Point& pos, bool loop = false);
    void stopSound(const std::string& name, int id);
    bool soundIsPlaying(const std::string& name, int id) const;

    void playMusic(const std::string& name, bool loop, double fadeDuration = -1);
    void stopMusic(double fadeDuration = -1);
    void setMusicVolume(double volume);

    void setMasterVolume(double volume);

  private:
    void checkInitialised() const;
    void onPlayerMove();
    double calcVolume(const SoundEffect& sound, const SoundInstance& instance) const;
    int nextAvailable(const std::array<SoundInstance, CONCURRENT_SOUNDS>& sounds);

    EntityManager& m_entityManager;
    TimeService& m_timeService;
    bool m_initialised;
    entityId_t m_entityId;
    QMediaPlayer m_mediaPlayer;
    QMediaPlaylist m_playlist;
    std::map<std::string, pSoundEffect_t> m_sounds;
    std::map<std::string, QMediaContent> m_musicTracks;
    double m_masterVolume;
    double m_musicVolume;
};


#endif
