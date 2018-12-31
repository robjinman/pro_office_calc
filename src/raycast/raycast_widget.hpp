#ifndef __PROCALC_RAYCAST_RAYCAST_WIDGET_HPP__
#define __PROCALC_RAYCAST_RAYCAST_WIDGET_HPP__


#include <memory>
#include <map>
#include <QWidget>
#include <QTimer>
#include <QImage>
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/geometry.hpp"
#include "raycast/time_service.hpp"
#include "raycast/root_factory.hpp"
#include "qt_obj_ptr.hpp"
#ifdef DEBUG
#  include <chrono>
#endif


class QPaintEvent;
class AppConfig;
class EventSystem;
struct RenderGraph;
namespace parser { struct Object; }

class RaycastWidget : public QWidget {
  Q_OBJECT

  public:
    RaycastWidget(const AppConfig& appConfig, EventSystem& eventSystem, int width = 320,
      int height = 240, int frameRate = 60);

    void initialise(const std::string& mapFile);
    void start();

    EntityManager& entityManager() {
      return m_entityManager;
    }

    RootFactory& rootFactory() {
      return m_rootFactory;
    }

    TimeService& timeService() {
      return m_timeService;
    }

    AudioService& audioService() {
      return m_audioService;
    }

    ~RaycastWidget() override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private slots:
    void tick();

  private:
    void setupSystems();
    void setupObjectFactories();
    void setupTimer();
    void setupEventHandlers();
    void setCameraInRenderer();
    void drawLoadingText();
    void loadMap(const std::string& mapFilePath);
    void configure(RenderGraph& rg, const parser::Object& config);
    void configureAudioService(const parser::Object& obj);
    void loadSoundAssets(const parser::Object& obj);
    void loadMusicAssets(const parser::Object& obj);
    void loadTextures(RenderGraph& rg, const parser::Object& obj);
    void uncaptureCursor();
    void handleCursorMovement(int x, int y);
    void handleKeyboardState();
#ifdef DEBUG
    void measureFrameRate();
#endif

    const AppConfig& m_appConfig;
    EventSystem& m_eventSystem;

    EntityManager m_entityManager;
    TimeService m_timeService;
    AudioService m_audioService;
    RootFactory m_rootFactory;
    int m_width;
    int m_height;
    int m_frameRate;
    QtObjPtr<QTimer> m_timer;
    QImage m_buffer;
    std::map<int, bool> m_keyStates;
    bool m_mouseBtnState;
    bool m_cursorCaptured;
    Qt::CursorShape m_defaultCursor;
    entityId_t m_entityId;
    bool m_playerImmobilised = false;

#ifdef DEBUG
    std::chrono::high_resolution_clock::time_point m_t = std::chrono::high_resolution_clock::now();
    double m_measuredFrameRate = 0;
    long m_frame = 0;
#endif
};


#endif
