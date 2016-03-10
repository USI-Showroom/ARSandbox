#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <QMediaPlayer>


#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

class Animal : QObject
{
    Q_OBJECT
private:
    Point2d _position, _direction;
    int _life, _maxLife;
    int _decisionTicks;
    int _soundTicks;

    double _minH, _maxH;
    double _speed;
    double _angle;
    bool _resurrected;

    bool _playSound;
    QMediaPlayer _sound;

public:
    Animal(const Point2d newPosition, const double minH, const double maxH, const bool playSound);
    Animal(const double minH, const double maxH, const bool playSound);
    virtual ~Animal();

    void think(const UnitSquareMapping &mapping);
    void update();

    inline const Point2d &position() const { return _position; }
    inline bool alive() const { return _life>0; }

    inline void setPosition(const double newX, const double newY)
    {
        _position.x() = newX;
        _position.y() = newY;
    }

    inline void setLife(const int newLife)
    {
        _life = newLife;
    }

    inline const Point2d &direction() const { return _direction; }

private:
    void resurrect(const UnitSquareMapping &mapping);
    void newDirection(const UnitSquareMapping &mapping);
    void move();

    double randRange(const double min, const double max);

};

#endif //ANIMAL_HPP
