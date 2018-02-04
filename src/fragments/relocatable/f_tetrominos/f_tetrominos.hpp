#ifndef __PROCALC_FRAGMENTS_F_TETROMINOS_HPP__
#define __PROCALC_FRAGMENTS_F_TETROMINOS_HPP__


#include <list>
#include <memory>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPolygon>
#include "fragment.hpp"
#include "event.hpp"


class EventSystem;

struct FTetrominosData : public FragmentData {
  std::unique_ptr<QTimer> timer;
};

class FTetrominos : public QLabel, public Fragment {
  Q_OBJECT

  public:
    FTetrominos(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FTetrominos() override;

  private slots:
    void tick();

  private:
    struct Tetromino {
      enum kind_t {
        I = 0,
        J = 1,
        L = 2,
        O = 3,
        S = 4,
        T = 5,
        Z = 6
      };

      kind_t kind;
      double x;
      double y;
      double a;

      double dy;
      double da;

      std::array<QPolygon, 4> blocks;
      QColor colour;
    };

    FTetrominosData m_data;
    std::list<Tetromino> m_tetrominos;
    std::unique_ptr<QImage> m_buffer;

    void constructTetrominos();
    void moveTetrominos();
    void drawTetrominos(QImage& buffer);
};


#endif
