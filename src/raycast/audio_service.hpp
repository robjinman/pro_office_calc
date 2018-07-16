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


// Number of instances of each sound effect
const int CONCURRENT_SOUNDS = 8;


struct SoundEffect {
  std::array<QSoundEffect, CONCURRENT_SOUNDS> sound;
  std::array<int, CONCURRENT_SOUNDS> soundIds;

  double volume = 1.0;
};

typedef std::unique_ptr<SoundEffect> pSoundEffect_t;

class EntityManager;
class TimeService;

class AudioService {
  public:
    AudioService(EntityManager& entityManager, TimeService& timeService);

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
    int nextAvailable(const std::array<QSoundEffect, CONCURRENT_SOUNDS>& sounds);

    EntityManager& m_entityManager;
    TimeService& m_timeService;
    QMediaPlayer m_mediaPlayer;
    QMediaPlaylist m_playlist;
    std::map<std::string, pSoundEffect_t> m_sounds;
    std::map<std::string, QMediaContent> m_musicTracks;
    double m_masterVolume;
    double m_musicVolume;
};


#endif
